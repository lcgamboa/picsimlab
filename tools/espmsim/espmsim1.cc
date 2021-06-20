
#include"tcp.h"

#include"espmsim1.h"

#ifndef CONSOLE
#include"espmsim1_d.cc"
#else
CPWindow1::CPWindow1(void){};
#endif

#include"serial.h"
#include"espmsim2.h"


CPWindow1 Window1;


//Implementation




void
CPWindow1::togglebutton1_EvOnToggleButton(CControl * control)
{
   combo1.SetEnable(!togglebutton1.GetCheck());

   if(togglebutton1.GetCheck())
   {
    

     sfd=serial_open(combo1.GetText ().char_str ());
     
#ifdef LINUX 
     if(sfd < 0)
#else
    if(sfd == INVALID_HANDLE_VALUE)
#endif     
      {
        togglebutton1.SetCheck (0);
        combo1.SetEnable(1);
        text1.AddLine (lxT("Error on open serial port\n"));
        return;
      }
     
     togglebutton1.SetText(lxT("Disconnect"));
     text1.Clear ();     
     serial_cfg(sfd,115200);
     bptr=buff;
     blen=0;
     timer1.SetRunState (1);
   }
   else
   {
     timer1.SetRunState (0);  
     togglebutton1.SetText(lxT("Connect"));
     serial_close(sfd);
   }

};


void
CPWindow1::_EvOnCreate(CControl * control)
{
  char * resp = serial_list ();

 if (resp)
  {
   combo1.SetItems (resp);
   combo1.SetText (combo1.GetItem (0));  
   free (resp);
  }
 else
  {
   printf ("No serial ports found!\n");
  }
  

  skt_start();

#ifdef _WIN_ 
 wxFileName fexe(wxStandardPaths::Get().GetExecutablePath());
 wxSetWorkingDirectory(fexe.GetPath());
#endif

};


void
CPWindow1::_EvOnDestroy(CControl * control)
{
  timer1.SetRunState (0);
  skt_stop();
};




int 
CPWindow1::search_cmd(char * buff, int blen)
{
    int i;
    int j;
    char cmd[100];
    
    for(i=0;i<blen;i++)
    {
        if((buff[i] == '\r')&&(buff[i+1] == '\n'))
        {
           
            strncpy(cmd,buff,i);
            cmd[i]=0;
            
            execute_cmd(cmd);
                  
            for(j=0;j< blen-2-i;j++)
            {
                buff[j]=buff[j+i+2];
            }
            
            buff[i+2]=0;
            //printf("resto buffer=[%s]",buff);
            
            return i+2;
        }
    
    }

    return 0;
}


void 
CPWindow1::execute_cmd(char * cmd)
{
    char resp[512];

    strcpy(resp,"\r\n\r\nERROR\r\n");
    
    printf("cmd=[%s]\n",cmd);
#ifndef CONSOLE
    text1.AddLine (lxT("cmd=[")+lxString(cmd)+lxT("]\n"));
#endif        
    if(!strcmp(cmd,"AT"))
    {
        strcpy(resp,"\r\n\r\nOK\r\n");    
    }
    
    if(!strcmp(cmd,"AT+RST"))
    {
        strcpy(resp,"\r\n\r\nOK\r\n");    
        serial_send(sfd,(unsigned char *) resp,strlen(resp));
  
	//close sockets
	for(int i=0;i<4;i++)
        {
          if(SCLI[i] > 0)skt_close(SCLI[i]);
          SCLI[i]=-1;
        }
        if(SKL > 0)skt_close(SKL);
          SKL=-1;
        
        CIPSERVER=0;  
        PORT=0;
        CIPMUX=0;
        CIPMODE=0;
        WCON=0;
        
        
        skt_stop();
        sleep(1);
        skt_start();

        strcpy(resp,"WIFI DISCONNECT\r\n\r\n"
" ets Jan  8 2013,rst cause:1, boot mode:(3,7)\r\n\r\n"
"load 0x40100000, len 1396, room 16\r\n" 
"tail 4\r\n"
"chksum 0x89\r\n"
"load 0x3ffe8000, len 776, room 4\r\n" 
"tail 4\r\n"
"chksum 0xe8\r\n"
"load 0x3ffe8308, len 540, room 4\r\n" 
"tail 8\r\n"
"chksum 0xc0\r\n"
"csum 0xc0\r\n\r\n"
"2nd boot version : 1.4(b1)\r\n"
"  SPI Speed      : 40MHz\r\n"
"  SPI Mode       : QIO\r\n"
"  SPI Flash Size & Map: 8Mbit(512KB+512KB)\r\n"
"jump to run user1 @ 1000\r\n\r\n"
"rl�B�\r\n"
"ready\r\n");  
    }
  
    if(!strcmp(cmd,"AT+GMR"))
    {
        strcpy(resp,"\r\nAT version:0.51.0.0(Nov 27 2015 13:37:21\r\n"
                    "SDK version:1.5.0\r\n"
                    "compile time:Nov 27 2015 13:58:02\r\n"
                    "\r\nOK\r\n");    
    }
    
    
  /*  
    AT+CWLAP="N16FU","20:cf:30:ce:60:fe",11
  */          
            
    if(!strcmp(cmd,"AT+CWLAP"))
    {    
        if(CWMODE != 2)
        {
        strcpy(resp,"\r\n"
"+CWLAP:(4,\"rede1\",-91,\"30:b5:c2:2b:58:de\",1)\r\n"
"+CWLAP:(0,\"netmail12\",-88,\"00:0c:42:18:c6:4c\",2)\r\n"
"+CWLAP:(0,\"netmail10\",-91,\"00:0c:42:1f:1d:81\",7)\r\n"
"+CWLAP:(0,\"netmail11\",-84,\"00:0c:42:1f:73:2e\",9)\r\n\r\n"
"OK\r\n");
        sleep(1);
          }
    }
/*    
AT+CWJAP?

+CWJAP:"lccasa"
        No AP

OK
        
AT+CWJAP="lccasa","171171171b"

+CWJAP:1

FAIL        
*/            
    if(strstr(cmd,"AT+CWJAP="))
    {
        if(CWMODE != 2)
        {
           sscanf(cmd,"AT+CWJAP=%s",SSID);   
           
           printf("ssid=[%s]\n",SSID); 
                   
           if(!strcmp(SSID,"\"rede1\",\"123456\""))
           {  
             strcpy(resp,"\r\n\r\nWIFI CONNECTED\r\n"
                         "WIFI GOT IP"
                         "\r\n\r\nOK\r\n");
             WCON=1;
           }
           else
             WCON=0;
           sleep(2);
        }
    }
/*    
AT+CWQAP=?


OK
        
        
AT+CWQAP


OK
  */      
    
    
    if(strstr(cmd,"AT+CWDHCP="))
    {
        strcpy(resp,"\r\n\r\nOK\r\n");    
    }

    if(!strcmp(cmd,"AT+CIFSR"))
    {
      if(WCON)  
         strcpy(resp,"\r\n+CIFSR:STAIP,\"127.0.0.1\"\r\n"
                  "+CIFSR:STAMAC,\"11:22:33:44:55:66\"\r\n"
                  "\r\nOK\r\n");    
      
  //getifaddrs(&addrs);
      /*
  char name[256];
  char *ip;
  hostent *hostinfo;
  
  if( gethostname ( name, sizeof(name)) == 0)
  {
   printf("Host name: %s\n", name);
 
   if((hostinfo = gethostbyname(name)) != NULL)
   {
    int nCount = 0;
    while(hostinfo->h_addr_list[nCount])
    {
     ip = inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[nCount]);
 
     printf("IP #%d: %s\n", ++nCount, ip);
    }
   }
    }
   */
    }

/*
AT+ CWSAP?


ERROR
        
AT+ CWSAP=        


AT+CWMODE?

+CWMODE:1

OK
*/
    

    if(strstr(cmd,"AT+CWMODE"))
    {
        if(cmd[strlen(cmd)-2]=='=')
        {
          if(cmd[strlen(cmd)-1] != '?')
          {
            int mode;  
            mode=cmd[strlen(cmd)-1]-'0';
            
            if((mode >= 0)&&(mode <= 3))
            {
              CWMODE=mode;
              strcpy(resp,"\r\n\r\nOK\r\n");
            }
          }
          else
          {
              cmd[strlen(cmd)-2]=0;
              sprintf(resp,"+%s:%i\r\n\r\nOK\r\n",cmd,CWMODE);
          }
        }
        else
        {
           cmd[strlen(cmd)-1]=0; 
           sprintf(resp,"+%s:%i\r\n\r\nOK\r\n",cmd,CWMODE); 
        }
    }

  
/*        
AT+CIPSTART=?

+CIPSTART:(id)("type"),("ip address"),(port)
+CIPSTART:((id)"type"),("domain name"),(port)

OK

AT+CIPSTART =
        
AT+CIPMUX?

+CIPMUX:0

OK
*/        
    if(strstr(cmd,"AT+CIPMUX="))
    {
        if((CIPSERVER == 0)&&(CIPMODE ==0 ))
          {
          sscanf(cmd,"AT+CIPMUX=%i",&CIPMUX);
          strcpy(resp,"\r\n\r\nOK\r\n");
          }
    }

    if(!strcmp(cmd,"AT+CIPSTATUS"))
    {
        strcpy(resp,"\r\nSTATUS:4\r\n"
                    "\r\nOK\r\n");    
    }   
            
    if(strstr(cmd,"AT+CIPSERVER="))
    {
        sscanf(cmd,"AT+CIPSERVER=%i,%i",&CIPSERVER,&PORT);   
        
        printf("server=%i\n", CIPSERVER);
        printf("port=%i\n", PORT);
        
        if(CIPSERVER)
          {
            if(SKL < 0)//only if server is off 
	    {
              SKL=skt_server(PORT);
              if(SKL > 0)
                strcpy(resp,"\r\n\r\nOK\r\n");
              else
                CIPSERVER=0;
	    }  
          }
        else
          {
            for(int i=0;i<4;i++)
              {
                if(SCLI[i] > 0)skt_close(SCLI[i]);
                SCLI[i]=-1;
              }
            if(SKL > 0)skt_close(SKL);
            SKL=-1;
            strcpy(resp,"\r\n\r\nOK\r\n");  
          }  
    }
/*    
   AT+CIPSTO?

+CIPSTO:180

OK
AT+CIPSTO=200


ERROR
                  
           
AT+CIUPDATE
*/           
           
    if(strstr(cmd,"AT+CIPSEND="))
    {
       int link;
       int size;
       
        if(CIPMUX)
            sscanf(cmd,"AT+CIPSEND=%i,%i",&link,&size);   
        else
            sscanf(cmd,"AT+CIPSEND=%i",&size); 

       if(size > 2048)size=2048;	
       
       if(((link >=0)&&(link <= 3))||(CIPMUX == 0))
        if(SCLI[link] > 0)
          {
        
           serial_send(sfd,(const unsigned char *)"\r\n\r\nOK\r\n>",9);
       
           int r=0;
           do{
              r+=serial_rec (sfd,(unsigned char *)SBUFF+r,size-r);
           }while(r < size);
          
           if(CIPMUX)
	   {
              if((r= skt_send (SCLI[link],(const char *)SBUFF,size)) < 0)
	      {
                skt_close (SCLI[link]);
		SCLI[link]=-1;      
	        printf("Send Error!\n");    
	      }
	   }
           else
	   {
              if((r= skt_send (SCLI[0],(const char *)SBUFF,size)) < 0)
	      {
                skt_close (SCLI[0]);
	        SCLI[0]=-1; 
	        printf("Send Error!\n");    
	      }
	   }


           char str[100];
           sprintf(str,"\r\nRecv %i bytes\r\n",r);
           serial_send(sfd,(const unsigned char *) str, strlen(str)); 
    
	   printf("DATA=[%s]\n",SBUFF);
        
           strcpy(resp,"SEND OK\r\n");
          }
   

    }

    if(strstr(cmd,"AT+CIPCLOSE="))
    {
        int link;
        
        sscanf(cmd,"AT+CIPCLOSE=%i",&link);   

        if((link >=0)&&(link <= 3))
        if(SCLI[link] >=0)
        {
          skt_close (SCLI[link]);
          SCLI[link]=-1;
          sprintf(resp,"\r\n%i,CLOSED\r\n"
                       "\r\nOK\r\n",link);
        }

    }            
/*

---ATE1
OK
ATE0


OK


AT+CWLIF


AT+CWDHCP

AT+CIPSTAMAC?

+CIPSTAMAC:"18:fe:34:a7:08:d6"

OK

AT+CIPSTA

AT+CIPAP

AT+CIPMODE?

+CIPMODE:0

OK
*/ 

    
    serial_send(sfd,(unsigned char *) resp,strlen(resp)); 
}


void
CPWindow1::timer1_EvOnTime(CControl * control)
{
  long r;
  
    r=serial_rec(sfd,(unsigned char *) bptr,500-blen);

    if(r > 0)
    {
      
       //echo 
      serial_send(sfd,(unsigned char *) bptr,r);   
        
      blen+=r;   
      if((r=search_cmd(buff,blen)))
      {
         blen-=r; 
         bptr=buff+blen;
         
         while((r=search_cmd(buff,blen)))
         {
           blen-=r; 
           bptr=buff+blen;
         }
         
      }  
      else
      {
         bptr=buff+blen;
         if(blen > 500)
         {
             printf("Buffer overflow\n");
             timer1.SetRunState (0);
         }
      }      
      
    };
    
    //TCP
    
    if((CIPSERVER)&&(WCON))
      {
        if(SKL > 0)
          {
            for(int i=0;i<4;i++)
              {
                if(SCLI[i] < 0) //try to connect
                  {
                    SCLI[i]= skt_accept (SKL);
                    if(SCLI[i] >= 0)
                    {
                       char cmd[100]; 
                       sprintf(cmd,"%i,CONNECT\r\n\r\n",i);  
                       serial_send(sfd,(unsigned char *) cmd, strlen(cmd)); 
                    }
                  }
                
                if(SCLI[i] >= 0)
                  {
                    int size=skt_recv(SCLI[i],RBUFF,2048);
                    
                    if(size >0)//data received
                      {
                       char cmd[100];
                       
                       sprintf(cmd,"+IPD,%i,%i:",i,size); 
                       serial_send(sfd,(unsigned char *) cmd, strlen(cmd)); 
                       serial_send(sfd,(unsigned char *) RBUFF, size); 
                       serial_send(sfd,(unsigned char *) "\r\n", 2); 
                      }
                    else if (size == 0)//close socket
                      {
                         //char cmd[100];
                                               
                         if(SCLI[i] > 0)skt_close(SCLI[i]);
                         SCLI[i]=-1;
                         //TODO verficar no esp8266 
                         //sprintf(cmd,"%i,CLOSED\r\n",i);
                         //serial_send(sfd,(unsigned char *) cmd, strlen(cmd)); 
                             // printf ("read error %i: %s \n",size, strerror (errno)); 
                      }
                  }
                
              }
          }
      
      }
    
    
};


void
CPWindow1::timer2_EvOnTime(CControl * control)
{
#ifndef CONSOLE
  text2.Clear ();
  text2.AddLine (lxT("WIFI CONNECTED=")+itoa(WCON)+lxT("\n"));
  text2.AddLine (lxT("CWMODE=")+itoa(CWMODE)+lxT("\n"));
  text2.AddLine (lxT("\n"));
  text2.AddLine (lxT("CIPMODE=")+itoa(CIPMODE)+lxT("\n"));
  text2.AddLine (lxT("CIPMUX=")+itoa(CIPMUX)+lxT("\n")); 
  text2.AddLine (lxT("CIPSERVER=")+itoa(CIPSERVER)+lxT("    PORT=")+itoa(PORT)+lxT("\n")); 
  text2.AddLine (lxT("\n"));
  text2.AddLine (lxT("SKL=")+itoa(SKL)+lxT(" C0=")+itoa(SCLI[0])+lxT(" C1=")+itoa(SCLI[1])+lxT(" C2=")+itoa(SCLI[2])+lxT(" C3=")+itoa(SCLI[3])+lxT("\n")); 
#endif
};


void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
 WDestroy ();
}

void
CPWindow1::menu1_Help_Contents_EvMenuActive(CControl * control)
{
#ifdef EXT_BROWSER
 lxString stemp;
 stemp.Printf (lxT ("https://lcgamboa.github.io/picsimlab_docs/%s/Esp8266ModemSimulator.html"), lxT (_VERSION_));
 lxLaunchDefaultBrowser (stemp);
#else	
  Window2.html1.SetLoadFile(lxString(lxT(_SHARE_))+lxT("docs/espmsim/espmsim.html"));  
  Window2.Show();
#endif  
}

void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
 Message (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ") + lxString (lxT (_VERSION_)));
}


