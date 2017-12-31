; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   CONECTANDO O PIC - RECURSOS AVANÇADOS                 *
; *                               EXEMPLO 6                                 *
; *                                                                         *
; *                NICOLÁS CÉSAR LAVINIA e DAVID JOSÉ DE SOUZA              *
; *                                                                         *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *   VERSÃO : 2.0                                                          *
; *     DATA : 24/02/2003                                                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                             DESCRIÇÃO GERAL                             *
; *                                                                         *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DO MÓDULO PWM
;  DO PIC16F877. ELE MONITORA OS QUATRO BOTÕES E CONFORME O BOTÃO SELECIONADO
;  APLICA UM VALOR DIFERENTE NO PWM, FAZENDO ASSIM UM CONTROLE SOBRE A
;  VELOCIDADE DO VENTILADOR. NO LCD É MOSTRADO O VALOR ATUAL DO DUTY CYCLE.
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

		FILTRO_BOTOES		; FILTRO PARA RUIDOS

		TEMPO1
		TEMPO0			; CONTADORES P/ DELAY

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

#DEFINE	BOTAO_0		PORTB,0		; ESTADO DO BOTÃO 0
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_1		PORTB,1		; ESTADO DO BOTÃO 1
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_2		PORTB,2		; ESTADO DO BOTÃO 2
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_3		PORTB,3		; ESTADO DO BOTÃO 3
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                 SAÍDAS                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
;  FUTURAS ALTERAÇÕES DO HARDWARE.

#DEFINE	DISPLAY		PORTD		; BARRAMENTO DE DADOS DO DISPLAY

#DEFINE	RS		PORTE,0		; INDICA P/ O DISPLAY UM DADO OU COMANDO
					; 1 -> DADO
					; 0 -> COMANDO

#DEFINE	ENABLE		PORTE,1		; SINAL DE ENABLE P/ DISPLAY
					; ATIVO NA BORDA DE DESCIDA

#DEFINE	VENTILADOR	PORTC,1		; SAÍDA P/ O VENTILADOR
					; 1 -> VENTILADOR LIGADO
					; 0 -> VENTILADOR DESLIGADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   VETOR DE RESET DO MICROCONTROLADOR                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  POSIÇÃO INICIAL PARA EXECUÇÃO DO PROGRAMA

	ORG	0X0000			; ENDEREÇO DO VETOR DE RESET
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
;  CONFIGURAÇÕES DOS REGISTRADORES ESPECIAIS (SFR). A ROTINA INICIALIZA A
;  MÁQUINA E AGUARDA O ESTOURO DO WDT.

CONFIG_
	CLRF	PORTA			; GARANTE TODAS AS SAÍDAS EM ZERO
	CLRF	PORTB
	CLRF	PORTC
	CLRF	PORTD
	CLRF	PORTE

	BANK1				; SELECIONA BANCO 1 DA RAM

	MOVLW	B'11111111'
	MOVWF	TRISA			; CONFIGURA I/O DO PORTA

	MOVLW	B'11111111'
	MOVWF	TRISB			; CONFIGURA I/O DO PORTB

	MOVLW	B'11111101'
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
					; CONFIGURA PORTA E PORTE COMO I/O DIGITAL

	MOVLW	.255
	MOVWF	PR2			; CONFIGURA PERÍODO DO PWM
					; T=((PR2)+1)*4*Tosc*TMR2 Prescale
					; T=((255)+1)*4*250ns*16
					; T=4,096ms -> 244,14Hz

	BANK0				; SELECIONA BANCO 0 DA RAM

	MOVLW	B'00000111'
	MOVWF	T2CON			; CONFIGURA TMR2
					; TIMER 2 LIGADO
					; PRESCALE  - 1:16
					; POSTSCALE - 1:1

	MOVLW	B'00001111'
	MOVWF	CCP2CON			; CONFIGURA CCP2CON PARA PWM
					; (PINO RC1)

	CLRF	CCPR2L			; INICIA COM DUTY CYCLE EM ZERO

;  AS INSTRUÇÕES A SEGUIR FAZEM COM QUE O PROGRAMA TRAVE QUANDO HOUVER UM
;  RESET OU POWER-UP, MAS PASSE DIRETO SE O RESET FOR POR WDT. DESTA FORMA,
;  SEMPRE QUE O PIC É LIGADO, O PROGRAMA TRAVA, AGUARDA UM ESTOURO DE WDT
;  E COMEÇA NOVAMENTE. ISTO EVITA PROBLEMAS NO START-UP DO PIC.

	BTFSC	STATUS,NOT_TO		; RESET POR ESTOURO DE WATCHDOG TIMER ?
	GOTO	$			; NÃO - AGUARDA ESTOURO DO WDT
					; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         INICIALIZAÇÃO DA RAM                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA IRÁ LIMPAR TODA A RAM DO BANCO 0, INDO DE 0X20 A 0X7F.
;  EM SEGUIDA, AS VARIÁVEIS DE RAM DO PROGRAMA SÃO INICIALIZADAS.

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
	CALL	ESCREVE			; AUTOMÁTICO À DIREITA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA DA TELA PRINCIPAL                       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A TELA PRINCIPAL DO PROGRAMA, COM AS FRASES:
;  LINHA 1 - "CURSO MODULO 2" 
;  LINHA 2 - "   PWM:  xx%  "

MOSTRA_TELA_PRINCIPAL
	MOVLW	0X81			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 0 / COLUNA 1
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "CURSO MODULO 2"
	MOVLW	'C'
	CALL	ESCREVE
	MOVLW	'U'
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	'S'
	CALL	ESCREVE
	MOVLW	'O'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'M'
	CALL	ESCREVE
	MOVLW	'O'
	CALL	ESCREVE
	MOVLW	'D'
	CALL	ESCREVE
	MOVLW	'U'
	CALL	ESCREVE
	MOVLW	'L'
	CALL	ESCREVE
	MOVLW	'O'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'2'
	CALL	ESCREVE

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0XC3			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 3
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "   PWM:  OFF    "
	MOVLW	'P'
	CALL	ESCREVE
	MOVLW	'W'
	CALL	ESCREVE
	MOVLW	'M'
	CALL	ESCREVE
	MOVLW	':'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'O'
	CALL	ESCREVE
	MOVLW	'F'
	CALL	ESCREVE
	MOVLW	'F'
	CALL	ESCREVE

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                          VARREDURA DOS BOTÕES                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA VERIFICA SE ALGUM BOTÃO ESTÁ PRESSIONADO E CASO AFIRMATIVO
; DESVIA PARA O TRATAMENTO DO MESMO.

VARRE
	CLRWDT				; LIMPA WATCHDOG TIMER

; **************** VERIFICA ALGUM BOTÃO PRESSIONADO *************************

VARRE_BOTOES
	BTFSS	BOTAO_0			; O BOTÃO 0 ESTA PRESSIONADO ?
	GOTO	TRATA_BOTAO_0		; SIM - PULA P/ TRATA_BOTAO_0
					; NÃO

	BTFSS	BOTAO_1			; O BOTÃO 1 ESTA PRESSIONADO ?
	GOTO	TRATA_BOTAO_1		; SIM - PULA P/ TRATA_BOTAO_1
					; NÃO

	BTFSS	BOTAO_2			; O BOTÃO 2 ESTA PRESSIONADO ?
	GOTO	TRATA_BOTAO_2		; SIM - PULA P/ TRATA_BOTAO_2
					; NÃO

	BTFSS	BOTAO_3			; O BOTÃO 3 ESTA PRESSIONADO ?
	GOTO	TRATA_BOTAO_3		; SIM - PULA P/ TRATA_BOTAO_3
					; NÃO

; *************************** FILTRO P/ EVITAR RUIDOS ***********************

	MOVLW	FILTRO_TECLA		; CARREGA O VALOR DE FILTRO_TECLA
	MOVWF	FILTRO_BOTOES		; SALVA EM FILTRO_BOTOES
					; RECARREGA FILTRO P/ EVITAR RUIDOS
					; NOS BOTÕES

	GOTO	VARRE			; VOLTA PARA VARRER TECLADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         TRATAMENTO DOS BOTÕES                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; NESTE TRECHO DO PROGRAMA ESTÃO TODOS OS TRATAMENTOS DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 0 ***************************

TRATA_BOTAO_0
	MOVF	FILTRO_BOTOES,F
	BTFSC	STATUS,Z		; FILTRO JÁ IGUAL A ZERO ?
					; (FUNÇÃO JA FOI EXECUTADA?)		
	GOTO	VARRE			; SIM - VOLTA P/ VARREDURA DO TECLADO
					; NÃO
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO

	CLRF	CCPR2L			; ZERA CCPR2L
	BCF	CCP2CON,5		; ZERA OS BITS 5 e 4 
	BCF	CCP2CON,4		; (LSB DO DUTY CYCLE)
				; Tp = CCPR2L:CCP2CON<5,4>*Tosc*TMR2 Prescale
				; Tp = 0 * 250ns * 16
				; Tp = 0
				; PWM -> DUTY CYCLE = 0% -> OFF 

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0XC8			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 8
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE " OFF"
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'O'
	CALL	ESCREVE
	MOVLW	'F'
	CALL	ESCREVE
	MOVLW	'F'
	CALL	ESCREVE

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 1 ***************************

TRATA_BOTAO_1
	MOVF	FILTRO_BOTOES,F
	BTFSC	STATUS,Z		; FILTRO JÁ IGUAL A ZERO ?
					; (FUNÇÃO JA FOI EXECUTADA?)		
	GOTO	VARRE			; SIM - VOLTA P/ VARREDURA DO TECLADO
					; NÃO
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO
	MOVLW	0X80
	MOVWF	CCPR2L			; CARREGA CCPR2L COM 0X80
	BCF	CCP2CON,5		; LIMPA OS BITS 5 e 4
	BCF	CCP2CON,4		; LSB DO DUTY CYCLE
				; Tp = CCPR2L:CCP2CON<5,4>*Tosc*TMR2 Prescale
				; Tp = 512 * 250ns * 16
				; Tp = 2,048ms
				; PWM -> DUTY CYCLE = 50%

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0XC8			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 8
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE " 50%"
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'5'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	'%'
	CALL	ESCREVE

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 2 ***************************

TRATA_BOTAO_2
	MOVF	FILTRO_BOTOES,F
	BTFSC	STATUS,Z		; FILTRO JÁ IGUAL A ZERO ?
					; (FUNÇÃO JA FOI EXECUTADA?)		
	GOTO	VARRE			; SIM - VOLTA P/ VARREDURA DO TECLADO
					; NÃO
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO
	MOVLW	0XC0
	MOVWF	CCPR2L			; CARREGA CCPR2L COM 0XC0
	BCF	CCP2CON,5		; LIMPA OS BITS 5 e 4
	BCF	CCP2CON,4		; LSB DO DUTY CYCLE
				; Tp = CCPR2L:CCP2CON<5,4>*Tosc*TMR2 Prescale
				; Tp = 768 * 250ns * 16
				; Tp = 3,072ms
				; PWM -> DUTY CYCLE = 75%

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0XC8			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 8
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "75%"
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'7'
	CALL	ESCREVE
	MOVLW	'5'
	CALL	ESCREVE
	MOVLW	'%'
	CALL	ESCREVE

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 3 ***************************

TRATA_BOTAO_3
	MOVF	FILTRO_BOTOES,F
	BTFSC	STATUS,Z		; FILTRO JÁ IGUAL A ZERO ?
					; (FUNÇÃO JA FOI EXECUTADA?)		
	GOTO	VARRE			; SIM - VOLTA P/ VARREDURA DO TECLADO
					; NÃO
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO
	MOVLW	0XFF
	MOVWF	CCPR2L			; CARREGA CCPR2L COM 0XFF
	BSF	CCP2CON,5		; SETA OS BITS 5 e 4 
	BSF	CCP2CON,4		; LSB DO DUTY CYCLE
				; Tp = CCPR2L:CCP2CON<5,4>*Tosc*TMR2 Prescale
				; Tp = 1023 * 250ns * 16
				; Tp = 4,092ms
				; PWM -> DUTY CYCLE = 99,90%

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0XC8			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 8
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "100%"
	MOVLW	'1'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	'%'
	CALL	ESCREVE

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              FIM DO PROGRAMA                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END				; FIM DO PROGRAMA

