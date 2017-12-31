; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   CONECTANDO O PIC - RECURSOS AVAN�ADOS                 *
; *                               EXEMPLO 1                                 *
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
;  ESTE SOFTWARE EST� PREPARADO PARA LER QUATRO BOT�ES E TOCAR O BUZZER COM
;  DURA��O VARI�VEL CONFORME A TECLA PRESSIONADA, AL�M DE ACENDER O LED
;  INDICANDO A �LTIMA TECLA PRESSIONADA.
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         CONFIGURA��ES PARA GRAVA��O                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

 __CONFIG _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_ON & _WDT_ON & _XT_OSC

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                            DEFINI��O DAS VARI�VEIS                      *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE BLOCO DE VARI�VEIS EST� LOCALIZADO NO FINAL DO BANCO 0, A PARTIR
;  DO ENDERE�O 0X70, POIS ESTA LOCALIZA��O � ACESSADA DE QUALQUER BANCO,
;  FACILITANDO A OPERA��O COM AS VARI�VEIS AQUI LOCALIZADAS.

	CBLOCK	0X70			; POSI��O COMUM A TODOS OS BANCOS

		W_TEMP			; REGISTRADOR TEMPOR�RIO PARA W
		STATUS_TEMP		; REGISTRADOR TEMPOR�RIO PARA STATUS
		BOTOES_TEMP		; REGISTRADOR TEMPOR�RIO PARA BOT�ES

		STATUS_BOTOES		; ARMAZENA O ESTADO DOS BOT�ES
		STATUS_LEDS		; ARMAZENA O ESTADO DOS LEDS

		FILTRO_BT0		; FILTRO PARA BOTAO 0
		FILTRO_BT1		; FILTRO PARA BOTAO 1
		FILTRO_BT2		; FILTRO PARA BOTAO 2
		FILTRO_BT3		; FILTRO PARA BOTAO 3

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

#DEFINE	BANK1	BSF	STATUS,RP0	 ; SELECIONA BANK1 DA MEMORIA RAM
#DEFINE	BANK0	BCF	STATUS,RP0	 ; SELECIONA BANK0 DA MEMORIA RAM
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                           CONSTANTES INTERNAS                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINI��O DE CONSTANTES FACILITA A PROGRAMA��O E A MANUTEN��O.

FILTRO_BOTAO	EQU	.20		; FILTRO P/ EVITAR RUIDOS DOS BOT�ES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                    DECLARA��O DOS FLAGs DE SOFTWARE                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINI��O DE FLAGs AJUDA NA PROGRAMA��O E ECONOMIZA MEM�RIA RAM.

;  ESTE PROGRAMA N�O UTILIZA NENHUM FLAG DE USU�RIO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                ENTRADAS                                 *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS ENTRADAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
;  FUTURAS ALTERA��ES DO HARDWARE.

#DEFINE	BOTOES		PORTB		; ENTRADA DOS BOT�ES (RB0 AT� RB3)

#DEFINE	BOTAO_0		BOTOES_TEMP,0	; ESTADO DO BOT�O 0
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_1		BOTOES_TEMP,1	; ESTADO DO BOT�O 1
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_2		BOTOES_TEMP,2	; ESTADO DO BOT�O 2
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

#DEFINE	BOTAO_3		BOTOES_TEMP,3	; ESTADO DO BOT�O 3
					; 1 -> LIBERADO
					; 0 -> PRESSIONADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                 SA�DAS                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
;  FUTURAS ALTERA��ES DO HARDWARE.

#DEFINE	LEDS		PORTB		; SA�DA PARA LEDS (RB0 AT� RB3)

#DEFINE	LED_BOTAO_0	STATUS_LEDS,0	; LED CORRESPONDENTE AO BOT�O 0
					; 1 -> LED LIGADO
					; 0 -> LED DESLIGADO

#DEFINE	LED_BOTAO_1	STATUS_LEDS,1	; LED CORRESPONDENTE AO BOT�O 1
					; 1 -> LED LIGADO
					; 0 -> LED DESLIGADO

#DEFINE	LED_BOTAO_2	STATUS_LEDS,2	; LED CORRESPONDENTE AO BOT�O 2
					; 1 -> LED LIGADO
					; 0 -> LED DESLIGADO

#DEFINE	LED_BOTAO_3	STATUS_LEDS,3	; LED CORRESPONDENTE AO BOT�O 3
					; 1 -> LED LIGADO
					; 0 -> LED DESLIGADO

#DEFINE	BUZZER		PORTA,5		; SA�DA PARA BUZZER

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   VETOR DE RESET DO MICROCONTROLADOR                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  POSI��O INICIAL PARA EXECU��O DO PROGRAMA

	ORG	0X0000			; ENDERE�O DO VETOR DE RESET
	NOP
	GOTO	CONFIG_			; PULA PARA CONFIG DEVIDO A REGI�O
					; DESTINADA �S INTERRUP��ES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                VETOR DE INTERRUP��O DO MICROCONTROLADOR                 *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  POSI��O DE DESVIO DO PROGRAMA QUANDO UMA INTERRUP��O ACONTECE

	ORG	0X0004			; ENDERE�O DO VETOR DE INTERRUP��O

;  � MUITO IMPORTANTE QUE OS REGISTRADORES PRIORIT�RIOS AO FUNCIONAMENTO DA
;  M�QUINA, E QUE PODEM SER ALTERADOS TANTO DENTRO QUANTO FORA DAS INTS SEJAM
;  SALVOS EM REGISTRADORES TEMPOR�RIOS PARA PODEREM SER POSTERIORMENTE
;  RECUPERADOS.

SALVA_CONTEXTO
	MOVWF	W_TEMP			; COPIA W PARA W_TEMP
	SWAPF	STATUS,W
	MOVWF	STATUS_TEMP		; COPIA STATUS PARA STATUS_TEMP

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                 TESTA QUAL INTERRUP��O FOI SOLICITADA                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  TESTA O FLAG DAS INTERRUP��ES PARA SABER PARA QUAL ROTINA DESVIAR.

TESTA_INT
	BTFSC	INTCON,T0IF		; FOI INTERRUP��O DE TMR0 ?
	GOTO	INT_TMR0		; SIM - PULA P/ INT_TMR0
					; N�O - ENT�O FOI TMR2

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                 TRATAMENTO DA INTERRUP��O DE TIMER 2                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ROTINA PARA TRATAMENTO DA INTERRUP��O DE TIMER 2.
;  INVERTE O ESTADO DO PINO DO BUZZER.

INT_TMR2
	BCF	PIR1,TMR2IF		; LIMPA FLAG DA INTERRUP��O

	BTFSS	BUZZER			; BUZZER LIGADO?
	GOTO	LIGA_BUZZER		; N�O -  ENT�O LIGA
					; SIM
	BCF	BUZZER			; DESLIGA O BUZZER
	GOTO	SAI_INT			; SAI DA INTERRUP��O

LIGA_BUZZER
	BSF	BUZZER			; LIGA O BUZZER
	GOTO	SAI_INT			; SAI DA INTERRUP��O

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                  TRATAMENTO DA INTERRUP��O DE TIMER 0                   *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  TRATAMENTO DA INTERRUP��O DE TIMER 0. RESPONS�VEL POR CONVERTER OS PINOS
;  DOS BOT�ES EM ENTRADA, SALVAR A SITUA��O DOS MESMOS NUMA VARI�VEL
;  TEMPOR�RIA, CONVERTER NOVAMENTE OS PINOS PARA SA�DA E ATUALIZAR OS LEDS.

INT_TMR0
	BCF	INTCON,T0IF		; LIMPA FLAG DA INTERRUP��O

; ***************** FOTOGRAFA O ESTADO DOS BOT�ES ***************************

	BANK1				; SELECIONA BANCO 1 DA RAM
	MOVLW	B'00001111'		; PREPARA MASCARA
	IORWF	TRISB,F			; EXECUTA MASCARA (RB0...RB3 ENTRADA)
	BANK0				; SELECIONA BANCO 0 DA RAM

	GOTO	$+1
	GOTO	$+1			; DELAY(ESTABILIZA��O ENTR.=4 CICLOS)

	MOVF	BOTOES,W		; CARREGA NO WORK O ESTADO DOS BOT�ES
	XORLW	0XFF			; INVERTE TODOS OS BITS DEVIDO � 
					; L�GICA INVERTIDA DOS BOT�ES
	ANDLW	B'00001111'		; LIMPA A PARTE ALTA, QUE N�O � USADA
	MOVWF	BOTOES_TEMP		; SALVA EM BOTOES_TEMP

	BANK1				; SELECIONA BANCO 1 DA RAM
	MOVLW	B'11110000'		; PREPARA MASCARA
	ANDWF	TRISB,F			; EXECUTA MASCARA (RB0...RB3 SA�DA)
	BANK0				; SELECIONA BANCO 0 DA RAM


; ********* ATUALIZA STATUS_BOTOES CONFORME BOT�ES PRESSIONADOS *************

TESTA_BT0
	BTFSS	BOTAO_0			; BOT�O 0 PRESSIONADO?
	GOTO	BT0_LIB			; N�O - TRATA BOT�O COMO LIBERADO
					; SIM
	DECFSZ	FILTRO_BT0,F		; DECREMENTA FILTRO DO BOT�O. ACABOU?
	GOTO	TESTA_BT1		; N�O - TESTA PR�XIMO BOT�O
	BSF	STATUS_BOTOES,0		; SIM - MARCA BOT�O COMO PRESSIONADO
	GOTO	TESTA_BT1		; TESTA PR�XIMO BOT�O
	
BT0_LIB
	MOVLW	FILTRO_BOTAO
	MOVWF	FILTRO_BT0		; REINICIALIZA FILTRO
	BCF	STATUS_BOTOES,0		; MARCA BOT�O COMO LIBERADO

TESTA_BT1
	BTFSS	BOTAO_1			; BOT�O 1 PRESSIONADO?
	GOTO	BT1_LIB			; N�O - TRATA BOT�O COMO LIBERADO
					; SIM
	DECFSZ	FILTRO_BT1,F		; DECREMENTA FILTRO DO BOT�O. ACABOU?
	GOTO	TESTA_BT2		; N�O - TESTA PR�XIMO BOT�O
	BSF	STATUS_BOTOES,1		; SIM - MARCA BOT�O COMO PRESSIONADO
	GOTO	TESTA_BT2		; TESTA PR�XIMO BOT�O

BT1_LIB
	MOVLW	FILTRO_BOTAO
	MOVWF	FILTRO_BT1		; REINICIALIZA FILTRO
	BCF	STATUS_BOTOES,1		; MARCA BOT�O COMO LIBERADO

TESTA_BT2
	BTFSS	BOTAO_2			; BOT�O 2 PRESSIONADO?
	GOTO	BT2_LIB			; N�O - TRATA BOT�O COMO LIBERADO
					; SIM
	DECFSZ	FILTRO_BT2,F		; DECREMENTA FILTRO DO BOT�O. ACABOU?
	GOTO	TESTA_BT3		; N�O - TESTA PR�XIMO BOT�O
	BSF	STATUS_BOTOES,2		; SIM - MARCA BOT�O COMO PRESSIONADO
	GOTO	TESTA_BT3		; TESTA PR�XIMO BOT�O

BT2_LIB
	MOVLW	FILTRO_BOTAO
	MOVWF	FILTRO_BT2		; REINICIALIZA FILTRO
	BCF	STATUS_BOTOES,2		; MARCA BOT�O COMO LIBERADO

TESTA_BT3
	BTFSS	BOTAO_3			; BOT�O 3 PRESSIONADO?
	GOTO	BT3_LIB			; N�O - TRATA BOT�O COMO LIBERADO
					; SIM
	DECFSZ	FILTRO_BT3,F		; DECREMENTA FILTRO DO BOT�O. ACABOU?
	GOTO	CONTINUA		; N�O - CONTINUA EXECU��O DO PROGRAMA
	BSF	STATUS_BOTOES,3		; SIM - MARCA BOT�O COMO PRESSIONADO
	GOTO	CONTINUA		; E CONTINUA EXECU��O DO PROGRAMA

BT3_LIB
	MOVLW	FILTRO_BOTAO
	MOVWF	FILTRO_BT3		; REINICIALIZA FILTRO
	BCF	STATUS_BOTOES,3		; MARCA BOT�O COMO LIBERADO

CONTINUA
	MOVF	STATUS_BOTOES,W
	MOVWF	LEDS			; ATUALIZA LEDS CONFORME BOT�ES

	MOVF	STATUS_BOTOES,F
	BTFSS	STATUS,Z		; TODOS OS BOT�ES SOLTOS?
	GOTO	MUDA_FREQ		; N�O - DEVE ALT. A FREQ.
					; SIM
	BANK1				; MUDA PARA BANK1
	MOVLW	.255
	MOVWF	PR2			; PR2 = 255
	BCF	PIE1,TMR2IE		; DESLIGA INT. TIMER2
	BANK0				; MUDA PARA BANK0
	BCF	BUZZER			; GARANTE PINO DO BUZZER EM 0
	GOTO	SAI_INT			; SAI DA INTERRUP��O

MUDA_FREQ
	CALL	ACERTA_FREQ		; CHAMA TABELA DE FREQ.
	BANK1				; MUDA PARA BANK1
	MOVWF	PR2			; ACERTA VALOR DE PR2 CONFORME
					; RETORNO DA TABELA
	BSF	PIE1,TMR2IE		; LIGA INT. TIMER2
	BANK0				; MUDA PARA BANK0

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                     FIM DA ROTINA DE INTERRUP��O                        *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  RESTAURAR OS VALORES DE "W" E "STATUS" ANTES DE RETORNAR.

SAI_INT
	SWAPF	STATUS_TEMP,W
	MOVWF	STATUS			; COPIA STATUS_TEMP PARA STATUS
	SWAPF	W_TEMP,F
	SWAPF	W_TEMP,W		; COPIA W_TEMP PARA W
	RETFIE				; RETORNA DA INTERRUP��O

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                  TABELA DE ACERTO DA FREQU�NCIA DO BUZZER               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ACERTA_FREQ
	MOVF	STATUS_BOTOES,W		; COLOCA STATUS_BOTOES EM W
	ADDWF	PCL,F			; SOMA STATUS_BOTOES AO PCL
					; CRIANDO UMA SELE��O TIPO "CASE"
					; CONFORME TABELA ABAIXO:
					; - -> LIBERARO
					; X -> PRESSIONADO
					; BT3   BT2   BT1   BT0
	RETLW	.255			;  -	 -     -     - 
	RETLW	.16			;  -	 -     -     X 
	RETLW	.32			;  -	 -     X     - 
	RETLW	.48			;  -	 -     X     X 
	RETLW	.64			;  -	 X     -     - 
	RETLW	.80			;  -	 X     -     X 
	RETLW	.96			;  -	 X     X     - 
	RETLW	.112			;  -	 X     X     X 
	RETLW	.128			;  X	 -     -     - 
	RETLW	.144			;  X	 -     -     X 
	RETLW	.160			;  X	 -     X     - 
	RETLW	.176			;  X	 -     X     X 
	RETLW	.192			;  X	 X     -     - 
	RETLW	.208			;  X	 X     -     X 
	RETLW	.224			;  X	 X     X     - 
	RETLW	.240			;  X	 X     X     X 

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

	MOVLW	B'11011111'
	MOVWF	TRISA			; CONFIGURA I/O DO PORTA

	MOVLW	B'11110000'
	MOVWF	TRISB			; CONFIGURA I/O DO PORTB

	MOVLW	B'11111111'
	MOVWF	TRISC			; CONFIGURA I/O DO PORTC

	MOVLW	B'11111111'
	MOVWF	TRISD			; CONFIGURA I/O DO PORTD

	MOVLW	B'00000111'
	MOVWF	TRISE			; CONFIGURA I/O DO PORTE

	MOVLW	B'11001111'
	MOVWF	OPTION_REG		; CONFIGURA OPTIONS
					; PULL-UPs DESABILITADOS
					; INTER. NA BORDA DE SUBIDA DO RB0
					; TIMER0 INCREM. PELO CICLO DE M�QUINA
					; WDT   - 1:128
					; TIMER - 1:1
					
	MOVLW	B'01100000'		
	MOVWF	INTCON			; CONFIGURA INTERRUP��ES
					; HABILITA AS INT. DE TMR0 E PERIF.
	MOVLW	B'00000000'		
	MOVWF	PIE1			; CONFIGURA INTERRUP��ES
					; DESABILITA AS INT. DE TMR1 E TMR2

	MOVLW	B'00000111'
	MOVWF	ADCON1			; CONFIGURA CONVERSOR A/D
					; CONFIGURA PORTA E PORTE COMO I/O DIGITAL

	BANK0				; SELECIONA BANCO 0 DA RAM

	MOVLW	B'00001111'
	MOVWF	T2CON			; TIMER2: PRESCALE  - 1:16
					;         POSTSCALE - 1:2

;  AS INSTRU��ES A SEGUIR FAZEM COM QUE O PROGRAMA TRAVE QUANDO HOUVER UM
;  RESET OU POWER-UP, MAS PASSE DIRETO SE O RESET FOR POR WDT. DESTA FORMA,
;  SEMPRE QUE O PIC � LIGADO, O PROGRAMA TRAVA, AGUARDA UM ESTOURO DE WDT
;  E COME�A NOVAMENTE. ISTO EVITA PROBLEMAS NO START-UP DO PIC.

	BTFSC	STATUS,NOT_TO		; RESET POR ESTOURO DE WDT?
	GOTO	$			; N�O - AGUARDA ESTOURO DO WDT
					; SIM

;  RECONFIGURA O VALOR DO OPTION_REG PARA ACERTAR O PRESCALE.

	BANK1				; SELECIONA BANCO 1 DA RAM
	MOVLW	B'11000010'
	MOVWF	OPTION_REG		; RECONFIGURA OPTIONS
					; PULL-UPs DESABILITADOS
					; INTER. NA BORDA DE SUBIDA DO RB0
					; TIMER0 INCR. PELO CICLO DE M�QUINA
					; WDT    - 1:1
					; TIMER0 - 1:8
	BANK0				; SELECIONA BANCO 0 DA RAM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         INICIALIZA��O DA RAM                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA IR� LIMPAR TODA A RAM DO BANCO 0, INDO DE 0X20 A 0X7F.
;  EM SEGUIDA, AS VARI�VEIS DE RAM DO PROGRAMA S�O INICIALIZADAS.

	MOVLW	0X20
	MOVWF	FSR			; APONTA O ENDERE�AMENTO INDIRETO PARA
					; A PRIMEIRA POSI��O DA RAM
LIMPA_RAM
	CLRF	INDF			; LIMPA A POSI��O
	INCF	FSR,F			; INCREMENTA PONTEIRO P/ A PR�X. POS.
	MOVF	FSR,W
	XORLW	0X80			; COMPARA PONTEIRO COM A �LT. POS. +1
	BTFSS	STATUS,Z		; J� LIMPOU TODAS AS POSI��ES?
	GOTO	LIMPA_RAM		; N�O - LIMPA A PR�XIMA POSI��O
					; SIM

	MOVLW	FILTRO_BOTAO		; INICIALIZA OS FILTROS DOS BOT�ES
	MOVWF	FILTRO_BT0
	MOVWF	FILTRO_BT1
	MOVWF	FILTRO_BT2
	MOVWF	FILTRO_BT3
	
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              LOOP PRINCIPAL                             *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA PRINCIPAL SIMPLESMENTE LIMPA O WDT, POIS TODA A L�GICA DO
;  PROGRAMA � TRATADA DENTRO DAS INTERRUP��ES. 

	BSF	INTCON,GIE		; LIGA AS INTERRUP��ES

LOOP
	CLRWDT				; LIMPA WATCHDOG TIMER
        NOP
	GOTO	LOOP			; VOLTA AO LOOP

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              FIM DO PROGRAMA                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END				; FIM DO PROGRAMA

