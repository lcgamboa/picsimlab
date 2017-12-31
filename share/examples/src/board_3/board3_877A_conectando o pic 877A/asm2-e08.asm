; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                   CONECTANDO O PIC - RECURSOS AVANÇADOS                 *
; *                               EXEMPLO 8                                 *
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
;  ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
;  NA MEMÓRIA E2PROM SERIAL EXTERNA, UTILIZANDO O MASTER I2C.
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
		TEMPO_TURBO		; TEMPORIZADOR P/ TURBO DAS TECLAS

		TEMPO1
		TEMPO0			; CONTADORES P/ DELAY

		FLAG			; FLAG DE USO GERAL

		AUX			; REGISTRADOR AUXILIAR DE USO GERAL

		ENDERECO_HIGH		; REGISTRADORES DE ENDEREÇO PARA
		ENDERECO_LOW		; ACESSO À MEMÓRIA EEPROM SERIAL EXTERNA
					; MAPEADOS NO BANCO 0 DA RAM

		BUFFER			; REGISTRADOR PARA LEITURA/GRAVAÇÃO NA EEPROM SERIAL
					; EXTERNA

		VALOR_DADOS		; REGISTRADOR DE DADO PARA EEPROM SERIAL EXTERNA
					; MAPEADO NO BANCO 0 DA RAM
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

#DEFINE	BANK1	BSF	STATUS,RP0	; SELECIONA BANK1 DA MEMORIA RAM
#DEFINE	BANK0	BCF	STATUS,RP0	; SELECIONA BANK0 DA MEMORIA RAM

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                           CONSTANTES INTERNAS                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINIÇÃO DE CONSTANTES FACILITA A PROGRAMAÇÃO E A MANUTENÇÃO.

FILTRO_TECLA	EQU	.200		; FILTRO P/ EVITAR RUIDOS DOS BOTÕES

TURBO_TECLA	EQU	.60		; TEMPORIZADOR P/ TURBO DAS TECLAS

END_EEPROM_H	EQU	0X00		; ENDEREÇO P/ LEITURA E GRAVAÇÃO
END_EEPROM_L	EQU	0X00		; NA MEMÓRIA EEPROM SERIAL

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                    DECLARAÇÃO DOS FLAGs DE SOFTWARE                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  A DEFINIÇÃO DE FLAGs AJUDA NA PROGRAMAÇÃO E ECONOMIZA MEMÓRIA RAM.

#DEFINE	F_ERRO		FLAG,0		; 1 --> ERRO NA LEITURA DA EEPROM SERIAL

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

#DEFINE	SCL		PORTC,3		; VIA DE CLOCK DA EEPROM
					; I/O DEVE ESTAR COMO SAÍDA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                            ENTRADAS/SAÍDAS                              *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#DEFINE	SDA		PORTC,4		; VIA DE DADOS BIDIRECIONAL DA EEPROM
					; I/O DEVE INICIAR COMO ENTRADA

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
; *               ROTINA DE ESCRITA LINHA 1 DO LCD                          *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A LINHA 1 DA TELA PRINCIPAL DO LCD, COM A FRASE:
;  LINHA 1 - "   MASTER I2C   "

ATUALIZA_TELA_LINHA_1
	BCF	RS			; SELECIONA O DISPLAY P/ COMANDOS
	MOVLW	0X83			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 0 / COLUNA 3
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

					; COMANDOS PARA ESCREVER AS
					; LETRAS DE "MASTER I2C"
	MOVLW	'M'
	CALL	ESCREVE
	MOVLW	'A'
	CALL	ESCREVE
	MOVLW	'S'
	CALL	ESCREVE
	MOVLW	'T'
	CALL	ESCREVE
	MOVLW	'E'
	CALL	ESCREVE
	MOVLW	'R'
	CALL	ESCREVE
	MOVLW	' '
	CALL	ESCREVE
	MOVLW	'I'
	CALL	ESCREVE
	MOVLW	'2'
	CALL	ESCREVE
	MOVLW	'C'
	CALL	ESCREVE

	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA LINHA 2 DO LCD                          *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A LINHA 2 DA TELA PRINCIPAL DO LCD.
;  A ROTINA LEVA EM CONTA A VARIÁVEL VALOR_DADOS PARA FORMAR A LINHA 2.

ATUALIZA_TELA_LINHA_2
	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XC6			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 6

	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	SWAPF	VALOR_DADOS,W		; INVERTE NIBLE DO VALOR_DADOS
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO É POSITIVO? (É UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; NÃO - WORK FICA COM 30h (NÚMERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERSÃO ASCII)
	CALL	ESCREVE 		; ENVIA CARACTER AO DISPLAY LCD

	MOVF	VALOR_DADOS,W		; CARREGA WORK COM VALOR_DADOS
	ANDLW	B'00001111'		; MASCARA BITS MAIS SIGNIFICATIVOS
	MOVWF	AUX			; SALVA EM AUXILIAR

	MOVLW	0X0A
	SUBWF	AUX,W			; AUX - 10d (ATUALIZA FLAG DE CARRY)
	MOVLW	0X30			; CARREGA WORK COM 30h
	BTFSC	STATUS,C		; RESULTADO É POSITIVO? (É UMA LETRA?)
	MOVLW	0X37			; SIM - CARREGA WORK COM 37h
					; NÃO - WORK FICA COM 30h (NÚMERO)
	ADDWF	AUX,W			; SOMA O WORK AO AUXILIAR
					; (CONVERSÃO ASCII)
	CALL	ESCREVE 		; ENVIA CARACTER AO DISPLAY LCD

	MOVLW	'h'
	CALL	ESCREVE			; ESCREVE "h" NO DISPLAY
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE CHECAGEM DE EVENTOS I2C LIBERADOS               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA AGUARDA ATÉ QUE TODOS OS EVENTOS DA I2C ESTEJAM LIBERADOS.

AGUARDA_I2C_LIVRE
	BANK1				; ALTERA P/ BANK1
	BTFSC	SSPSTAT,R_W		; ESTÁ OCORRENDO ALGUM EVENTO I2C?
	GOTO	$-1			; SIM, ESPERA TERMINAR
	MOVF	SSPCON2,W
	ANDLW	B'00011111'		; MASCARA SSPCON2 (ATUALIZA FLAG ZERO)
	BTFSS	STATUS,Z		; BITS DE EVENTOS LIBERADOS?
	GOTO	$-3			; NÃO - AGUARDA
	BANK0				; SIM - VOLTA P/ BANK0
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                               ACK OUT	                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA ENVIA UM ACK OUT PARA O BARRAMENTO I2C.

ACK_OUT
	BANK1				; ALTERA P/ BANK1
	BCF	SSPCON2,ACKDT		; CARREGA ACK
	BSF	SSPCON2,ACKEN		; TRANSMITE
	BANK0				; VOLTA P/ BANK0
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                               NACK OUT	                                  *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA ENVIA UM NACK OUT PARA O BARRAMENTO I2C.

NACK_OUT
	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,ACKDT		; CARREGA NACK
	BSF	SSPCON2,ACKEN		; TRANSMITE
	BANK0				; VOLTA P/ BANK0
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                  ROTINA PARA TESTAR SE O ACK FOI RECEBIDO               *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA TESTA O BIT DE ACK RECEBIDO NO REGISTRADOR SSPCON2. PARA
; FACILITAR O RESTANTE DO SOFTWARE, A ROTINA COPIA ESTE FLAG NO FLAG F_ERRO
; PRESENTE NO BANCO 0 DA RAM, POIS O REGISTRADOR SSPCON2 ENCONTRA-SE NO BANK1.

TESTA_ACK
	BANK1				; ALTERA P/ BANK1
	BTFSC	SSPCON2,ACKSTAT		; RECEBEU ACK ?
	GOTO	RECEBEU_NACK		; NÃO - SINALIZA ERRO
					; SIM
	BANK0				; VOLTA P/ BANK0
	BCF	F_ERRO			; LIMPA FLAG DE ERRO
	RETURN				; RETORNA

RECEBEU_NACK
	BANK0				; VOLTA P/ BANK0
	BSF	F_ERRO			; SETA FLAG P/ INDICAR ERRO
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                  LEITURA DA EEPROM SERIAL EXTERNA                       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA LÊ A MEMÓRIA SERIAL EXTERNA. O ENDEREÇO DEVE SER PASSADO PELAS
; VARIÁVEIS ENDERECO_HIGH E ENDERECO_LOW. O VALOR LIDO É RETORNADO EM BUFFER.
; CASO ALGUM ERRO DE LEITURA OCORRA, A ROTINA DESVIA P/ I2C_ERRO.

I2C_EEPROM_READ
	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,SEN		; INICIA START BIT
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO

	BCF	STATUS,C		; ZERA O CARRY
	RLF	ENDERECO_HIGH,W		; ROTACIONA ENDERECO_HIGH
	IORLW	B'10100000'		; JUNTA AO BYTE DE CONTROLE
	MOVWF	SSPBUF			; TRANSMITE CONTROLE + END_HIGH
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO
	MOVF	ENDERECO_LOW,W
	MOVWF	SSPBUF			; TRANSMITE ENDEREÇO BAIXO
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO

	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,RSEN		; REINICIA START BIT
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	
	RLF	ENDERECO_HIGH,W		; ROTACIONA ENDERECO_HIGH
	IORLW	B'10100001'		; JUNTA AO BYTE DE CONTROLE
	MOVWF	SSPBUF			; TRANSMITE CONTROLE + END_HIGH
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO

	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,RCEN		; INICIA LEITURA DO BYTE
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	MOVF	SSPBUF,W
	MOVWF	BUFFER			; SALVA DADO EM BUFFER	
	CALL	NACK_OUT		; ENVIA NACK --> FIM
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO

	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,PEN		; INICIA STOP BIT
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                    ESCRITA NA EEPROM SERIAL EXTERNA                     *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA GRAVA UM DADO NA MEMÓRIA SERIAL EXTERNA. O ENDEREÇO DEVE SER
; PASSADO PELAS VARIÁVEIS ENDERECO_HIGH E ENDERECO_LOW. O VALOR A SER GRAVADO
; DEVE SER PASSADO EM BUFFER.
; CASO ALGUM ERRO DE GRAVAÇÃO OCORRA, A ROTINA DESVIA P/ I2C_ERRO.

I2C_EEPROM_WRITE
	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,SEN		; INICIA START BIT
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO

	BCF	STATUS,C		; ZERA O CARRY
	RLF	ENDERECO_HIGH,W		; ROTACIONA ENDERECO_HIGH
	IORLW	B'10100000'		; JUNTA AO BYTE DE CONTROLE
	MOVWF	SSPBUF			; TRANSMITE CONTROLE + END_HIGH
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO
	MOVF	ENDERECO_LOW,W
	MOVWF	SSPBUF			; TRANSMITE ENDEREÇO BAIXO
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO
	MOVF	BUFFER,W
	MOVWF	SSPBUF			; GRAVA DADO
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	CALL	TESTA_ACK		; CHAMA ROTINA P/ TESTAR ACK
	BTFSC	F_ERRO			; OCORREU ERRO DE ACK ?
	GOTO	I2C_ERRO		; SIM - PULA P/ I2C_ERRO
					; NÃO	
	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,PEN		; INICIA STOP BIT
	BANK0				; VOLTA P/ BANK0
	CALL	AGUARDA_I2C_LIVRE	; AGUARDA FIM DO EVENTO
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                      ROTINA P/ SINALIZAR ERRO NA I2C                    *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ESTA ROTINA SOMENTE É EXECUTA CASO ALGUM ERRO DE LEITURA/GRAVAÇÃO OCORRA
; COM A MEMÓRIA SERIAL.
; A ROTINA ENVIA UM STOP BIT PARA FINALIZAR A COMUNICAÇÃO COM A MEMÓRIA
; SERIAL, ENVIA UMA MENSAGEM DE ERRO AO DISPLAY E APÓS 1s RETORNA À TELA
; PRINCIPAL.

I2C_ERRO
	BANK1				; ALTERA P/ BANK1
	BSF	SSPCON2,PEN		; INICIA STOP BIT
	BANK0				; VOLTA P/ BANK0

	BCF	RS			; SELECIONA O DISPLAY P/ COMANDO
	MOVLW	0XC6			; COMANDO PARA POSICIONAR O CURSOR
	CALL	ESCREVE			; LINHA 1 / COLUNA 6
	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

	MOVLW	'E'
	CALL	ESCREVE
	MOVLW	'r'
	CALL	ESCREVE
	MOVLW	'r'
	CALL	ESCREVE			; ESCREVE "Err" NO LCD

	MOVLW	.250
	CALL	DELAY_MS
	MOVLW	.250
	CALL	DELAY_MS
	MOVLW	.250
	CALL	DELAY_MS
	MOVLW	.250
	CALL	DELAY_MS		; DELAY DE 1seg.

	CALL	ATUALIZA_TELA_LINHA_2	; ATUALIZA TELA PRINCIPAL
	RETURN				; RETORNA

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                CONFIGURAÇÕES INICIAIS DE HARDWARE E SOFTWARE            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  NESTA ROTINA SÃO INICIALIZADAS AS PORTAS DE I/O DO MICROCONTROLADOR E AS
;  CONFIGURAÇÕES DOS REGISTRADORES ESPECIAIS (SFR). A ROTINA INICIALIZA AS
;  VARIÁVEIS DE RAM E AGUARDA O ESTOURO DO WDT.

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

	MOVLW	B'11110101'
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

	MOVLW	B'00001001'
	MOVWF	SSPADD			; VELOCIDADE: 100KHz @ 4MHz
		
	MOVLW	B'10000000'
	MOVWF	SSPSTAT			; DESABILITA SLEW-RATE CONTROL (100 KHz)

	BANK0				; SELECIONA BANCO 0 DA RAM

	MOVLW	B'00101000'
	MOVWF	SSPCON			; HABILITA I2C - MASTER MODE
					; CONFIGURA PINOS COMO DA I2C

	BSF	SCL			; INICIALIZA SCL EM 1

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
;  ESTA ROTINA IRÁ LIMPAR TODA A RAM DO BANCO 0, INDO DE 0X20 A 0X7F

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
; *                   CONFIGURAÇÕES INICIAIS DO DISPLAY                     *
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

	BSF	RS			; SELECIONA O DISPLAY P/ DADOS

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         INICIALIZAÇÃO DA RAM                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTE TRECHO DO PROGRAMA LÊ O DADOS DA MEMÓRIAS E2PROM EXTERNA E
;  ATUALIZA A RAM.

LE_MEMORIA_EEPROM
	MOVLW	END_EEPROM_H
	MOVWF	ENDERECO_HIGH
	MOVLW	END_EEPROM_L
	MOVWF	ENDERECO_LOW		; CARREGA ENDEREÇO P/ LEITURA

	CALL	I2C_EEPROM_READ		; CHAMA ROTINA P/ LER DADO

	MOVF	BUFFER,W
	MOVWF	VALOR_DADOS		; SALVA DADO LIDO EM VALOR_DADOS

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *               ROTINA DE ESCRITA DA TELA PRINCIPAL                       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;  ESTA ROTINA ESCREVE A TELA PRINCIPAL DO PROGRAMA, COM AS FRASES:
;  LINHA 1 - "   MASTER I2C   "
;  LINHA 2 - "      xxh       "

	CALL	ATUALIZA_TELA_LINHA_1	; ATUALIZA TELA LINHA 1 DO LCD

	CALL	ATUALIZA_TELA_LINHA_2	; ATUALIZA TELA LINHA 2 DO LCD

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
	MOVLW	.1
	MOVWF	TEMPO_TURBO		; CARREGA TEMPO DO TURBO DAS TECLAS
					; COM 1 - IGNORA O TURBO A PRIMEIRA
					; VEZ QUE A TECLA É PRESSIONADA

	GOTO	VARRE			; VOLTA PARA VARRER TECLADO

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                         TRATAMENTO DOS BOTÕES                           *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; NESTE TRECHO DO PROGRAMA ESTÃO TODOS OS TRATAMENTOS DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 0 ***************************

TRATA_BOTAO_0
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO

	DECFSZ	TEMPO_TURBO,F		; FIM DO TEMPO DE TURBO ?
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM
	MOVLW	TURBO_TECLA
	MOVWF	TEMPO_TURBO		; RECARREGA TEMPORIZADOR DO TURBO
					; DAS TECLAS

	INCF	VALOR_DADOS,F		; INCREMENTA VALOR_DADOS

	CALL	ATUALIZA_TELA_LINHA_2	; CHAMA ROTINA P/ ATUALIZAR LCD

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; ************************* TRATAMENTO DO BOTÃO 1 ***************************

TRATA_BOTAO_1
	DECFSZ	FILTRO_BOTOES,F		; FIM DO FILTRO ? (RUIDO?)
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM - BOTÃO PRESSIONADO

	DECFSZ	TEMPO_TURBO,F		; FIM DO TEMPO DE TURBO ?
	GOTO	VARRE			; NÃO - VOLTA P/ VARRE
					; SIM
	MOVLW	TURBO_TECLA
	MOVWF	TEMPO_TURBO		; RECARREGA TEMPORIZADOR DO TURBO
					; DAS TECLAS

	DECF	VALOR_DADOS,F		; DECREMENTA VALOR_DADOS

	CALL	ATUALIZA_TELA_LINHA_2	; CHAMA ROTINA P/ ATUALIZAR LCD

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

; ********TRECHO DO PROGRAMA PARA GRAVAR DADO DA RAM NA MEMÓRIA *************

GRAVA_MEMORIA_EEPROM
	MOVLW	END_EEPROM_H
	MOVWF	ENDERECO_HIGH
	MOVLW	END_EEPROM_L
	MOVWF	ENDERECO_LOW		; CARREGA ENDERECO ONDE O DADO SERÁ SALVO
					; END. -> 0x0000
					; PRIMEIRA POSIÇÃO DA EEPROM

	MOVF	VALOR_DADOS,W
	MOVWF	BUFFER			; CARREGA DADO A SER SALVO EM BUFFER

	CALL	I2C_EEPROM_WRITE	; CHAMA ROTINA DE GRAVAÇÃO

	MOVLW	.10
	CALL	DELAY_MS		; GARANTE TEMPO DE ESCRITA (10ms)

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

; ******** TRECHO DO PROGRAMA PARA LER DADO DA MEMÓRIA E ATUALIZAR RAM *************

LER_MEMORIA_EEPROM
	MOVLW	END_EEPROM_H
	MOVWF	ENDERECO_HIGH
	MOVLW	END_EEPROM_L
	MOVWF	ENDERECO_LOW		; CARREGA ENDEREÇO DE LEITURA
					; END. -> 0x0000
					; PRIMEIRA POSIÇÃO DA EEPROM

	CALL	I2C_EEPROM_READ		; CHAMA ROTINA DE LEITURA

	BTFSC	F_ERRO
	GOTO	$+3
	MOVF	BUFFER,W
	MOVWF	VALOR_DADOS		; ATUALIZA RAM COM O VALOR LIDO

	CALL	ATUALIZA_TELA_LINHA_2	; CHAMA ROTINA P/ ATUALIZAR LCD

	GOTO	VARRE			; VOLTA P/ VARREDURA DOS BOTÕES

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                              FIM DO PROGRAMA                            *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END				; FIM DO PROGRAMA

