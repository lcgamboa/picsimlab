;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    CONTADOR MELHORADO - EX4                     *
;*                       DESBRAVANDO O PIC                         *
;*       DESENVOLVIDO PELA MOSAICO ENGENHARIA E CONSULTORIA        *
;*      VERSÃO: 1.0                             DATA: 30/10/01     *
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
		CONTADOR	;ARMAZENA O VALOR DA CONTAGEM
		FLAGS		;ARMAZENA OS FLAGS DE CONTROLE
		FILTRO1		;FILTRAGEM PARA O BOTÃO 1
		FILTRO2		;FILTRAGEM PARA O BOTÃO 2

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

MIN		EQU	.0	;VALOR MÍNIMO PARA O CONTADOR
MAX		EQU	.15	;VALOR MÁXIMO PARA O CONTADOR
T_FILTRO	EQU	.255	;FILTRO PARA BOTÃO
	
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
	RETFIE			;RETORNA DA INTERRUPÇÃO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                 ROTINA DE CONVERSÃO BINÁRIO -> DISPLAY          *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA IRÁ RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
; MOSTRADO NO DISPLAY PARA CADA VALOR DE CONTADOR. O RETORNO JÁ ESTÁ
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
	MOVF	CONTADOR,W	;COLOCA CONTADOR EM W
	ANDLW	B'00001111'	;MASCARA VALOR DE CONTADOR
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
	MOVLW	B'10000000'
	MOVWF	OPTION_REG	;PRESCALER 1:2 NO TMR0
				;PULL-UPS DESABILITADOS
				;AS DEMAIS CONFG. SÃO IRRELEVANTES
	MOVLW	B'00000000'
	MOVWF	INTCON		;TODAS AS INTERRUPÇÕES DESLIGADAS
	BANK0			;RETORNA PARA O BANCO 0

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     INICIALIZAÇÃO DAS VARIÁVEIS                 *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	CLRF	PORTA		;LIMPA O PORTA
	CLRF	PORTB		;LIMPA O PORTB
	CLRF	FLAGS		;LIMPA TODOS OS FLAGS
	MOVLW	MIN
	MOVWF	CONTADOR	;INICIA CONTADOR = MIN
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY INICIALMENTE

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ROTINA PRINCIPAL                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
MAIN
	MOVLW	T_FILTRO
	MOVWF	FILTRO1		;INICIALIZA FILTRO1 = T_FILTRO
	MOVWF	FILTRO2		;INICIALIZA FILTRO2 = T_FILTRO

CHECA_BT1
	BTFSC	BOTAO1		;O BOTÃO 1 ESTÁ PRESSIONADO?
	GOTO	BT1_LIB		;NÃO, ENTÃO TRATA COMO LIBERADO
				;SIM
	DECFSZ	FILTRO1,F	;DECREMENTA O FILTRO DO BOTÃO
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
	DECFSZ	FILTRO2,F	;DECREMENTA O FILTRO DO BOTÃO
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
	MOVF	CONTADOR,W	;COLOCA CONTADOR EM W
	XORLW	MIN		;APLICA XOR ENTRE CONTADOR E MIN
				;PARA TESTAR IGUALDADE. SE FOREM
				;IGUAIS, O RESULTADO SERÁ ZERO
	BTFSC	STATUS,Z	;RESULTOU EM ZERO?
	GOTO	MAIN		;SIM, RETORNA SEM AFETAR CONT.
				;NÃO
	DECF	CONTADOR,F	;DECREMENTA O CONTADOR
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY

INC				;AÇÃO DE INCREMENTAR
	BSF	ST_BT2		;MARCA BOTÃO 2 COMO JÁ PRESSIONADO
	MOVF	CONTADOR,W	;COLOCA CONTADOR EM W
	XORLW	MAX		;APLICA XOR ENTRE CONTADOR E MAX
				;PARA TESTAR IGUALDADE. SE FOREM
				;IGUAIS, O RESULTADO SERÁ ZERO
	BTFSC	STATUS,Z	;RESULTOU EM ZERO?
	GOTO	MAIN		;SIM, RETORNA SEM AFETAR CONT.
				;NÃO
	INCF	CONTADOR,F	;INCREMENTA O CONTADOR
	GOTO	ATUALIZA	;ATUALIZA O DISPLAY

ATUALIZA
	CALL	CONVERTE	;CONVERTE CONTADOR NO NÚMERO DO
				;DISPLAY
	MOVWF	PORTB		;ATUALIZA O PORTB PARA
				;VISUALIZARMOS O VALOR DE CONTADOR
				;NO DISPLAY
	GOTO	MAIN		;NÃO, VOLTA AO LOOP PRINCIPAL

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       FIM DO PROGRAMA                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END			;OBRIGATÓRIO

