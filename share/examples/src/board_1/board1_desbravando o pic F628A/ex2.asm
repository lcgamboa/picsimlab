;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                  CONTADOR SIMPLIFICADO - EX2                    *
;*                       DESBRAVANDO O PIC                         *
;*       DESENVOLVIDO PELA MOSAICO ENGENHARIA E CONSULTORIA        *
;*      VERSÃO: 1.0                             DATA: 30/10/01     *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                      DESCRIÇÃO DO ARQUIVO                       *
;*-----------------------------------------------------------------*
;*  SISTEMA MUITO SIMPLES PARA INCREMENTAR ATÉ UM DETERMINADO      *
;*  VALOR (MAX) DE DEPOIS DECREMENTAR ATÉ OUTRO (MIN).             *
;*                                                                 *
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
		FILTRO		;FILTRAGEM PARA O BOTÃO

	ENDC			;FIM DO BLOCO DE MEMÓRIA		

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                        FLAGS INTERNOS                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS FLAGS UTILIZADOS PELO SISTEMA

#DEFINE	SENTIDO	FLAGS,0		;FLAG DE SENTIDO
				; 0 -> SOMANDO
				; 1 -> SUBTRAINDO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                         CONSTANTES                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODAS AS CONSTANTES UTILIZADAS PELO SISTEMA

MIN		EQU	.10	;VALOR MÍNIMO PARA O CONTADOR
MAX		EQU	.30	;VALOR MÁXIMO PARA O CONTADOR
T_FILTRO	EQU	.230	;FILTRO PARA BOTÃO
	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                           ENTRADAS                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS PINOS QUE SERÃO UTILIZADOS COMO ENTRADA
; RECOMENDAMOS TAMBÉM COMENTAR O SIGNIFICADO DE SEUS ESTADOS (0 E 1)

#DEFINE	BOTAO	PORTA,2	;PORTA DO BOTÃO
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
;*                     INICIO DO PROGRAMA                          *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
INICIO
	BANK1			;ALTERA PARA O BANCO 1
	MOVLW	B'00000100'
	MOVWF	TRISA		;DEFINE RA2 COMO ENTRADA E DEMAIS
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
	MOVLW	B'00000111'
	MOVWF	CMCON		;DEFINE O MODO DE OPERAÇÃO DO COMPARADOR ANALÓGICO

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     INICIALIZAÇÃO DAS VARIÁVEIS                 *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	CLRF	PORTA		;LIMPA O PORTA
	CLRF	PORTB		;LIMPA O PORTB
	MOVLW	MIN
	MOVWF	CONTADOR	;INICIA CONTADOR = V_INICIAL

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ROTINA PRINCIPAL                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
MAIN
	MOVLW	T_FILTRO
	MOVWF	FILTRO		;INICIALIZA FILTRO = T_FILTRO

CHECA_BT
	BTFSC	BOTAO		;O BOTÃO ESTÁ PRESSIONADO?
	GOTO	MAIN		;NÃO, ENTÃO CONTINUA ESPERANDO
				;SIM
	DECFSZ	FILTRO,F	;DECREMENTA O FILTRO DO BOTÃO
				;TERMINOU?
	GOTO	CHECA_BT	;NÃO, CONTINUA ESPERANDO
				;SIM

TRATA_BT
	BTFSS	SENTIDO		;DEVE SOMAR (SENTIDO=0)?
	GOTO	SOMA		;SIM
				;NÃO
SUBTRAI
	DECF	CONTADOR,F	;DECREMENTA O CONTADOR

	MOVLW	MIN		;MOVE O VALOR MÍNIMO PARA W
	SUBWF	CONTADOR,W	;SUBTRAI O VALOR DE W (MIN) DE CONTADOR
	BTFSC	STATUS,C	;TESTA CARRY. RESULTADO NEGATIVO?
	GOTO	ATUALIZA	;NÃO, ENTÃO CONTA >= MIN
				;SIM, ENTÃO CONTA < MIN

	INCF	CONTADOR,F	;INCREMENTA CONTADOR NOVAMENTE
				;POIS PASSOU DO LIMITE
	BCF	SENTIDO		;MUDA SENTIDO PARA SOMA
	GOTO	MAIN		;VOLTA AO LOOP PRINCIPAL

SOMA
	INCF	CONTADOR,F	;INCREMENTA O CONTADOR

	MOVLW	MAX		;MOVE O VALOR MÁXIMO PARA W
	SUBWF	CONTADOR,W	;SUBTRAI O VALOR DE W (MIN) DE CONTADOR
	BTFSS	STATUS,C	;TESTA CARRY. RESULTADO NEGATIVO?
	GOTO	ATUALIZA	;SIM, ENTÃO CONTA < MAX
				;NÃO, ENTÃO CONTA >= MAX

	BSF	SENTIDO		;MUDA SENTIDO PARA SUBTRAÇÃO
	GOTO	MAIN		;VOLTA AO LOOP PRINCIPAL

ATUALIZA
	MOVF	CONTADOR,W	;COLOCA CONTADOR EM W
	MOVWF	PORTB		;ATUALIZA O PORTB PARA
				;VISUALIZARMOS O VALOR DE CONTADOR

	BTFSS	BOTAO		;O BOTÃO CONTINUA PRESSIONADO?
	GOTO	$-1		;SIM, ENTÃO ESPERA LIBERAÇÃO PARA
				;QUE O CONTADOR NÃO DISPARE
	GOTO	MAIN		;NÃO, VOLTA AO LOOP PRINCIPAL

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       FIM DO PROGRAMA                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END			;OBRIGATÓRIO

