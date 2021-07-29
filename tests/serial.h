/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gamboa Lopes

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

#ifndef SERIAL_H
#define SERIAL_H

#ifndef _WIN_
//#include <termios.h>
#include "/usr/include/asm-generic/termbits.h"
#include "/usr/include/asm-generic/ioctls.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#else
#include <windows.h>
#endif



class cserial
{
private:
public:
#ifndef _WIN_
  int serialfd;
#else
  HANDLE serialfd;
#endif
  char buff[500];
  char name[50];

#ifndef _WIN_

  int
  Open (const char* portname)
  {
    serialfd = open (portname, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (serialfd < 0)
      {
        perror (portname);
        printf ("Erro on Port Open:%s!\n", portname);
        return 0;
      }

    strcpy(name,portname);
    printf ("Port Open:%s!\n", portname);
    return 1;

  }

  int
  Close (void)
  {
    if (serialfd != 0)
      {
        close (serialfd);
        serialfd = 0;
      }
    return 0;
  }

  int
  Config (unsigned int speed)
  {
    struct termios2 tio;
    ioctl(serialfd, TCGETS2, &tio);
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_cflag |=  CS8 | CLOCAL | CREAD;
    tio.c_ispeed = speed;
    tio.c_ospeed = speed;
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_lflag = 0; 
    tio.c_cc[VTIME] = 0; /* inter-character timer unused */
    tio.c_cc[VMIN] = 0; /* blocking read until 5 chars received */
    return  ioctl(serialfd, TCSETS2, &tio);
  }

  int
  Send (unsigned char c)
  {
    if (serialfd)
      {
        return write (serialfd, &c, 1);
      }
    else
      return 0;
  }
  
  int
  Send (unsigned char * c, int size)
  {
    if (serialfd)
      {
        return write (serialfd, c, size);
      }
    else
      return 0;
  }

  int
  Receive (unsigned char * c)
  {
    if (serialfd)
      {
        return read (serialfd, c, 1);
      }
    else
      return 0;
  }

  int
  Receive (unsigned char * c, int size)
  {
    if (serialfd)
      {
	 int ret;     
	 int lido=0;
         unsigned int cont =0;
	 do{
           ret=read (serialfd, &c[lido], size-lido);
	   if(ret > 0)lido+=ret;
	   usleep(1000);
	   cont++;
	 }while((lido < size)&&(cont < 1000));
        return lido; 
      }
    else
      return 0;
  }

void
 SetModem(unsigned long data)
{
   ioctl (serialfd, TIOCMSET, &data);
}

unsigned int  GetModem(void)
{
  unsigned long state;
  ioctl (serialfd, TIOCMGET, &state);
  return state;
}

#else

  int
  Open (LPCSTR portname)
  {

    serialfd = CreateFile (portname, GENERIC_READ | GENERIC_WRITE,
                           0, // exclusive access
                           NULL, // no security
                           OPEN_EXISTING,
                           0, // no overlapped I/O
                           NULL); // null template
    if (serialfd == INVALID_HANDLE_VALUE)
      {
        serialfd = 0;
        printf ("Erro on Port Open:%s!\n", portname);
        return 0;
      }
    strcpy(name,portname);
    printf ("Port Open:%s!\n", portname);
    return 1;
  }

  int
  Close (void)
  {
    if (serialfd != 0)
      {
        CloseHandle (serialfd);
        serialfd = 0;
      }
    return 0;
  }

  int
  Config (unsigned int BAUDRATE)
  {
    switch (BAUDRATE / 300)
      {
      case 1:
        BAUDRATE = 300;
        break;
      case 2:
        BAUDRATE = 600;
        break;
      case 4:
        BAUDRATE = 1200;
        break;
      case 8:
        BAUDRATE = 2400;
        break;
      case 16:
        BAUDRATE = 4800;
        break;
      case 32:
        BAUDRATE = 9600;
        break;
      case 64:
        BAUDRATE = 19200;
        break;
      case 128:
        BAUDRATE = 38400;
        break;
      case 192:
        BAUDRATE = 57600;
        break;
      default:
        BAUDRATE = 115200;
        break;
      }

    //printf("baud = %i \n",BAUDRATE);

    BOOL bPortReady;
    DCB dcb;
    COMMTIMEOUTS CommTimeouts;

    bPortReady = GetCommState (serialfd, &dcb);
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

    bPortReady = SetCommState (serialfd, &dcb);

    // Communication timeouts are optional

    bPortReady = GetCommTimeouts (serialfd, &CommTimeouts);

    CommTimeouts.ReadIntervalTimeout = MAXDWORD;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;

    bPortReady = SetCommTimeouts (serialfd, &CommTimeouts);


    //EscapeCommFunction (serialfd, SETRTS);

    return 0;
  }

  int
  Send (unsigned char c)
  {
    if (serialfd)
      {
        unsigned long nbytes;

        WriteFile (serialfd, &c, 1, &nbytes, NULL);
        return nbytes;
      }
    else
      return 0;
  }
  
  int
  Send (unsigned char * c, int size)
  {
    if (serialfd)
      {
        unsigned long nbytes;

        WriteFile (serialfd, c, size, &nbytes, NULL);
        return nbytes;
      }
    else
      return 0;
  }


  int
  Receive (unsigned char * c)
  {
    if (serialfd)
      {
        unsigned long nbytes;

        ReadFile (serialfd, c, 1, &nbytes, NULL);
        return nbytes;
      }
    else
      return 0;
  }

void
 SetModem(unsigned long data)
{
   EscapeCommFunction (serialfd, SETRTS);
}

unsigned int  GetModem(void)
{
  unsigned long state;
  GetCommModemStatus(serialfd ,&state);
  return state;
}

#endif

  const char *GetName(void)
  {
    return name;
  }

  void
  SendStr (const char * str)
  {
    for (unsigned int i = 0; i < strlen (str); i++)
      Send (str[i]);
  }

  const char *
  RecStr (void)
  {
    int i = -1;

    do
      {
        i++;
        Receive ((unsigned char *) &buff[i]);
      }
    while ((buff[i] != 0) && ((buff[i] != '\n')));
    buff[i + 1] = '\0';

    return buff;
  }
};

#endif /* SERIAL_H */

