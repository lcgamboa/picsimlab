/* ########################################################################

   PICsim - PIC simulator

   ########################################################################

   Copyright (c) : 2008-2016  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef LINUX
#include <conio.h>
#include <time.h>
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <glob.h>
#endif

        
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
        


#ifdef LINUX
int 
#else
HANDLE
#endif
serial_open(const char * sname)
{

#ifndef LINUX
  HANDLE serialfd = CreateFile(sname, GENERIC_READ | GENERIC_WRITE,
0, // exclusive access
NULL, // no security
OPEN_EXISTING,
0, // no overlapped I/O
NULL); // null template

#else
  int serialfd = open(sname, O_RDWR | O_NOCTTY |  O_NONBLOCK );
 
#endif
  return serialfd;
}

int 
#ifdef LINUX
serial_close(int serialfd)
#else
serial_close(HANDLE serialfd)
#endif
{
  if (serialfd != 0) 
  {
#ifndef LINUX
  CloseHandle(serialfd);
#else    
    close(serialfd);
#endif
    serialfd=0;
  }
  return 0;
}



int
#ifdef LINUX
serial_cfg(int serialfd,int baud)
#else
serial_cfg(HANDLE serialfd,int baud)
#endif
{
    unsigned int BAUDRATE;
   

    switch(((int)((baud/300.0)+0.5))) 
    {
       case 0 ... 1:
          #ifdef LINUX
          BAUDRATE=B300;
          #else
          BAUDRATE=300;
          #endif  
          break; 
       case 2 ... 3:
          #ifdef LINUX
          BAUDRATE=B600;
          #else
          BAUDRATE=600;
          #endif  
          break; 
       case 4 ... 7:
          #ifdef LINUX
          BAUDRATE=B1200;
          #else
          BAUDRATE=1200;
          #endif  
          break; 
       case 8 ... 15:
          #ifdef LINUX
          BAUDRATE=B2400;
          #else
          BAUDRATE=2400;
          #endif  
          break; 
       case 16 ... 31:
          #ifdef LINUX
          BAUDRATE=B4800;
          #else
          BAUDRATE=4800;
          #endif  
          break; 
       case 32 ... 63:
          #ifdef LINUX
          BAUDRATE=B9600;
          #else
          BAUDRATE=9600;
          #endif  
          break; 
       case 64 ... 127:
          #ifdef LINUX
          BAUDRATE=B19200;
          #else
          BAUDRATE=19200;
          #endif  
          break; 
       case 128 ... 191:
          #ifdef LINUX
          BAUDRATE=B38400;
          #else
          BAUDRATE=38400;
          #endif  
          break; 
       case 192 ... 383:
          #ifdef LINUX
          BAUDRATE=B57600;
          #else
          BAUDRATE=57600;
          #endif  
          break; 
       default:
          #ifdef LINUX
          BAUDRATE=B115200;
          #else
          BAUDRATE=115200;
          #endif  
          break; 
    } 

#ifndef LINUX
  //BOOL bPortReady;
  DCB dcb;
  COMMTIMEOUTS CommTimeouts;

/*bPortReady =*/ GetCommState(serialfd , &dcb);
dcb.BaudRate = BAUDRATE;
dcb.ByteSize = 8;
dcb.Parity = NOPARITY;
dcb.StopBits = ONESTOPBIT;
dcb.fAbortOnError = TRUE;

// set XON/XOFF
dcb.fOutX = FALSE; // XON/XOFF off for transmit
dcb.fInX = FALSE; // XON/XOFF off for receive
// set RTSCTS
dcb.fOutxCtsFlow = FALSE; // turn off CTS flow control
//dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; //
dcb.fRtsControl = RTS_CONTROL_DISABLE; //
// set DSRDTR
dcb.fOutxDsrFlow = FALSE; // turn off DSR flow control
//dcb.fDtrControl = DTR_CONTROL_ENABLE; //
dcb.fDtrControl = DTR_CONTROL_DISABLE; //
// dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; //

/*bPortReady =*/ SetCommState(serialfd , &dcb);

// Communication timeouts are optional

/*bPortReady =*/ GetCommTimeouts (serialfd , &CommTimeouts);

CommTimeouts.ReadIntervalTimeout = MAXDWORD;
CommTimeouts.ReadTotalTimeoutConstant = 0;
CommTimeouts.ReadTotalTimeoutMultiplier = 0;
CommTimeouts.WriteTotalTimeoutConstant = 0;
CommTimeouts.WriteTotalTimeoutMultiplier = 0;

/*bPortReady =*/ SetCommTimeouts (serialfd , &CommTimeouts);
	

EscapeCommFunction(serialfd ,SETRTS );

#else
   struct termios newtio;
   int cmd;   
        
//        tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE |CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR|IGNBRK;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0; /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
        
        tcflush(serialfd, TCIFLUSH);
        tcsetattr(serialfd,TCSANOW,&newtio);
        
	cmd=TIOCM_RTS;
	ioctl(serialfd, TIOCMBIS ,&cmd);
#endif

	return 0; 
}


#ifdef LINUX      
unsigned long serial_send(int serialfd,const unsigned char * c, unsigned int len)
#else
unsigned long serial_send(HANDLE serialfd,const unsigned char * c, unsigned int len)
#endif
{
  if(serialfd)
  {
    printf("Send=[\n%s]\n",c);
#ifndef LINUX
   unsigned long nbytes;
    
   WriteFile(serialfd, c, len, &nbytes,NULL);
 
   return nbytes;
#else
  return write (serialfd,c,len);   
#endif
  }
  else
    return 0;
}

#ifdef LINUX
unsigned long serial_rec(int serialfd, unsigned char * c, unsigned int len)
#else
unsigned long serial_rec(HANDLE serialfd, unsigned char * c, unsigned int len)
#endif
{
  if(serialfd)
  {
#ifndef LINUX
    unsigned long nbytes;
      
    ReadFile(serialfd, c, len,&nbytes, NULL);
#else
    long nbytes;

     nbytes = read (serialfd,c,len);   
     if(nbytes<0)nbytes=0;
#endif    
    return nbytes;
   }
   else
     return 0;
}

/*
unsigned long serial_rec_tout(int serialfd, unsigned char * c)
{
 unsigned int tout=0;

  if(serialfd)
  {
#ifndef LINUX
    unsigned long nbytes;
    do
    { 
      Sleep(1);	
      ReadFile(pic->serialfd, c, 1,&nbytes, NULL);
#else
    long nbytes;
    do
    { 
      usleep(100);
      nbytes = read (serialfd,c,1);   
      if(nbytes<0)nbytes=0;
#endif    
      tout++;
    }while((nbytes == 0 )&&(tout < 1000));
    return nbytes;
   }
   else
     return 0;
}
*/



/*
void serial(_pic * pic)
{
  unsigned char rctemp;


  if((*pic->serial_RCSTA & 0x80)==0x80)
  {
    if(pic->s_open == 0) 
    {

      if(pic->serialfd > 0)
      {
        serial_cfg(pic);
        pic->s_open=1;
       if(pic->print)printf("#Open Port:%s!\n",pic->SERIALDEVICE);
      }
      else
      {
        if(pic->print)printf("#Erro Open Port:%s!\n",pic->SERIALDEVICE);
        pic->s_open=-1;
      }
      *pic->serial_TXSTA |=0x02; //TRMT=1 empty 
      *pic->serial_PIR1 |=0x10; //TXIF=1  
      pic->txtc=-1;
    }
   pic->pins[pic->usart[0]-1].ptype=PT_USART;
   pic->pins[pic->usart[1]-1].ptype=PT_USART;
   
   if(pic->flowcontrol)pic_set_pin(pic, pic->rtspin,0); //enable send
  }
  else
  {
    if(pic->s_open == 1)
    {
      pic->s_open=0;
    }
   pic->pins[pic->usart[0]-1].ptype=PT_CMOS;
   pic->pins[pic->usart[1]-1].ptype=PT_CMOS;
   if(pic->flowcontrol)pic_set_pin(pic, pic->rtspin,1); //disable send
  }
  

   if(pic->lram == pic->serial_TXREG_ADDR)    
    {
      pic->txtc++;
      if(pic->txtc > 1)pic->txtc=1; 
      pic->txtemp[(unsigned char)pic->txtc]= *pic->serial_TXREG;
      *pic->serial_TXSTA &=~0x02; //TRMT=0 full   
      *pic->serial_PIR1 &=~0x10; //TXIF=0 trasmiting
    }
  
    //envia byte para TSTR
    if((*pic->serial_TXSTA & 0x02)&&(pic->txtc >= 0)) 
    {
         pic->txtc--;
         
         if(!pic->txtc)
         {
           pic->txtemp[(unsigned char)pic->txtc]= *pic->serial_TXREG;
           *pic->serial_TXSTA &=~0x02; //TRMT=0 full   
           *pic->serial_PIR1 &=~0x10; //TXIF=0 trasmiting
         }
    }

   if(pic->lram == pic->serial_RCSTA_ADDR)
   {               //CREN 
      if((*pic->serial_RCSTA & 0x10) == 0)
      {  
       *pic->serial_RCSTA &=~0x02; //clear OERR
       pic->serialc=0;
      }
   }    


   if(pic->rram == pic->serial_RCREG_ADDR)    
    { 
      switch(pic->recb)
      {
        case 1:
          *pic->serial_RCREG=0;
          *pic->serial_PIR1 &=~0x20; //clear RCIF
          pic->recb--;    
          break;
        case 2:
          *pic->serial_RCREG=pic->RCREG2;
          pic->RCREG2=0; 
	  pic->recb--;    
          break;
        default:
        break; 
      }

    }


    pic->serialc++;

    if(*pic->serial_TXSTA & 0x04)
    {
       //BRGH=1  start + 8 bits + stop
       if(pic->serialc >= (((*pic->serial_SPBRG)+1)*40))pic->sr =1;
    }
    else
    {
       //BRGH=0  start + 8 bits + stop
       if(pic->serialc >= (((*pic->serial_SPBRG)+1)*160))pic->sr =1;
    }

  
    if(pic->sr ==1 )
    {
    
     pic->serialc=0;
     
      
      if((pic->s_open != 0)&&(( *pic->serial_TRIS_RX &  pic->serial_TRIS_RX_MASK) != 0)) //work only if RX tris bit is set
     {
      if(serial_recbuff(pic,&rctemp) == 1)
      {

        if((*pic->serial_RCSTA & 0x12) == 0x10)//CREN=1  OERR=0 
        { 
         switch(pic->recb)
         {
         case 0: 
           *pic->serial_RCREG=rctemp;
	   pic->RCREG2=0;
           pic->recb++;
           break;
         case 1: 
           pic->RCREG2=rctemp;
           pic->recb++;
           break; 
         default: 
           *pic->serial_RCSTA |=0x02; //set OERR
           break; 
          }
        }
        
   //       printf("reb=%i temp=%02X RCREG=%02X RECREG2=%02X  RCSTA=%02X\n",pic->recb,rctemp,pic->ram[RCREG],pic->RCREG2,pic->ram[RCSTA]);
        
         if(((*pic->serial_PIE1 & 0x20) == 0x20)&&((*pic->serial_PIR1 & 0x20) != 0x20))
         {
           if(pic->print)printf("serial rx interrupt (%#04X)\n",rctemp);
         }
         //set RCIF
         *pic->serial_PIR1 |=0x20;  
      }
     } 
    
      //if(((pic->ram[P18_TXSTA] & 0x02) == 0 ) &&((pic->ram[pic->pins[pic->usart[1]-1].port+0x12] &  (0x01 << pic->pins[pic->usart[1]-1].pord)) == 0))
      if((*pic->serial_TXSTA & 0x02) == 0 )
       {   
        if(pic->s_open == 1 ) serial_send(pic,pic->txtemp[0]);
        *pic->serial_TXSTA |=0x02; //TRMT=1 empty  
        
        if(((*pic->serial_PIE1 & 0x10) == 0x10)&&((*pic->serial_PIR1 & 0x10) != 0x10))
        {
          if(pic->print)printf("serial tx interrupt (%#04X)\n",pic->txtemp[0]);
        }
        *pic->serial_PIR1 |=0x10; //TXIF=1  
      }   
      pic->sr=0;
    }

//Hardware flowcontrol

}

void 
pic_set_serial(_pic * pic, const char * name, int flowcontrol,int ctspin,int  rtspin)
{
  strcpy(pic->SERIALDEVICE,name);
  
  pic->flowcontrol=flowcontrol;
  pic->serialfd=0;
  if(flowcontrol == 1)
  {
    pic->ctspin=ctspin;
    pic->rtspin=rtspin;
  }
};
*/


char *
serial_list (void)
{
 char *resp = NULL;
 unsigned int i = 0;
 int length;
#ifdef _WIN_
 HKEY key;
 char *value, *data;
 DWORD max_value_len, max_data_size;
 DWORD value_len, data_size;
 DWORD type;
 int ret = 1;

 if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
  {
   printf (" %i RegOpenKeyEx() failed\n", ret);
   return resp;
  }
 if (RegQueryInfoKey (key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &max_value_len, &max_data_size, NULL, NULL) != ERROR_SUCCESS)
  {
   printf ("%i RegQueryInfoKey() failed\n", ret);
   RegCloseKey (key);
   return resp;
  }
 if (!(value = (char *) malloc ((max_value_len + 1))))
  {
   printf (" %i Registry value malloc failed\n", ret);
   RegCloseKey (key);
   return resp;
  }
 if (!(data = (char *) malloc ((max_data_size + 1))))
  {
   printf ("%i Registry data malloc failed\n", ret);
   free (value);
   RegCloseKey (key);
   return resp;
  }

 i = 0;
 length = 0;
 while (value_len = max_value_len + 1,
        data_size = max_data_size,
        RegEnumValue (key, i, value, &value_len, NULL, &type, (LPBYTE) data, &data_size) == ERROR_SUCCESS)
  {
   if (type == REG_SZ)
    {
     length += data_size;
    }
   i++;
  }

 if (length > 0)
  {
   resp = (char *) malloc (length + 1);
   if (!resp)
    {
     printf ("Resp malloc failed\n");
     free (value);
     free (data);
     RegCloseKey (key);
     return resp;
    }

   resp[0] = 0;
   i = 0;

   while (value_len = max_value_len + 1,
          data_size = max_data_size,
          RegEnumValue (key, i, value, &value_len, NULL, &type, (LPBYTE) data, &data_size) == ERROR_SUCCESS)
    {
     if (type == REG_SZ)
      {
       strcat (resp, data);
       strcat (resp, ",");
      }
     i++;
    }
  }

 free (data);
 free (value);
 RegCloseKey (key);

#else
 int ret;
 glob_t globbuf;

 ret = glob ("/dev/ttyS*", 0, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }

 ret = glob ("/dev/ttyUSB*", GLOB_APPEND, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }
 ret = glob ("/dev/ttyACM*", GLOB_APPEND, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }
 ret = glob ("/dev/ttyAMA*", GLOB_APPEND, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }
 ret = glob ("/dev/rfcomm*", GLOB_APPEND, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }
 ret = glob ("/dev/tnt*", GLOB_APPEND, NULL, &globbuf);
 if ((ret != 0)&&(ret != GLOB_NOMATCH))
  {
   printf ("glob error %i\n", ret);
   return resp;
  }


 length = 0;
 for (i = 0; i < globbuf.gl_pathc; i++)
  {
   length += strlen (globbuf.gl_pathv[i]) + 1;
  }

 if (length > 0)
  {
   resp = (char *) malloc (length + 1);
   if (!resp)
    {

     printf ("Resp malloc failed\n");
     return resp;
    }

   resp[0] = 0;
   for (i = 0; i < globbuf.gl_pathc; i++)
    {
     strcat (resp, globbuf.gl_pathv[i]);
     strcat (resp, ",");
    }
  }

 globfree (&globbuf);


#endif

 return resp;
}
