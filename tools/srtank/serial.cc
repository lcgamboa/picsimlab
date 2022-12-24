
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
#include <glob.h>
#endif



/*-------------------serial------------------------------------*/
        
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
        

char SERIALDEVICE[100];

#ifdef _WIN_
HANDLE phCom;
#define BAUDRATE 19200
#else
int pfd;
#define BAUDRATE B19200
#endif


int 
sopen(const char * port)
{
#ifdef _WIN_
phCom = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
0, // exclusive access
NULL, // no security
OPEN_EXISTING,
0, // no overlapped I/O
NULL); // null template

  if(phCom == INVALID_HANDLE_VALUE)
  {
   return 1;
  }

#else
  pfd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK); 
  if (pfd < 0) 
  {
    perror(port); 
    return 1; 
  }
#endif
  return 0;
}

int 
sclose(void)
{
#ifdef _WIN_
  CloseHandle(phCom);
#else    
  if (pfd >=0) 
  {
    close(pfd);
    pfd=-1;
  }
#endif
  return 0;
}



int
scfg(void)
{
#ifdef _WIN_
  //BOOL bPortReady;
  DCB dcb;
  COMMTIMEOUTS CommTimeouts;

/*bPortReady =*/ GetCommState(phCom, &dcb);
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
dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; //
// set DSRDTR
dcb.fOutxDsrFlow = FALSE; // turn off DSR flow control
//dcb.fDtrControl = DTR_CONTROL_ENABLE; //
dcb.fDtrControl = DTR_CONTROL_DISABLE; //
// dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; //

/*bPortReady =*/ SetCommState(phCom, &dcb);

// Communication timeouts are optional

/*bPortReady =*/ GetCommTimeouts (phCom, &CommTimeouts);

CommTimeouts.ReadIntervalTimeout = MAXDWORD;
CommTimeouts.ReadTotalTimeoutConstant = 0;
CommTimeouts.ReadTotalTimeoutMultiplier = 0;
CommTimeouts.WriteTotalTimeoutConstant = 0;
CommTimeouts.WriteTotalTimeoutMultiplier = 0;

/*bPortReady =*/ SetCommTimeouts (phCom, &CommTimeouts);
	

EscapeCommFunction(phCom,SETRTS );

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
        newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
        
        tcflush(pfd, TCIFLUSH);
        tcsetattr(pfd,TCSANOW,&newtio);
        
	cmd=TIOCM_RTS;
	ioctl(pfd, TIOCMBIS ,&cmd);
#endif   
	return 0; 
}


      
unsigned long ssend(unsigned char c)
{
#ifdef _WIN_
   unsigned long nbytes;
    
   WriteFile(phCom, &c, 1, &nbytes,NULL);
   return nbytes;
#else
  return write (pfd,&c,1);   
#endif
}

unsigned long srec(unsigned char * c)
{
#ifdef _WIN_
    unsigned long nbytes;
      
  if(phCom != INVALID_HANDLE_VALUE)    
    ReadFile(phCom, c, 1,&nbytes, NULL);
  else
    nbytes=0;
#else
    long nbytes;

  if(pfd >= 0)
  {
     nbytes = read (pfd,c,1);   
     if(nbytes<0)nbytes=0;
  }
  else
  {
    nbytes=0;
  }
#endif    
    return nbytes;
}

unsigned long srec_tout(unsigned char * c)
{
 unsigned int tout=0;

#ifdef _WIN_
    unsigned long nbytes;
    do
    { 
      Sleep(1);	
      ReadFile(phCom, c, 1,&nbytes, NULL);
#else
    long nbytes;
    do
    { 
      usleep(100);
      nbytes = read (pfd,c,1);   
      if(nbytes<0)nbytes=0;
#endif    
      tout++;
    }while((nbytes == 0 )&&(tout < 1000));
    return nbytes;
}




char *
slist (void)
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
