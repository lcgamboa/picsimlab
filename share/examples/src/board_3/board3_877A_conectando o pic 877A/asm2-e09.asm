; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   CONECTANDO O PIC - RECURSOS AVAN�ADOS                 *
; *                               EXEMPLO 9                                 *
; *                                                                         *
; *                NICOL�S C�SAR LAVINIA e DAVID JOS� DE SOUZA              *
; *                                                                         *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *   VERS�O : 2.0                                                          *
; *     DATA : 24/02/2003                                                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                             DESCRI��O GERAL                             *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA USART DO PIC.
;  O SOFTWARE CONVERTE O CANAL 1 DO CONVERSOR A/D (POTENCI�METRO P2) E MOSTRA
;  NO DISPLAY O VALOR CONVERTIDO EM DECIMAL E HAXADECIMAL.
;  AL�M DE MOSTRAR O VALOR NO DISPLAY, O SOFTWARE TRANSMITE PELA USART O VALOR
;  DA CONVERS�O. OS VALORES RECEBIDOS PELA USART TAMB�M S�O MOSTRADOS NO LCD
;  COMO CARACTERES ASCII.
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         CONFIGURA��ES PARA GRAVA��O                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

 __CONFIG _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_ON & _WDT_ON & _XT_OSC

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                            DEFINI��O DAS VARI�VEIS                      *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE BLOCO DE VARI�VEIS EST� LOCALIZADO LOGO NO IN�CIO DO BANCO 0

	CBLOCK	0X20			; POSI��O INICIAL DA RAM

		TEMPO0
		TEMPO1			; TMEPORIZADORES P/ ROTINA DE DELAY

		AUX			; REGISTRADOR AUXILIAR DE USO GERAL

		UNIDADE			; ARMAZENA VALOR DA UNIDADE
		DEZENA			; ARMAZENA VALOR DA DEZENA
		CENTENA			; ARMAZENA VALOR DA CENTENA

	ENDC

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                DEFINI��O DAS VARI�VEIS INTERNAS DO PIC                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  O ARQUIVO DE DEFINI��ES DO PIC UTILIZADO DEVE SER REFERENCIADO PARA QUE
;  OS NOMES DEFINIDOS PELA MICROCHIP POSSAM SER UTILIZADOS, SEM A NECESSIDADE
;  DE REDIGITA��O.

	#INCLUDE <P16F877A.INC>		; MICROCONTROLADOR UTILIZADO


; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      DEFINI��O DOS BANCOS DE RAM                        *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  OS PSEUDOS-COMANDOS "BANK0" E "BANK1", AQUI DEFINIDOS, AJUDAM A COMUTAR
;  ENTRE OS BANCOS DE MEM�RIA.

#DEFINE	BANK1	BSF	STATUS,RP0 	; SELECIONA BANK1 DA MEMORIA RAM
#DEFINE	BANK0	BCF	STATUS,RP0	; SELECIONA BANK0 DA MEMORIA RAM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                           CONSTANTES INTERNAS                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINI��O DE CONSTANTES FACILITA A PROGRAMA��O E A MANUTEN��O.

;  ESTE PROGRAMA N�O UTILIZA NENHUMA CONSTANTE.

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                    DECLARA��O DOS FLAGs DE SOFTWARE                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINI��O DE FLAGs AJUDA NA PROGRAMA��O E ECONOMIZA MEM�RIA RAM.

;  ESTE PROGRAMA N�O UTILIZA NENHUM FLAG DE USU�RIO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                ENTRADAS                                 *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS ENTRADAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
;  FUTURAS ALTERA��ES DO HARDWARE.

;  ESTE PROGRAMA UTILIZA UMA ENTRADA P/ O CONVERSOR A/D.
;  ESTA ENTRADA N�O PRECISA SER DECLARADA, POIS O SOFTWARE NUNCA FAZ
;  REFER�NCIA A ELA DE FORMA DIRETA, POIS O CANAL A/D A SER CONVERTIDO �
;  SELECIONADO NO REGISTRADOS ADCON0 DE FORMA BIN�RIA E N�O ATRAV�S DE
;  DEFINES. POR�M PARA FACILITAR O ENTENDIMENTO DO HARDWARE VAMOS DECLARAR
;  ESTA ENTRADA NORMALMENTE.

#DEFINE	CAD_P2	PORTA,1			; ENTRADA A/D P/ O POTENCI�METRO P2

;  AL�M DA ENTRADA DO CONVERSOR A/D, TEMOS A ENTRADA DA USART (RECEP��O).
;  NOVAMENTE ESTA ENTRADA N�O NECESSITA SER DECLARADA, POR�M, PARA 
;  FACILITAR O ENTENDIMENTO DO HARDWARE VAMOS DECLARAR ESTA ENTRADA
;  NORMALMENTE.

#DEFINE	RXUSART	PORTC,7			; ENTRADA DE RX DA USART

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                 SA�DAS                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
;  FUTURAS ALTERA��ES DO HARDWARE.

#DEFINE	DISPLAY		PORTD		; BARRAMENTO DE DADOS DO DISPLAY

#DEFINE	RS		PORTE,0		; INDICA P/ O DISPLAY UM DADO OU COMANDO
					; 1 -> DADO
					; 0 -> COMANDO

#DEFINE	ENABLE		PORTE,1		; SINAL DE ENABLE P/ DISPLAY
					; ATIVO NA BORDA DE DESCIDA

;  TEMOS TAMB�M A SA�DA DE TX DA USART.
;  NOVAMENTE ESTA SA�DA N�O NECESSITA SER DECLARADA, POR�M, PARA FACILITAR O
;  ENTENDIMENTO DO HARDWARE VAMOS DECLARAR ESTA SA�DA NORMALMENTE.

#DEFINE	TXUSART	PORTC,6			; SA�DA DE TX DA USART

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   VETOR DE RESET DO MICROCONTROLADOR                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  POSI��O INICIAL PARA EXECU��O DO PROGRAMA

	ORG	0X0000			; ENDERE�O DO VETOR DE RESET
	GOTO	CONFIG_			; PULA PARA CONFIG DEVIDO A REGI�O
					; DESTINADA AS ROTINAS SEGUINTES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   ROTINA DE DELAY (DE 1MS AT� 256MS)                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA � UMA ROTINA DE DELAY VARI�VEL, COM DURA��O DE 1MS X O VALOR PASSADO
;  EM WORK (W).

DELAY_MS
	MOVWF	TEMPO1			; CARREGA TEMPO1 (UNIDADES DE MS)
	MOVLW	.250
	MOVWF	TEMPO0			; CARREGA TEMPO0 (P/ CONTAR 1MS)

	CLRWDT				; LIMPA WDT (PERDE TEMPO)
	DECFSZ	TEMPO0,F		; FIM DE TEMPO0 ?
	GOTO	$-2			; N�O - VOLTA 2 INSTRU��ES
					; SIM - PASSOU-SE 1MS
	DECFSZ	TEMPO1,F		; FIM DE TEMPO1 ?
	GOTO	$-6			; N�O - VOLTA 6 INSTRU��ES
					; SIM
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA DE UM CARACTER NO DISPLAY               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ENVIA UM CARACTER PARA O M�DULO DE LCD. O CARACTER A SER
;  ESCRITO DEVE SER COLOCADO EM WORK (W) ANTES DE CHAMAR A ROTINA.

ESCREVE
	MOVWF	DISPLAY			; ATUALIZA DISPLAY (PORTD)
	NOP				; PERDE 1US PARA ESTABILIZA��O
	BSF	ENABLE			; ENVIA UM PULSO DE ENABLE AO DISPLAY
	GOTO	$+1			; .
	BCF	ENABLE			; .

	MOVLW	.1
	CALL	DELAY_MS		; DELAY DE 1MS
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                               AJUSTE DECIMAL                            *
; *           W [HEX] =  CENTENA [DEC] : DEZENA [DEC] ; UNIDADE [DEC]       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA RECEBE UM ARGUMENTO PASSADO PELO WORK E RETORNA NAS VARI�VEIS
;  CENTENA, DEZENA E UNIDADE O N�MERO BCD CORRESPOND�NTE AO PAR�METRO PASSADO.

AJUSTE_DECIMAL
	MOVWF	AUX			; SALVA VALOR A CONVERTER EM AUX

	CLRF	UNIDADE
	CLRF	DEZENA
	CLRF	CENTENA			; RESETA REGISTRADORES

	MOVF	AUX,F
	BTFSC	STATUS,Z		; VALOR A CONVERTER = 0 ?
	RETURN				; SIM - RETORNA
					; N�O
	INCF	UNIDADE,F		; INCREMENTA UNIDADE

	MOVF	UNIDADE,W
	XORLW	0X0A
	BTFSS	STATUS,Z		; UNIDADE = 10d ?
	GOTO	$+3			; N�O
					; SIM
	CLRF	UNIDADE			; RESETA UNIDADE
	INCF	DEZENA,F		; INCREMENTA DEZENA

	MOVF	DEZENA,W
	XORLW	0X0A
	BTFSS	STATUS,Z		; DEZENA = 10d ?
	GOTO	$+3			; N�O
					; SIM
	CLRF	DEZENA			; RESETA DEZENA
	INCF	CENTENA,F		; INCREMENTA CENTENA

	DECFSZ	AUX,F			; FIM DA CONVERS�O ?
	GOTO	$-.14			; N�O - VOLTA P/ CONTINUAR CONVERS�O
	RETURN				; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                CONFIGURA��ES INICIAIS DE HARDWARE E SOFTWARE            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  NESTA ROTINA S�O INICIALIZADAS AS PORTAS DE I/O DO MICROCONTROLADOR E AS
;  CONFIGURA��ES DOS REGISTRADORES ESPECIAIS (SFR). A ROTINA INICIALIZA A
;  M�QUINA E AGUARDA O ESTOURO DO WDT.

CONFIG_
	CLRF	PORTA			; GARANTE TODAS AS SA�DAS EM ZERO
	CLRF	PORTB
	CLRF	PORTC
	CLRF	PORTD
	CLRF	PORTE

	BANK1				; SELECIONA BANCO 1 DA RAM

	MOVLW	B'11111111'
	MOVWF	TRISA			; CONFIGURA I/O DO PORTA

	MOVLW	B'11111111'
	MOVWF	TRISB			; CONFIGURA I/O DO PORTB

	MOVLW	B'10111111'
	MOVWF	TRISC			; CONFIGURA I/O DO PORTC

	MOVLW	B'00000000'
	MOVWF	TRISD			; CONFIGURA I/O DO PORTD

	MOVLW	B'00000100'
	MOVWF	TRISE			; CONFIGURA I/O DO PORTE

	MOVLW	B'11011011'
	MOVWF	OPTION_REG		; CONFIGURA OPTIONS
					; PULL-UPs DESABILITADOS
					; INTER. NA BORDA DE SUBIDA DO RB0
					; TIMER0 INCREM. PELO CICLO DE M�QUINA
					; WDT   - 1:8
					; TIMER - 1:1
					
	MOVLW	B'00000000'
	MOVWF	INTCON			; CONFIGURA INTERRUP��ES
					; DESABILITA TODAS AS INTERRUP��ES

	MOVLW	B'00000100'
	MOVWF	ADCON1			; CONFIGURA CONVERSOR A/D
					; RA0, RA1 E RA3 COMO ANAL�GICO
					; RA2, RA4 E RA5 COMO I/O DIGITAL
					; PORTE COMO I/O DIGITAL
					; JUSTIFICADO � ESQUERDA
					; 8 BITS EM ADRESH E 2 BITS EM ADRESL
					; Vref+ = VDD (+5V)
					; Vref- = GND ( 0V)

	MOVLW	B'00100100'
	MOVWF	TXSTA			; CONFIGURA USART
					; HABILITA TX
					; MODO ASSINCRONO
					; TRANSMISS�O DE 8 BITS
					; HIGH SPEED BAUD RATE
	MOVLW	.25
	MOVWF	SPBRG			; ACERTA BAUD RATE -> 9600bps

	BANK0				; SELECIONA BANCO 0 DA RAM

	MOVLW	B'10010000'
	MOVWF	RCSTA			; CONFIGURA USART
					; HABILITA RX
					; RECEP��O DE 8 BITS
					; RECEP��O CONT�NUA
					; DESABILITA ADDRESS DETECT

	MOVLW	B'01001001'
	MOVWF	ADCON0			; CONFIGURA CONVERSOR A/D
					; VELOCIDADE -> Fosc/8
					; CANAL 1
					; M�DULO LIGADO

;  AS INSTRU��ES A SEGUIR FAZEM COM QUE O PROGRAMA TRAVE QUANDO HOUVER UM
;  RESET OU POWER-UP, MAS PASSE DIRETO SE O RESET FOR POR WDT. DESTA FORMA,
;  SEMPRE QUE O PIC � LIGADO, O PROGRAMA TRAVA, AGUARDA UM ESTOURO DE WDT
;  E COME�A NOVAMENTE. ISTO EVITA PROBLEMAS NO START-UP DO PIC.

	BTFSC	STATUS,NOT_TO		; RESET POR ESTOURO DE WATCHDOG TIMER ?
	GOTO	$			; N�O - AGUARDA ESTOURO DO WDT
					; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         INICIALIZA��O DA RAM                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA IR� LIMPAR TODA A RAM DO BANCO 0, INDO DE 0X20 A 0X7F

	MOVLW	0X20
	MOVWF	FSR			; APONTA O ENDERE�AMENTO INDIRETO PARA
					; A PRIMEIRA POSI��O DA RAM
LIMPA_RAM
	CLRF	INDF			; LIMPA A POSI��O
	INCF	FSR,F			; INCREMENTA O PONTEIRO P/ A PR�X. POS.
	MOVF	FSR,W
	XORLW	0X80			; COMPARA O PONTEIRO COM A �LT. POS. +1
	BTFSS	STATUS,Z		; J� LIMPOU TODAS AS POSI��ES?
	GOTO	LIMPA_RAM		; N�O - LIMPA A PR�XIMA POSI��O
					; SIM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                     CONFIGURA��ES INICIAIS DO DISPLAY                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA INICIALIZA O DISPLAY P/ COMUNICA��O DE 8 VIAS, DISPLAY PARA 2
; LINHAS, CURSOR APAGADO E DESLOCAMENTO DO CURSOR � DIREITA. 

INICIALIZACAO_DISPLAY
	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZA��O

	MOVLW	.3
	CALL	DELAY_MS		; DELAY DE 3MS (EXIGIDO PELO DISPLAY)

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZA��O

	MOVLW	0X30			; ESCREVE COMANDO 0X30 PARA
	CALL	ESCREVE			; INICIALIZA��O

	MOVLW	B'00111000'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; INTERFACE DE 8 VIAS DE DADOS

	MOVLW	B'00000001'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; LIMPAR TODO O DISPLAY

	MOVLW	.1
	CALL	DELAY_MS		; DELAY DE 1MS

	MOVLW	B'00001100'		; ESCREVE COMANDO PARA
	CALL	ESCREVE			; LIGAR O DISPLAY SEM CURSOR

	MOVLW	B'00000110'		; ESCREVE COMANDO PARA INCREM.
	CALL	ESCREVE			; AUTOM�TICO � DIREITA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                 ROTINA DE ESCRITA DA TELA PRINCIPAL                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A TELA PRINCIPAL DO PROGRAMA, COM AS FRASES:
;  LINHA 1 - "USART:9600,8,n,1"
;  LINHA 2 - "TX:   d   h RX: "

	MOVLW	0X80			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 0 / COLUNA 0
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "USART:9600,8,n,1"
	MOVLW	'U'
	CALL	ESCREVE
	MOVLW	'S'
	CALL	ESCREVE
	MOVLW	'A'
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	'T'
	CALL	ESCREVE
	MOVLW	':'
	CALL	ESCREVE
	MOVLW	'9'
	CALL	ESCREVE
	MOVLW	'6'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	'0'
	CALL	ESCREVE
	MOVLW	','
	CALL	ESCREVE
	MOVLW	'8'
	CALL	ESCREVE
	MOVLW	','
	CALL	ESCREVE
	MOVLW	'n'
	CALL	ESCREVE
	MOVLW	','
	CALL	ESCREVE
	MOVLW	'1'
	CALL	ESCREVE

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XC0			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 0
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "TX:   d   h RX: "
	MOVLW	'T'
	CALL	ESCREVE
	MOVLW	'X'
	CALL	ESCREVE
	MOVLW	':'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'd'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'h'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	'X'
	CALL	ESCREVE
	MOVLW	':'
	CALL	ESCREVE

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                               LOOP PRINCIPAL                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A ROTINA PRINCIPAL FICA AGUARDANDO O FINAL DA CONVERS�O A/D E VERIFICANDO
;  SE ALGUM DADO FOI RECEBIDO PELA USART

	BSF	ADCON0,GO		; INICIA CONVERS�O A/D
					; EXECUTADA APENAS UMA VEZ

LOOP
	CLRWDT				; LIMPA WATCHDOG TIMER

	BTFSS	ADCON0,GO		; FIM DA CONVERS�O ?
	GOTO	FIM_CONVERSAO_AD	; SIM
					; N�O
	BTFSC	PIR1,RCIF		; RECEBEU ALGUM DADO NA SERIAL ?
	GOTO	DADO_RECEBIDO		; SIM
					; N�O
	GOTO	LOOP			; VOLTA P/ LOOP

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      MOSTRA A/D NO DISPLAY E TRANSMITE                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA MOSTRA O VALOR DA CONVERS�O A/D NO DISPLAY LCD TANTO EM DECIMAL
;  COMO EM HEXADECIMAL. O VALOR DA CONVERS�O TAMB�M � TRANSMITIDO PELA USART.
;  AO FINAL, A ROTINA REQUISITA UMA NOVA CONVERS�O A/D.

FIM_CONVERSAO_AD

; ************* MOSTRA VALOR DA CONVERS�O A/D EM DECIMAL ********************

	MOVF	ADRESH,W		; CARREGA WORK COM VALOR DO A/D
	CALL	AJUSTE_DECIMAL		; CHAMA ROTINA DE AJUSTE DECIMAL

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XC3			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 3
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	MOVF	CENTENA,W
	ADDLW	0X30			; CONVERTE BCD DA CENTENA EM ASCII
	CALL	ESCREVE			; ENVIA AO LCD

	MOVF	DEZENA,W
	ADDLW	0X30			; CONVERTE BCD DA DEZENA EM ASCII
	CALL	ESCREVE			; ENVIA AO LCD

	MOVF	UNIDADE,W
	ADDLW	0X30			; CONVERTE BCD DA UNIDADE EM ASCII
	CALL	ESCREVE			; ENVIA AO LCD

; ************* MOSTRA VALOR DA CONVERS�O A/D EM HEXADECIMAL ****************

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XC8			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 8
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	SWAPF	ADRESH,W		; INVERTE NIBLE DO ADRESH
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO � POSITIVO? (� UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; N�O - WORK FICA COM 30h (N�MERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERS�O ASCII)
	CALL	ESCREVE 		; ENVIA CARACTER AO DISPLAY LCD

	MOVF	ADRESH,W		; CARREGA WORK COM ADRESH
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO � POSITIVO? (� UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; N�O - WORK FICA COM 30h (N�MERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERS�O ASCII)
	CALL	ESCREVE 		; ENVIA CARACTER AO DISPLAY LCD


; ************* TRANSMITE VALOR DA CONVERS�O A/D PELA USART *****************

	MOVF	ADRESH,W		; CARREGA WORK COM O VALOR DO A/D
	BANK1				; ALTERA P/ BANCO 1 DA RAM
	BTFSS	TXSTA,TRMT		; O BUFFER DE TX EST� VAZIO ?
	GOTO	$-1			; N�O - AGUARDA ESVAZIAR
	BANK0				; SIM - VOLTA P/ BANCO 0 DA RAM
	MOVWF	TXREG			; SALVA WORK EM TXREG (INICIA TX)

; *********************** INICIA UMA NOVA CONVERS�O *************************

	BSF	ADCON0,GO		; PEDE UMA NOVA CONVERS�O A/D

	GOTO	LOOP			; VOLTA PARA LOOP

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      ROTINA DE RECEP��O DE DADOS NA USART               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA � EXECUTADA TODA VEZ QUE UM NOVO DADO � RECEBIDO PELA USART.
;  O DADO RECEBIDO � MOSTRADO NO LCD (EM ASCII).

DADO_RECEBIDO
	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XCF			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 15
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	MOVF	RCREG,W			; CARREGA DADO RECEBIDO NO WORK
	CALL	ESCREVE			; ENVIA AO LCD	
					; AO LER O REGISTRADOR RCREG O BIT
					; RCIF DA INTERRUP��O � LIMPO
					; AUTOMATICAMENTE.

	GOTO	LOOP			; VOLTA P/ LOOP PRINCIPAL
		
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              FIM DO PROGRAMA                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END				; FIM DO PROGRAMA

