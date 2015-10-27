; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   CONECTANDO O PIC - RECURSOS AVANÇADOS                 *
; *                               EXEMPLO 5                                 *
; *                                                                         *
; *                NICOLÁS CÉSAR LAVINIA e DAVID JOSÉ DE SOUZA              *
; *                                                                         *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *   VERSÃO : 2.0                                                          *
; *     DATA : 24/02/2003                                                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                             DESCRIÇÃO GERAL                             *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DE UM TIPO DE
;  CONVERSOR A/D FUNDAMENTADO NO TEMPO DE CARGA DE UM CAPACITOR. O TEMPO DE
;  CARGA DO CAPACITOR É MOSTRADO NO LCD E É INVERSAMENTE PROPORCIONAL À
;  TENSÃO APLICADA ATRVÉS DO POTENCIÔMETRO P2.
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         CONFIGURAÇÕES PARA GRAVAÇÃO                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

 __CONFIG _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_ON & _WDT_ON & _XT_OSC

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                            DEFINIÇÃO DAS VARIÁVEIS                      *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE BLOCO DE VARIÁVEIS ESTÁ LOCALIZADO LOGO NO INÍCIO DO BANCO 0

	CBLOCK	0X20			; POSIÇÃO INICIAL DA RAM

		TEMPO1
		TEMPO0			; CONTADORES P/ DELAY

		FILTRO_BOTOES		; FILTRO PARA RUIDOS DOS BOTÕES

		CONTADOR_AD		; CONTADOR PARA CONVERSOR A/D

		AUX			; REGISTRADOR AUXILIAR DE USO GERAL

	ENDC

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                DEFINIÇÃO DAS VARIÁVEIS INTERNAS DO PIC                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  O ARQUIVO DE DEFINIÇÕES DO PIC UTILIZADO DEVE SER REFERENCIADO PARA QUE
;  OS NOMES DEFINIDOS PELA MICROCHIP POSSAM SER UTILIZADOS, SEM A NECESSIDADE
;  DE REDIGITAÇÃO.

	#INCLUDE <P16F877A.INC>		; MICROCONTROLADOR UTILIZADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      DEFINIÇÃO DOS BANCOS DE RAM                        *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  OS PSEUDOS-COMANDOS "BANK0" E "BANK1", AQUI DEFINIDOS, AJUDAM A COMUTAR
;  ENTRE OS BANCOS DE MEMÓRIA.

#DEFINE	BANK1	BSF	STATUS,RP0 	; SELECIONA BANK1 DA MEMORIA RAM
#DEFINE	BANK0	BCF	STATUS,RP0	; SELECIONA BANK0 DA MEMORIA RAM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                           CONSTANTES INTERNAS                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINIÇÃO DE CONSTANTES FACILITA A PROGRAMAÇÃO E A MANUTENÇÃO.

FILTRO_TECLA	EQU	.200		; FILTRO P/ EVITAR RUIDOS DOS BOTÕES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                    DECLARAÇÃO DOS FLAGs DE SOFTWARE                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINIÇÃO DE FLAGs AJUDA NA PROGRAMAÇÃO E ECONOMIZA MEMÓRIA RAM.

;  ESTE PROGRAMA NÃO UTILIZA NENHUM FLAG DE USUÁRIO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                ENTRADAS                                 *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS ENTRADAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
;  FUTURAS ALTERAÇÕES DO HARDWARE.

#DEFINE	PINO_AD		TRISA,1		; PINO P/ LEITURA DO RC
					; 0 -> FORÇA A DESCARGA DO CAPACITOR
					; 1 -> LIBERA A CARGA DO CAPACITOR

#DEFINE	CAD		PORTA,1		; PINO P/ LEITURA DO CONV. A/D
					; 0 -> CAPACITOR DESCARREGADO
					; 1 -> CAPACITOR CARREGADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                 SAÍDAS                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
;  FUTURAS ALTERAÇÕES DO HARDWARE.

#DEFINE	DISPLAY		PORTD		; BARRAMENTO DE DADOS DO DISPLAY

#DEFINE	RS		PORTE,0		; INDICA P/ DISPLAY UM DADO OU COMANDO
					; 1 -> DADO
					; 0 -> COMANDO

#DEFINE	ENABLE		PORTE,1		; SINAL DE ENABLE P/ DISPLAY
					; ATIVO NA BORDA DE SUBIDA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   VETOR DE RESET DO MICROCONTROLADOR                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  POSIÇÃO INICIAL PARA EXECUÇÃO DO PROGRAMA

	ORG	0X000			; ENDEREÇO DO VETOR DE RESET
	GOTO	CONFIG_			; PULA PARA CONFIG DEVIDO A REGIÃO
					; DESTINADA AS ROTINAS SEGUINTES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   ROTINA DE DELAY (DE 1MS ATÉ 256MS)                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA É UMA ROTINA DE DELAY VARIÁVEL, COM DURAÇÃO DE 1MS X O VALOR PASSADO
;  EM WORK (W).

DELAY_MS
	MOVWF	TEMPO1			; CARREGA TEMPO1 (UNIDADES DE MS)
	MOVLW	.250
	MOVWF	TEMPO0			; CARREGA TEMPO0 (P/ CONTAR 1MS)

	CLRWDT				; LIMPA WDT (PERDE TEMPO)
	DECFSZ	TEMPO0,F		; FIM DE TEMPO0 ?
	GOTO	$-2			; NÃO - VOLTA 2 INSTRUÇÕES
					; SIM - PASSOU-SE 1MS
	DECFSZ	TEMPO1,F		; FIM DE TEMPO1 ?
	GOTO	$-6			; NÃO - VOLTA 6 INSTRUÇÕES
					; SIM
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA DE UM CARACTER NO DISPLAY               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ENVIA UM CARACTER PARA O MÓDULO DE LCD. O CARACTER A SER
;  ESCRITO DEVE SER COLOCADO EM WORK (W) ANTES DE CHAMAR A ROTINA.

ESCREVE
	MOVWF	DISPLAY			; ATUALIZA DISPLAY (PORTD)
	NOP				; PERDE 1US PARA ESTABILIZAÇÃO
	BSF	ENABLE			; ENVIA UM PULSO DE ENABLE AO DISPLAY
	GOTO	$+1			; .
	BCF	ENABLE			; .

	MOVLW	.1
	CALL	DELAY_MS		; DELAY DE 1MS
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                CONFIGURAÇÕES INICIAIS DE HARDWARE E SOFTWARE            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  NESTA ROTINA SÃO INICIALIZADAS AS PORTAS DE I/O DO MICROCONTROLADOR E AS
;  CONFIGURAÇÕES DOS REGISTRADORES ESPECIAIS (SFR). A ROTINA INICIALIZA AS
;  VARIÁVEIS DE RAM E AGUARDA O ESTOURO DO WDT.

CONFIG_
	CLRF	PORTA			; GARANTE AS SAÍDAS EM ZERO
	CLRF	PORTB
	CLRF	PORTC
	CLRF	PORTD
	CLRF	PORTE

	BANK1				; SELECIONA BANCO 1 DA RAM

	MOVLW	B'11011111'
	MOVWF	TRISA			; CONFIGURA I/O DO PORTA

	MOVLW	B'11111111'
	MOVWF	TRISB			; CONFIGURA I/O DO PORTB

	MOVLW	B'11111111'
	MOVWF	TRISC			; CONFIGURA I/O DO PORTC

	MOVLW	B'00000000'
	MOVWF	TRISD			; CONFIGURA I/O DO PORTD

	MOVLW	B'00000100'
	MOVWF	TRISE			; CONFIGURA I/O DO PORTE

	MOVLW	B'11011111'
	MOVWF	OPTION_REG		; CONFIGURA OPTIONS
					; PULL-UPs DESABILITADOS
					; INTER. NA BORDA DE SUBIDA DO RB0
					; TIMER0 INCREM. PELO CICLO DE MÁQUINA
					; WDT   - 1:128
					; TIMER - 1:1
					
	MOVLW	B'00000000'		
	MOVWF	INTCON			; CONFIGURA INTERRUPÇÕES
					; DESABILITADA TODAS AS INTERRUPÇÕES

	MOVLW	B'00000111'
	MOVWF	ADCON1			; CONFIGURA CONVERSOR A/D
					; CONFIGURA PORTA COM I/O DIGITAL

	BANK0				; SELECIONA BANCO 0 DA RAM

;  AS INTRUÇÕES A SEGUIR FAZEM COM QUE O PROGRAMA TRAVE QUANDO HOUVER UM
;  RESET OU POWER-UP, MAS PASSE DIRETO SE O RESET FOR POR WDT. DESTA FORMA,
;  SEMPRE QUE O PIC É LIGADO, O PROGRAMA TRAVA, AGUARDA UM ESTOURO DE WDT
;  E COMEÇA NOVAMENTE. ISTO EVITA PROBLEMAS NO START-UP DO PIC.

	BTFSC	STATUS,NOT_TO		; RESET POR ESTOURO DE WATCHDOG TIMER ?
	GOTO	$			; NÃO - AGUARDA ESTOURO DO WDT
					; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         INICIALIZAÇÃO DA RAM                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA IRÁ LIMPARÁ TODA A RAM DO BANCO 0, INDO DE 0X20 A 0X7F

	MOVLW	0X20
	MOVWF	FSR			; APONTA O ENDEREÇAMENTO INDIRETO PARA
					; A PRIMEIRA POSIÇÃO DA RAM
LIMPA_RAM
	CLRF	INDF			; LIMPA A POSIÇÃO
	INCF	FSR,F			; INCREMENTA O PONTEIRO P/ A PRÓX. POS.
	MOVF	FSR,W
	XORLW	0X80			; COMPARA O PONTEIRO COM A ÚLT. POS. +1
	BTFSS	STATUS,Z		; JÁ LIMPOU TODAS AS POSIÇÕES?
	GOTO	LIMPA_RAM		; NÃO - LIMPA A PRÓXIMA POSIÇÃO
					; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      CONFIGURAÇÕES INICIAIS DO DISPLAY                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA INICIALIZA O DISPLAY P/ COMUNICAÇÃO DE 8 VIAS, DISPLAY PARA 2
; LINHAS, CURSOR APAGADO E DESLOCAMENTO DO CURSOR À DIREITA. 

INICIALIZACAO_DISPLAY
	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZAÇÃO

	MOVLW	.3
	CALL	DELAY_MS		; DELAY DE 3MS (EXIGIDO PELO DISPLAY)

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZAÇÃO

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZAÇÃO

	MOVLW	B'00111000'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; INTERFACE DE 8 VIAS DE DADOS

	MOVLW	B'00000001'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; LIMPAR TODO O DISPLAY

	MOVLW	.1
	CALL	DELAY_MS		; DELAY DE 1MS

	MOVLW	B'00001100'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; LIGAR O DISPLAY SEM CURSOR

	MOVLW	B'00000110'		; ESCREVE COMANDO PARA INCREM.
	CALL	ESCREVE			; AUTOMÁTICO A ESQUERDA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA DA TELA PRINCIPAL                       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A TELA PRINCIPAL DO PROGRAMA, COM AS FRASES:
;  LINHA 1 - "A/D por RC  (P2)"
;  LINHA 2 - "T.CARGA:   x50us"

	MOVLW	0X80			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 0 / COLUNA 0

	BSF	RS			; SELECIONA O DISPLAY P/ DADOS
					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "A/D por RC (P2)"
	MOVLW	'A'
	CALL	ESCREVE
	MOVLW	'/'
	CALL	ESCREVE
	MOVLW	'D'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'p'
	CALL	ESCREVE
	MOVLW	'o'
	CALL	ESCREVE
	MOVLW	'r'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	'C'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'('
	CALL	ESCREVE
	MOVLW	'P'
	CALL	ESCREVE
	MOVLW	'2'
	CALL	ESCREVE
	MOVLW	')'
	CALL	ESCREVE

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS

	MOVLW	0XC0			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 0

	BSF	RS			; SELECIONA O DISPLAY P/ DADOS
					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "T.CARGA:   x50us"
	MOVLW	'T'
	CALL	ESCREVE
	MOVLW	'.'
	CALL	ESCREVE
	MOVLW	'C'
	CALL	ESCREVE
	MOVLW	'A'
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	'G'
	CALL	ESCREVE
	MOVLW	'A'
	CALL	ESCREVE
	MOVLW	':'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'x'
	CALL	ESCREVE
	MOVLW	'5'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	'u'
	CALL	ESCREVE
	MOVLW	's'
	CALL	ESCREVE

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *      ROTINA PARA DESCARREGAR O CAPACITOR DE LEITURA DO CONVERSOR A/D    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA CONVERTE O PINO DO MICROCONTROLADOR EM SAÍDA COM NÍVEL LÓGICO 0
; E AGUARDA QUE O CAPACITOR SE DESCARREGUE. EM SEGUIDA O PINO É CONVERTIDO
; NOVAMENTE EM ENTRADA PARA PERMITIR QUE O CAPACITOR DE CARREGUE.

DESCARGA_CAPACITOR
	CLRWDT				; LIMPA WATCHDOG TIMER

	CLRF	CONTADOR_AD		; ZERA O CONTADOR DE TEMPO DE CARGA
					; DO CAPACITOR

	BANK1				; SELECIONA BANCO 1 DA RAM
	BCF	PINO_AD			; TRANSFORMA PINO EM SAIDA
	BANK0				; VOLTA P/ BANCO 0 DA RAM
	BCF	CAD			; DESCARREGA O CAPACITOR

	MOVLW	.3
	CALL	DELAY_MS		; CHAMA ROTINA DE DELAY (3ms)
					; TEMPO NECESSÁRIO P/ DESCARGA
					; DO CAPACITOR

	BANK1				; SELECIONA BANCO 1 DA RAM
	BSF	PINO_AD			; TRANSFORMA PINO EM ENTRADA
	BANK0				; VOLTA P/ BANCO 0 DA RAM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      LOOP P/ ESPERAR CARGA DO CAPACITOR                 *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; O TEMPO CONTA O TEMPO QUE O CAPACITOR LEVA PARA ATINGIR UM NÍVEL DE TENSÃO
; SUFICIENTE PARA QUE O MICROCONTROLADOR ENTENDA NÍVEL LÓGICO 1 NA ENTRADA TTL
; DO PINO RA1. CASO O CAPACITOR NUNCA SE DEMORE MAIS DO QUE 256 CICLOS DESTE
; LOOP, A ROTINA DESVIA PARA UMA ROTINA DE SATURAÇÃO.
; O LOOP DA ROTINA É DE 50us (CRISTAL DE 4MHz).

LOOP_CAD
	NOP				; [1us]

	MOVLW	.14			; [2us]
	MOVWF	AUX			; [3us] - CARREGA AUX COM 14d
	DECFSZ	AUX,F
	GOTO	$-1			; [4us] À [44us] - DELAY

	INCFSZ	CONTADOR_AD,F		; INCREM. CONTADOR E VERIFICA ESTOURO
	GOTO	$+2			; NÃO HOUVE ESTOURO - PULA 1 INSTRUÇÃO
	GOTO	SATURACAO		; HOUVE ESTOURO - PULA P/ SATURAÇÃO

	BTFSS	CAD			; CAPACITOR JÁ CARREGOU ?
	GOTO	LOOP_CAD		; NÃO - VOLTA P/ LOOP_CAD
	GOTO	MOSTRA_CONTADOR		; SIM - MOSTRA TEMPO DE CARGA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *              MOSTRA O TEMPO DE CARGA DO CAPACITOR NO LCD                *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA MOSTRA O TEMPO DE CARGA DO CAPACITOR EM HAXADECIMAL NO LCD.
; CASO O CAPACITOR NÃO TENHA SE CARREGADO, A ROTINA DE SATURAÇÃO GARANTE
; UM VALOR MÁXIMO PARA O TEMPO DE CARGA (0xFF).

SATURACAO
	MOVLW	0XFF
	MOVWF	CONTADOR_AD		; SATURA O CONTADOR
					; (CAPACITOR NÃO CARREGOU)

MOSTRA_CONTADOR
	MOVLW	0XC9			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 9

	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	SWAPF	CONTADOR_AD,W		; INVERTE NIBLE DO CONTADOR_AD
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO E POSITIVO? (É UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; NÃO - WORK FICA COM 30h (NÚMERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERSÃO ASCII)

	CALL	ESCREVE			; MOSTRA NO DISPLAY

	MOVF	CONTADOR_AD,W		; CARREGA NO WORK O CONTADOR_AD
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO E POSITIVO? (É UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; NÃO - WORK FICA COM 30h (NÚMERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERSÃO ASCII)

	CALL	ESCREVE			; MOSTRA NO DISPLAY

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS

	GOTO	DESCARGA_CAPACITOR	; VOLTA P/ DESCARREGAR O CAPACITOR

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              FIM DO PROGRAMA                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END				; FIM DO PROGRAMA

