;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    CONTADOR MELHORADO - EX4                     *
;*                       DESBRAVANDO O PIC                         *
;*       DESENVOLVIDO PELA MOSAICO ENGENHARIA E CONSULTORIA        *
;*      VERSÃO: 1.0                             DATA: 11/06/99     *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                      DESCRIÇÃO DO ARQUIVO                       *
;*-----------------------------------------------------------------*
;*  CONTADOR QUE UTILIZA DOIS BOTÕES PARA INCREMENTAR E DECRE-     *
;*  MENTAR O VALOR CONTROLADO PELA VARIÁVEL "CONTADOR". ESTA       *
;*  VARIÁVEL ESTÁ LIMITADA PELAS CONSTANTES "MIN" E "MAX".         *
;*  O VALOR DO CONTADOR É MOSTRADO NO DISPLAY.                     *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ARQUIVOS DE DEFINIÇÕES                      *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#INCLUDE <P16F628A.INC>		;ARQUIVO PADRÃO MICROCHIP PARA 16F628A
	__CONFIG  _BODEN_ON & _CP_OFF & _PWRTE_ON & _WDT_OFF & _LVP_OFF & _MCLRE_ON & _XT_OSC

	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    PAGINAÇÃO DE MEMÓRIA                         *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;DEFINIÇÃO DE COMANDOS DE USUÁRIO PARA ALTERAÇÃO DA PÁGINA DE MEMÓRIA

#DEFINE	BANK0	BCF STATUS,RP0	;SETA BANK 0 DE MEMÓRIA
#DEFINE	BANK1	BSF STATUS,RP0	;SETA BANK 1 DE MAMÓRIA

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                         VARIÁVEIS                               *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DOS NOMES E ENDEREÇOS DE TODAS AS VARIÁVEIS UTILIZADAS 
; PELO SISTEMA

	CBLOCK	0x20		;ENDEREÇO INICIAL DA MEMÓRIA DE
				;USUÁRIO

		W_TEMP		;REGISTRADORES TEMPORÁRIOS PARA
		STATUS_TEMP	;INTERRUPÇÕES
				;ESTAS VARIÁVEIS NEM SERÃO UTI-
				;LIZADAS
		INTENSIDADE	;ARMAZENA O VALOR DA CONTAGEM
		FLAGS		;ARMAZENA OS FLAGS DE CONTROLE
		FILTRO11	;FILTRAGEM  1 PARA O BOTÃO 1
		FILTRO12	;FILTRAGEM  2 PARA O BOTÃO 1
		FILTRO21	;FILTRAGEM  1 PARA O BOTÃO 2
		FILTRO22	;FILTRAGEM  2 PARA O BOTÃO 2
		TEMPO           ;INTERVALOS DE 1 MS

	ENDC			;FIM DO BLOCO DE MEMÓRIA		

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                        FLAGS INTERNOS                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS FLAGS UTILIZADOS PELO SISTEMA

#DEFINE	ST_BT1	FLAGS,0		;STATUS DO BOTÃO 1
#DEFINE	ST_BT2	FLAGS,1		;STATUS DO BOTÃO 2

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                         CONSTANTES                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODAS AS CONSTANTES UTILIZADAS PELO SISTEMA

MIN		EQU	.0	;VALOR MÍNIMO PARA O INTENSIDADE
MAX		EQU	.15	;VALOR MÁXIMO PARA O INTENSIDADE
T_FILTRO	EQU	.20	;FILTRO PARA BOTÃO
	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                           ENTRADAS                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS PINOS QUE SERÃO UTILIZADOS COMO ENTRADA
; RECOMENDAMOS TAMBÉM COMENTAR O SIGNIFICADO DE SEUS ESTADOS (0 E 1)

#DEFINE	BOTAO1	PORTA,1	;PORTA DO BOTÃO
			; 0 -> PRESSIONADO
			; 1 -> LIBERADO

#DEFINE	BOTAO2	PORTA,2	;PORTA DO BOTÃO
			; 0 -> PRESSIONADO
			; 1 -> LIBERADO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                           SAÍDAS                                *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS PINOS QUE SERÃO UTILIZADOS COMO SAÍDA
; RECOMENDAMOS TAMBÉM COMENTAR O SIGNIFICADO DE SEUS ESTADOS (0 E 1)

#DEFINE	LAMPADA	PORTA,0	;DEFINE LAMPADA NO PINO17
			;0 LAMP. APAGADA
			;1 LAMP. ACESA

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       VETOR DE RESET                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ORG	0x00	;ENDEREÇO INICIAL DE PROCESSAMENTO
	GOTO	INICIO
	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    INÍCIO DA INTERRUPÇÃO                        *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; AS INTERRUPÇÕES NÃO SERÃO UTILIZADAS, POR ISSO PODEMOS SUBSTITUIR
; TODO O SISTEMA EXISTENTE NO ARQUIVO MODELO PELO APRESENTADO ABAIXO
; ESTE SISTEMA NÃO É OBRIGATÓRIO, MAS PODE EVITAR PROBLEMAS FUTUROS

	ORG	0x04		;ENDEREÇO INICIAL DA INTERRUPÇÃO
	MOVWF	W_TEMP		;SALVA W EM W_TEMP
	SWAPF	STATUS,W	
	MOVWF	STATUS_TEMP	;SALVA STATUS EM STATUS_TEMP

	BTFSS	INTCON,T0IF	;É INTERRUPÇÃO DE TMR0?
	GOTO	SAI_INT		;NÃO, SAI SE AÇÃO	
				;SIM

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*              TRATAMENTO DA INTERRUPÇÃO DE TMR0                  *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA IRÁ CONTAR O TEMPO, E QUANDO PASSAR 1 SEGUNDO, A VARI-
; ÁVEL "TEMPO" SERÁ DECREMENTADA.
; 1 SEGUNDO = 64us (PRESCALER) X 125 (TMR0) X 125 (TEMP1)

	BCF	INTCON,T0IF	;LIMPA FLAG DA INT.

	MOVLW	.256-.250	;SETA TIMER P250MS
	MOVWF	TMR0		;REINICIA TMR0

	INCF	TEMPO,F		;INCREMENTA TEMPO
	
	MOVLW	.16		;COLOCA 16 EM WORK
	XORWF	TEMPO,W		;COMPARA TEMPO COM 16
	BTFSC	STATUS,Z	;TESTA BIT Z DO REG. STATUS
	CLRF	TEMPO		;ZERA TEMPO

	MOVLW	.15		;COLOCA 15 EM W
	XORWF	INTENSIDADE,W	;COMPARA INTENSIDADE COM 15
	BTFSC	STATUS,Z	;TESTA BIT Z DO REG. STATUS
	GOTO	LIGA_LAMPADA

	MOVF	INTENSIDADE,W	;MOVE INTENSIDADE PARA W
	SUBWF	TEMPO,W		;SUBTRAI TEMPO DE INTENSIDADE
	BTFSS	STATUS,C	;TESTA BIC C DO REG. STATUS
				;VERIFICA SE TEMPO E MENOR QUE INTENSIDADE
	GOTO	LIGA_LAMPADA	

	BCF	LAMPADA		;DESLIGA LAMPADA
	GOTO	SAI_INT

LIGA_LAMPADA
	
	BSF	LAMPADA		;LIGA LAMPADA

	GOTO	SAI_INT		;SAI DA INTERRUPÇÃO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       FIM DA INTERRUPÇÃO                        *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
SAI_INT
	SWAPF	STATUS_TEMP,W
	MOVWF	STATUS		;RECUPERA STATUS
	SWAPF	W_TEMP,F
	SWAPF	W_TEMP,W	;RECUPERA W
	RETFIE			;RETORNA DA INTERRUPÇÃO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                 ROTINA DE CONVERSÃO BINÁRIO -> DISPLAY          *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA IRÁ RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
; MOSTRADO NO DISPLAY PARA CADA VALOR DE INTENSIDADE. O RETORNO JÁ ESTÁ
; FORMATADO PARA AS CONDIÇÕES DE LIGAÇÃO DO DISPLAY AO PORTB.
;	   a
;     **********
;     *        *
;   f *        * b
;     *    g   *
;     **********
;     *        *
;   e *        * c
;     *    d   *
;     **********  *.

CONVERTE
	MOVF	INTENSIDADE,W	;COLOCA INTENSIDADE EM W
	ANDLW	B'00001111'	;MASCARA VALOR DE INTENSIDADE
				;CONSIDERAR SOMENTE ATÉ 15
	ADDWF	PCL,F

;		B'EDC.BAFG'	; POSIÇÃO CORRETA DOS SEGUIMENTOS
	RETLW	B'11101110'	; 00 - RETORNA SÍMBOLO CORRETO 0
	RETLW	B'00101000'	; 01 - RETORNA SÍMBOLO CORRETO 1
	RETLW	B'11001101'	; 02 - RETORNA SÍMBOLO CORRETO 2
	RETLW	B'01101101'	; 03 - RETORNA SÍMBOLO CORRETO 3
	RETLW	B'00101011'	; 04 - RETORNA SÍMBOLO CORRETO 4	
	RETLW	B'01100111'	; 05 - RETORNA SÍMBOLO CORRETO 5	
	RETLW	B'11100111'	; 06 - RETORNA SÍMBOLO CORRETO 6	
	RETLW	B'00101100'	; 07 - RETORNA SÍMBOLO CORRETO 7	
	RETLW	B'11101111'	; 08 - RETORNA SÍMBOLO CORRETO 8	
	RETLW	B'01101111'	; 09 - RETORNA SÍMBOLO CORRETO 9	
	RETLW	B'10101111'	; 10 - RETORNA SÍMBOLO CORRETO A	
	RETLW	B'11100011'	; 11 - RETORNA SÍMBOLO CORRETO b	
	RETLW	B'11000110'	; 12 - RETORNA SÍMBOLO CORRETO C	
	RETLW	B'11101001'	; 13 - RETORNA SÍMBOLO CORRETO d	
	RETLW	B'11000111'	; 14 - RETORNA SÍMBOLO CORRETO E	
	RETLW	B'10000111'	; 15 - RETORNA SÍMBOLO CORRETO F	

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     INICIO DO PROGRAMA                          *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
INICIO
	BANK0			;ALTERA PARA O BANCO 0
	MOVLW	B'00000111'
	MOVWF	CMCON		;DEFINE O MODO DE OPERAÇÃO DO COMPARADOR ANALÓGICO

	BANK1			;ALTERA PARA O BANCO 1
	MOVLW	B'00000110'
	MOVWF	TRISA		;DEFINE RA1 E 2 COMO ENTRADA E DEMAIS
				;COMO SAÍDAS
	MOVLW	B'00000000'
	MOVWF	TRISB		;DEFINE TODO O PORTB COMO SAÍDA
	MOVLW	B'10000001'
	MOVWF	OPTION_REG	;PRESCALER 1:4 NO TMR0
				;PULL-UPS DESABILITADOS
				;AS DEMAIS CONFG. SÃO IRRELEVANTES
	MOVLW	B'10100000'
	MOVWF	INTCON		;CHAVE GERAL E TMR0 ATIVADAS
	BANK0			;RETORNA PARA O BANCO 0

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     INICIALIZAÇÃO DAS VARIÁVEIS                 *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	CLRF	PORTA		;LIMPA O PORTA
	CLRF	PORTB		;LIMPA O PORTB
	CLRF	FLAGS		;LIMPA TODOS OS FLAGS
	MOVLW	MIN
	MOVWF	INTENSIDADE	;INICIA INTENSIDADE = MIN
	MOVLW	.256-.250	;SETA TIMER P250MS
	MOVWF	TMR0		;REINICIA TMR0
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY INICIALMENTE

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ROTINA PRINCIPAL                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
MAIN
	CLRF	FILTRO11
	CLRF	FILTRO21
	MOVLW	T_FILTRO
	MOVWF	FILTRO12	;INICIALIZA FILTRO1 = T_FILTRO
	MOVWF	FILTRO22	;INICIALIZA FILTRO2 = T_FILTRO

CHECA_BT1

	BTFSC	BOTAO1		;O BOTÃO 1 ESTÁ PRESSIONADO?
	GOTO	BT1_LIB		;NÃO, ENTÃO TRATA COMO LIBERADO
				;SIM

	DECFSZ	FILTRO11,F	;DECREMENTA O FILTRO DO BOTÃO
				;TERMINOU?
	GOTO	CHECA_BT1	;NÃO, CONTINUA ESPERANDO
				;SIM

	DECFSZ	FILTRO12,F	;DECREMENTA O FILTRO DO BOTÃO
				;TERMINOU?
	GOTO	CHECA_BT1	;NÃO, CONTINUA ESPERANDO
				;SIM

	BTFSS	ST_BT1		;BOTÃO JÁ ESTAVA PRESSIONADO?
	GOTO	DEC		;NÃO, EXECUTA AÇÃO DO BOTÃO
	GOTO	CHECA_BT2	;SIM, CHECA BOTÃO 2

BT1_LIB
	BCF	ST_BT1		;MARCA BOTÃO 1 COMO LIBERADO

CHECA_BT2

	BTFSC	BOTAO2		;O BOTÃO 2 ESTÁ PRESSIONADO?
	GOTO	BT2_LIB		;NÃO, ENTÃO TRATA COMO LIBERADO
				;SIM

	DECFSZ	FILTRO21,F	;DECREMENTA O FILTRO DO BOTÃO
				;TERMINOU?
	GOTO	CHECA_BT2	;NÃO, CONTINUA ESPERANDO
				;SIM

	DECFSZ	FILTRO22,F	;DECREMENTA O FILTRO DO BOTÃO
				;TERMINOU?
	GOTO	CHECA_BT2	;NÃO, CONTINUA ESPERANDO
				;SIM

	BTFSS	ST_BT2		;BOTÃO JÁ ESTAVA PRESSIONADO?
	GOTO	INC		;NÃO, EXECUTA AÇÃO DO BOTÃO
	GOTO	MAIN		;SIM, VOLTA AO LOOPING

BT2_LIB
	BCF	ST_BT2		;MARCA BOTÃO 2 COMO LIBERADO
	GOTO	MAIN		;RETORNA AO LOOPING

DEC				;AÇÃO DE DECREMENTAR
	BSF	ST_BT1		;MARCA BOTÃO 1 COMO JÁ PRESSIONADO
	MOVF	INTENSIDADE,W	;COLOCA INTENSIDADE EM W
	XORLW	MIN		;APLICA XOR ENTRE INTENSIDADE E MIN
				;PARA TESTAR IGUALDADE. SE FOREM
				;IGUAIS, O RESULTADO SERÁ ZERO
	BTFSC	STATUS,Z	;RESULTOU EM ZERO?
	GOTO	MAIN		;SIM, RETORNA SEM AFETAR CONT.
				;NÃO
	DECF	INTENSIDADE,F	;DECREMENTA O INTENSIDADE
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY

INC				;AÇÃO DE INCREMENTAR
	BSF	ST_BT2		;MARCA BOTÃO 2 COMO JÁ PRESSIONADO
	MOVF	INTENSIDADE,W	;COLOCA INTENSIDADE EM W
	XORLW	MAX		;APLICA XOR ENTRE INTENSIDADE E MAX
				;PARA TESTAR IGUALDADE. SE FOREM
				;IGUAIS, O RESULTADO SERÁ ZERO
	BTFSC	STATUS,Z	;RESULTOU EM ZERO?
	GOTO	MAIN		;SIM, RETORNA SEM AFETAR CONT.
				;NÃO
	INCF	INTENSIDADE,F	;INCREMENTA O INTENSIDADE
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY

ATUALIZA
	CALL	CONVERTE	;CONVERTE INTENSIDADE NO NÚMERO DO
				;DISPLAY
	MOVWF	PORTB		;ATUALIZA O PORTB PARA
				;VISUALIZARMOS O VALOR DE INTENSIDADE
				;NO DISPLAY
	GOTO	MAIN		;NÃO, VOLTA AO LOOP PRINCIPAL

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       FIM DO PROGRAMA                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END			;OBRIGATÓRIO

