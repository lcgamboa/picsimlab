#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define LMAX 700
#define DMAX 100
char linha[LMAX+1];
char dados[4][DMAX];
int acesso=0;
int ret;

int processa_cmd(char* buff,const char* resp)
{
   int i;
   
   lcd.clear();
   lcd.setCursor(0, 0);
   i=0;
   do{
     if(!buff[i])
     {
        lcd.print("NO RESPONSE");  
        while(1);
        //return 0; 
     }
     lcd.write(buff[i++]); //imprime comando   
   }while((buff[i]!='\r')&&(buff[i+1]!='\n'));
   lcd.setCursor(0, 1);
   while(buff[i++]);//vai para o fim
   buff[i--]=0;
   //busca inicio ultima linha
   do
   {
       i--;
   }while((buff[i-2]!='\r')&&(buff[i-1]!='\n'));
   
   lcd.print(buff+i);
   
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
  
  //for(i=pos;buff[i+size-1] !=0 ;i++ )
  for(i=pos; i < size-1 ;i++ )
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
            strncpy(dados[ptr[5]-'0'],ptr+8+i,DMAX);
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
                  "<h1>Arduino ESP8266 Server</h1>\n"
                  "<br>Access number =";

const char resp_e[]="</body></html>\n";
    

void setup(void)
{
    
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Connect espmsim");
    lcd.setCursor(0, 1);
    lcd.print(" and press PD6");

    pinMode(6,INPUT);
    
    while(digitalRead(6) == 0)
    {
        delay(1);
    }
 
    
    Serial.print("AT\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
  
    delay(1000);
    Serial.print("AT+RST\r\n"); 
    //lê messagem reset
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"ready\r\n");
    
    delay(1000);
    Serial.print("AT+CWMODE=1\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
    
    
    delay(1000);
    Serial.print("AT+CWDHCP=1,1\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
    

    delay(1000);
    Serial.print("AT+CWJAP=\"rede1\",\"123456\"\r\n");  
    Serial.setTimeout(8000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    Serial.print("AT+CIFSR\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    lcd.setCursor(0, 1);
    linha[40]=0;        
    lcd.print(linha+25);//imprime IP
    
    delay(1000);
    Serial.print("AT+CIPMUX=1\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    processa_cmd(linha,"OK\r\n");
    
    delay(1000);
    Serial.print("AT+CIPSERVER=1,2000\r\n"); 
    Serial.setTimeout(2000);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    if(processa_cmd(linha,"OK\r\n"))    
    {
        lcd.clear();
        lcd.print(" Server Ok");
        lcd.setCursor(0, 1);
        lcd.print(" on port 2000");
    }
    else
    {
       lcd.clear();
       lcd.print("Server ERRO");
       while(1);
    }
    delay(1000);
 
}

unsigned char id;
char stemp[20];
int i;
void loop(void)
{
    Serial.setTimeout(200);
    ret=Serial.readBytes(linha, LMAX);
    linha[ret]=0;
    if(ret != 0)//recebeu dado
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
        
        lcd.setCursor(0, 1);
        lcd.print("Rec:");
        lcd.write(id);
        
        acesso++;
        Serial.print("AT+CIPSEND=");
        Serial.write(id);
        sprintf(stemp,",%i\r\n",strlen(resp_i)+strlen(resp_e)+5);
        Serial.print(stemp);
        Serial.setTimeout(200);
        ret=Serial.readBytes(linha, LMAX);
        linha[ret]=0;
        filtra_dados(linha);
    
        Serial.print(resp_i);
        if(acesso < 10000)Serial.print("0");
        if(acesso < 1000)Serial.print("0");
        if(acesso < 100)Serial.print("0");
        if(acesso < 10)Serial.print("0");
        Serial.print(acesso);
        Serial.print(resp_e);
        Serial.setTimeout(200);
        ret=Serial.readBytes(linha, LMAX);
        linha[ret]=0;
        filtra_dados(linha);
        processa_cmd(linha,"SEND OK\r\n"); 
            
        delay(200);
        Serial.print("AT+CIPCLOSE=");
        Serial.write(id);
        Serial.print("\r\n"); 
        Serial.setTimeout(200);
        ret=Serial.readBytes(linha, LMAX);
        linha[ret]=0;
        //filtra_dados(linha);
        processa_cmd(linha,"OK\r\n");
        linha[0]=0;
      }
    }
}
