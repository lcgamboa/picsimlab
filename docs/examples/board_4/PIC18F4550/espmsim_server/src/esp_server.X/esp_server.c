#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include"config.h"
#include"lcd.h"
#include"itoa.h"

char linha[1024];
char dados[4][100];
int acesso=0;

int processa_cmd(char* buff,const char* resp)
{
   int i;
   
   lcd_cmd(L_CLR);
   lcd_cmd(L_L1);
   i=0;
   do{
     if(!buff[i])
     {
        lcd_str("NO RESPONSE");  
        while(1);
        //return 0; 
     }
     lcd_dat(buff[i++]); //imprime comando   
   }while((buff[i]!='\r')&&(buff[i+1]!='\n'));
   lcd_cmd(L_L2);
   while(buff[i++]);//vai para o fim
   buff[i--]=0;
   //busca inicio ultima linha
   do
   {
       i--;
   }while((buff[i-2]!='\r')&&(buff[i-1]!='\n'));
   
   lcd_str(buff+i);
   
   if(!strcmp(buff+i,resp))
   {
     return 1;
   }
   else
   {
     return 0;
   }  
}

void strrem(char* buff,int pos,int size)
{
  int i;
  for(i=pos;buff[i+size-1] !=0 ;i++ )
  {
      buff[i]=buff[i+size];
  }
}

void filtra_dados(char* buff)
{
    char * ptr;
    int i, size;
    //apaga ID,CONNECT
    do{
        ptr=strstr(buff,",CONNECT\r\n");
        if(ptr!= NULL)strrem(buff,ptr-buff-1,13); 
    }while(ptr!= NULL);
    
    //apaga ID,CLOSED    
    do{
        ptr=strstr(buff,",CLOSED\r\n");
        if(ptr!= NULL)strrem(buff,ptr-buff-1,12); 
    }while(ptr!= NULL);
    
    //salva +IPD  e apaga
    do{
        ptr=strstr(buff,"+IPD,");
        if(ptr!= NULL)
        {
            //localiza final tamanho
            for(i=0;ptr[7+i] != ':';i++);
            ptr[7+i]=0;
            size=atoi(ptr+7);
            ptr[7+i]=':';
            strncpy(dados[ptr[5]-'0'],ptr+8+i,100);
            
            strrem(buff,ptr-buff,10+i+size);
        }
    }while(ptr!= NULL);
    
    
}


//"<!DOCTYPE HTML>"

const char resp_i[]="HTTP/1.1 200 OK \r\n"
                  "Content-Type: text/html\r\n"
                  "Connection: close\r\n"
                  "Refresh: 20\r\n"
                  "\r\n"
                  "<html><body>\n"
                  "<h1>PIC18F4550 ESP8266 Server</h1>\n"
                  "<br>Access number =";

const char resp_e[]="</body></html>\n";
    

void setup(void)
{
  

    TRISCbits.TRISC7=1;
    
    serial_init();
    lcd_init();
        lcd_cmd(L_CLR);
    lcd_cmd(L_NCR);
    lcd_cmd(L_L1);
    lcd_str("Connect Espmsim");
    lcd_cmd(L_L2);
    lcd_str(" and press RB0");
    
    while(digitalRead(_RB0) == 1)
    {
        delay(1);
    }
 
    
    serial_tx_str("AT\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    serial_tx_str("AT+RST\r\n"); 
    //lê messagem reset
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"ready\r\n");
    
    delay(1000);
    serial_tx_str("AT+CWMODE=1\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"OK\r\n");
    
    
    delay(1000);
    serial_tx_str("AT+CWDHCP=1,1\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"OK\r\n");
    

    delay(1000);
    serial_tx_str("AT+CWJAP=\"rede1\",\"123456\"\r\n");  
    serial_rx_str(linha, 1024, 8000);
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    serial_tx_str("AT+CIFSR\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    lcd_cmd(L_L2);
    linha[40]=0;        
    lcd_str(linha+25);//imprime IP
    
    delay(1000);
    serial_tx_str("AT+CIPMUX=1\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    serial_tx_str("AT+CIPSERVER=1,2000\r\n"); 
    serial_rx_str(linha, 1024, 2000);
    if(processa_cmd(linha,"OK\r\n"))    
    {
        lcd_cmd(L_CLR);
        lcd_str(" Server Ok");
        lcd_cmd(L_L2);
        lcd_str(" on port 2000");
    }
    else
    {
       lcd_cmd(L_CLR);
       lcd_str("Server ERRO");
       while(1);
    }
    delay(1000);
 
}

unsigned char id;
char stemp[20];
int i;
void loop(void)
{
    serial_rx_str(linha, 1024, 200);
    if(linha[0] != 0)//recebeu dado
    {   
     filtra_dados(linha);
    }
    
     //verifica se tem dados para tratar
     for(i=0;i<4;i++)
     {
      if(dados[i][0] !=0 )
      {    
        id= i+'0';
        //zera buffer de dados
        dados[i][0]=0;
        
        lcd_cmd(L_L2);
        lcd_str("Rec:");
        lcd_dat(id);
        
        acesso++;
        serial_tx_str("AT+CIPSEND=");
        serial_tx(id);
        sprintf(stemp,",%i\r\n",strlen(resp_i)+strlen(resp_e)+5);
        serial_tx_str(stemp);
        serial_rx_str(linha, 1024, 200);
        filtra_dados(linha);
    
        serial_tx_str(resp_i);
        serial_tx_str(itoa_(acesso,linha));
        serial_tx_str(resp_e);
        serial_rx_str(linha, 1024, 200);
        filtra_dados(linha);
        processa_cmd(linha,"SEND OK\r\n"); 
            
        delay(200);
        serial_tx_str("AT+CIPCLOSE=");
        serial_tx(id);
        serial_tx_str("\r\n"); 
        serial_rx_str(linha, 1024, 200);
        filtra_dados(linha);
        processa_cmd(linha,"OK\r\n");
        linha[0]=0;
      }
    }
}
