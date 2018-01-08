/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

//serial stuff
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN_
#include <conio.h>
#include <time.h>
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#endif

//uart support ============================================================

#ifdef _WIN_
HANDLE serialfd;
HANDLE avr_serial_get_fd(void){return serialfd;};
#else
int serialfd;
int avr_serial_get_fd(void){return serialfd;};
#endif


int 
avr_serial_open(char * SERIALDEVICE)
{
      if(SERIALDEVICE[0] == 0)
      { 	
#ifdef _WIN_
        strcpy(SERIALDEVICE,"COM2");
#else
        strcpy(SERIALDEVICE,"/dev/tnt2");
#endif
      }


#ifdef _WIN_
serialfd = CreateFile(SERIALDEVICE, GENERIC_READ | GENERIC_WRITE,
0, // exclusive access
NULL, // no security
OPEN_EXISTING,
0, // no overlapped I/O
NULL); // null template
  if( serialfd == INVALID_HANDLE_VALUE)
  {
     serialfd=0;
//     printf("Erro on Port Open:%s!\n",SERIALDEVICE);
     return 0; 
  }
#else
  serialfd = open(SERIALDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
 
  if (serialfd < 0) 
  {
    serialfd=0;
    perror(SERIALDEVICE); 
//    printf("Erro on Port Open:%s!\n",SERIALDEVICE);
    return 0; 
  }
//  printf("Port Open:%s!\n",SERIALDEVICE);
#endif
  return 1;
}

int 
avr_serial_close(void)
{
  if (serialfd != 0) 
  {
#ifdef _WIN_
  CloseHandle(serialfd);
#else    
    close(serialfd);
#endif
    serialfd=0;
  }
  return 0;
}



int
avr_serial_cfg(float serialexbaud)
{
    unsigned int BAUDRATE;
    int serialbaud;
    
    /*
    if(*serial_TXSTA & 0x04) //BRGH=1 
    {
       serialexbaud=pic->freq/(16*((*pic->serial_SPBRG) +1));
    }
    else
    {
        serialexbaud=pic->freq/(64*((*pic->serial_SPBRG) +1));
    }
    */
    
        serialbaud=serialexbaud; //FIXME 
      
          #ifndef _WIN_
          BAUDRATE=B9600;
          #else
          BAUDRATE=9600;
          #endif 

/*
    switch(((int)((pic->serialexbaud/300.0)+0.5))) 
    {
       case 0 ... 1:
          pic->serialbaud=300;
          #ifndef _WIN_
          BAUDRATE=B300;
          #else
          BAUDRATE=300;
          #endif  
          break; 
       case 2 ... 3:
          pic->serialbaud=600;
          #ifndef _WIN_
          BAUDRATE=B600;
          #else
          BAUDRATE=600;
          #endif  
          break; 
       case 4 ... 7:
          pic->serialbaud=1200;
          #ifndef _WIN_
          BAUDRATE=B1200;
          #else
          BAUDRATE=1200;
          #endif  
          break; 
       case 8 ... 15:
          pic->serialbaud=2400;
          #ifndef _WIN_
          BAUDRATE=B2400;
          #else
          BAUDRATE=2400;
          #endif  
          break; 
       case 16 ... 31:
          pic->serialbaud=4800;
          #ifndef _WIN_
          BAUDRATE=B4800;
          #else
          BAUDRATE=4800;
          #endif  
          break; 
       case 32 ... 63:
          pic->serialbaud=9600;
          #ifndef _WIN_
          BAUDRATE=B9600;
          #else
          BAUDRATE=9600;
          #endif  
          break; 
       case 64 ... 127:
          pic->serialbaud=19200;
          #ifndef _WIN_
          BAUDRATE=B19200;
          #else
          BAUDRATE=19200;
          #endif  
          break; 
       case 128 ... 191:
          pic->serialbaud=38400;
          #ifndef _WIN_
          BAUDRATE=B38400;
          #else
          BAUDRATE=38400;
          #endif  
          break; 
       case 192 ... 383:
          pic->serialbaud=57600;
          #ifndef _WIN_
          BAUDRATE=B57600;
          #else
          BAUDRATE=57600;
          #endif  
          break; 
       default:
          pic->serialbaud=115200;
          #ifndef _WIN_
          BAUDRATE=B115200;
          #else
          BAUDRATE=115200;
          #endif  
          break; 
    } 
*/
    
#ifdef _WIN_
  BOOL bPortReady;
  DCB dcb;
  COMMTIMEOUTS CommTimeouts;

bPortReady = GetCommState(serialfd , &dcb);
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

bPortReady = SetCommState(serialfd , &dcb);

// Communication timeouts are optional

bPortReady = GetCommTimeouts (serialfd , &CommTimeouts);

CommTimeouts.ReadIntervalTimeout = MAXDWORD;
CommTimeouts.ReadTotalTimeoutConstant = 0;
CommTimeouts.ReadTotalTimeoutMultiplier = 0;
CommTimeouts.WriteTotalTimeoutConstant = 0;
CommTimeouts.WriteTotalTimeoutMultiplier = 0;

bPortReady = SetCommTimeouts (serialfd , &CommTimeouts);
	

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
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
        
        tcflush(serialfd, TCIFLUSH);
        tcsetattr(serialfd,TCSANOW,&newtio);
        
	cmd=TIOCM_RTS;
	ioctl(serialfd, TIOCMBIS ,&cmd);
#endif

	return serialbaud; 
}


      
unsigned long avr_serial_send(unsigned char c)
{
  if(serialfd)
  {
#ifdef _WIN_
   unsigned long nbytes;
    
   WriteFile(serialfd, &c, 1, &nbytes,NULL);
   return nbytes;
#else
  return write (serialfd,&c,1);   
#endif
  }
  else
    return 0;
}

unsigned long avr_serial_rec( unsigned char * c)
{
  if(serialfd)
  {
#ifdef _WIN_
    unsigned long nbytes;
      
    ReadFile(serialfd, c, 1,&nbytes, NULL);
#else
    long nbytes;

     nbytes = read (serialfd,c,1);   
     if(nbytes<0)nbytes=0;
#endif    
    return nbytes;
   }
   else
     return 0;
}

unsigned long avr_serial_rec_tout( unsigned char * c)
{
 unsigned int tout=0;

  if(serialfd)
  {
#ifdef _WIN_
    unsigned long nbytes;
    do
    { 
      Sleep(1);	
      ReadFile(serialfd, c, 1,&nbytes, NULL);
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


unsigned long avr_serial_recbuff( unsigned char * c)
{
   return avr_serial_rec(c);
}

int avr_serial_get_dsr(void)
{
#ifdef _WIN_
   long unsigned int state;
   GetCommModemStatus(serialfd ,&state);
   
   return (state & MS_DSR_ON);
#else     
   int state;
   ioctl(serialfd, TIOCMGET ,&state);
   
   return (state & TIOCM_DSR);
#endif
}