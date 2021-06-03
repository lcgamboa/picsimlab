/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gambôa Lopes

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


#include "../picsimlab1.h"
#include "../picsimlab5.h"
#include "rcontrol.h"
#include "lcd_hd44780.h"

#define dprint if (1) {} else printf

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
//system headers dependent
#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <sys/unistd.h>
#include <fcntl.h>
#else
#include<winsock.h>
#ifndef MSG_WAITALL
#define MSG_WAITALL (1 << 3)
#define MSG_NOSIGNAL 0
#endif
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void setnblock(int sock_descriptor);

void setblock(int sock_descriptor);

//typedef struct sockaddr sockaddr;


static int sockfd = -1;
static int listenfd = -1;
static int server_started = 0;

#define BSIZE 1024
static char buffer[BSIZE];
static int bp = 0;

int
rcontrol_init(unsigned short tcpport)
{
 struct sockaddr_in serv;

 if (!server_started)
  {
   dprint ("rcontrol: init\n");

   if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("rcontrol: socket error : %s \n", strerror (errno));
     return 1;
    };

   int reuse = 1;
   if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
    perror ("rcontrol: setsockopt(SO_REUSEADDR) failed");

   memset (&serv, 0, sizeof (serv));
   serv.sin_family = AF_INET;
   serv.sin_addr.s_addr = htonl (INADDR_ANY);
   serv.sin_port = htons (tcpport);

   if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)) < 0)
    {
     printf ("rcontrol: bind error : %s \n", strerror (errno));
     return 1;
    }

   if (listen (listenfd, SOMAXCONN) < 0)
    {
     printf ("rcontrol: listen error : %s \n", strerror (errno));
     return 1;
    }
   server_started = 1;

  }
 return 0;
}

static int
sendtext(const char * str)
{
 int size = strlen (str);

 if (send (sockfd, str, size, MSG_NOSIGNAL) != size)
  {
   printf ("rcontrol: send error : %s \n", strerror (errno));
   return 1;
  }

 return 0;
}

int
rcontrol_start(void)
{
 struct sockaddr_in cli;
#ifndef _WIN_
 unsigned int clilen;
#else
 int clilen;
#endif
 clilen = sizeof (cli);

 setnblock (listenfd);

 if (
     (sockfd =
      accept (listenfd, (sockaddr *) & cli, & clilen)) < 0)
  {
   return 1;
  }


 setnblock (sockfd);
 dprint ("rcontrol: Client connected!---------------------------------\n");

 memset (buffer, 0, BSIZE);
 bp = 0;

 return sendtext ("\r\nPICSimLab Remote Control Interface\r\n\r\n  Type help to see supported commands\r\n\r\n>");

}

void
rcontrol_stop(void)
{
 dprint ("rcontrol: Client disconnected!---------------------------------\n");
 if (sockfd >= 0)
  {
   shutdown (sockfd, SHUT_RDWR);
   close (sockfd);
  }
 sockfd = -1;
}

void
rcontrol_end(void)
{

 if (server_started)
  {
   rcontrol_stop ();
   dprint ("rcontrol: end\n");
   //shutdown (listenfd, SHUT_RDWR);
   //close (listenfd);
  }
 //listenfd = -1;
 //server_started = 0;
}

static char
decodess(unsigned char v)
{

 switch (v & 0x7F)
  {
  case 0x00:
   return '_';
   break;
  case 0x3F:
   return '0';
  case 0x06:
   return '1';
  case 0x5B:
   return '2';
  case 0x4F:
   return '3';
  case 0x66:
   return '4';
  case 0x6D:
   return '5';
  case 0x7D:
   return '6';
  case 0x07:
   return '7';
  case 0x7F:
   return '8';
  case 0x6F:
   return '9';
  case 0x77:
   return 'A';
  case 0x7C:
   return 'B';
  case 0x58:
  case 0x39:
   return 'C';
  case 0x5E:
   return 'D';
  case 0x79:
   return 'E';
  case 0x71:
   return 'F';
  default:
   return 'i';
  }
}

/* Defined types
 VS - value short
 */
static void
ProcessInput(const char * msg, input_t * Input, int * ret)
{
 lxString stemp;
 if ((Input->name[0] == 'V')&&(Input->name[1] == 'S'))
  {
   short temp = ((*((unsigned char *) Input->status)) << 8) | (*(((unsigned char *) (Input->status)) + 1));
   stemp.Printf ("%s %s= %i\r\n", msg, Input->name, temp);
   *ret += sendtext ((const char *) stemp.c_str ());
  }
 else
  {
   stemp.Printf ("%s %s= %i\r\n", msg, Input->name, *((unsigned char *) Input->status));
   *ret += sendtext ((const char *) stemp.c_str ());
  }
}

/* Defined types
 LD - led
 DS - Display LCD
 MT - DC motor
 GD - step and servo angles
 SS - seven sgments 
 */

static void
ProcessOutput(const char * msg, output_t * Output, int * ret)
{
 lxString stemp;
 char lstemp[200];
 static unsigned char ss = 0; //seven segment 


 if ((Output->name[0] == 'L')&&(Output->name[1] == 'D'))
  {
   stemp.Printf ("%s %s= %3.0f\r\n", msg, Output->name, *((float *) Output->status) - 55);
   *ret += sendtext ((const char *) stemp.c_str ());
  }
 else if ((Output->name[0] == 'D')&&(Output->name[1] == 'S'))
  {
   lcd_t * lcd = (lcd_t*) Output->status;
   snprintf (lstemp, 199, "%s %s= |%.16s\r\n", msg, Output->name, &lcd->ddram_char[0]);

   int size = strlen (lstemp) - 19;
   for (int x = 0; x < size; x++)
    {
     lstemp[x + size + 19] = ' ';
    }
   snprintf (lstemp + (2 * size + 19), 199, "|%.16s\r\n", &lcd->ddram_char[40]);
   *ret += sendtext (lstemp);
  }
 else if ((Output->name[0] == 'M')&&(Output->name[1] == 'T'))
  {
   unsigned char **status = (unsigned char **) Output->status;
   snprintf (lstemp, 199, "%s %s-> dir= %i speed= %3i position= %3i\r\n",
             msg, Output->name, *status[0], *status[1], *status[2]);
   *ret += sendtext (lstemp);
  }
 else if ((Output->name[0] == 'D')&&(Output->name[1] == 'G'))
  {
   snprintf (lstemp, 199, "%s %s-> angle= %5.1f\r\n",
             msg, Output->name, *((float*) Output->status)*180.0 / M_PI);
   *ret += sendtext (lstemp);
  }
 else if ((Output->name[0] == 'S')&&(Output->name[1] == 'S'))
  {
   switch (Output->name[3])
    {
    case 'A':
     ss = 0x00;
     if (*((int *) Output->status) > 60) ss |= 0x01;
     break;
    case 'B':
     if (*((int *) Output->status) > 60) ss |= 0x02;
     break;
    case 'C':
     if (*((int *) Output->status) > 60) ss |= 0x04;
     break;
    case 'D':
     if (*((int *) Output->status) > 60) ss |= 0x08;
     break;
    case 'E':
     if (*((int *) Output->status) > 60) ss |= 0x10;
     break;
    case 'F':
     if (*((int *) Output->status) > 60) ss |= 0x20;
     break;
    case 'G':
     if (*((int *) Output->status) > 60) ss |= 0x40;
     break;
    case 'P':
     if (*((int *) Output->status) > 60) ss |= 0x80;
     stemp.Printf ("%s SS_%c= %c\r\n", msg, Output->name[4], decodess (ss));
     *ret += sendtext ((const char *) stemp.c_str ());
     break;
    }
  }
}

static char
pintypetoletter(unsigned char type)
{
 switch (type)
  {
  case PT_POWER:
   return 'P';
   break;
  case PT_DIGITAL:
   return 'D';
   break;
  case PT_ANALOG:
   return 'A';
   break;
  case PT_ANAREF:
   return 'R';
   break;
  case PT_USART:
   return 'U';
   break;
  case PT_NC:
   return '-';
   break;
  }
 return '?';
}

int
rcontrol_loop(void)
{
 int i, j;
 int n;
 int ret = 0;
 lxString stemp;
 char lstemp[200];
 board * Board;
 part * Part;
 input_t * Input;
 output_t * Output;
 const picpin * pins;

 //open connection  
 if (sockfd < 0)
  {
   return rcontrol_start ();
  }

 n = recv (sockfd, (char *) &buffer[bp], 1024 - bp, 0);

 if (n > 0)
  {
   //remove putty telnet handshake
   if (buffer[bp + n] == 3)
    {
     for (int x = 0; x < bp + n + 1; x++)
      {
       buffer[x] = 0;
      }
     n = 0;
     bp = 0;
    }

   if (strchr (buffer, '\n'))
    {
     char cmd[100];
     int cmdsize = 0;

     while (buffer[cmdsize] != '\n')
      {
       cmd[cmdsize] = buffer[cmdsize];
       cmdsize++;
      }
     cmd[cmdsize] = 0;

     for (int i = 0; i < BSIZE - cmdsize - 1; i++)
      {
       buffer[i] = buffer[i + cmdsize + 1];
      }
     bp -= cmdsize - n + 1;

     if (cmd[cmdsize - 1] == '\r')
      {
       cmd[cmdsize - 1] = 0; //strip \r  
      }

     strcpy (cmd, (const char *) lowercase (cmd).c_str ());


     dprint ("cmd[%s]\n", cmd);

     switch (cmd[0])
      {
      case 'e':
       if (!strcmp (cmd, "exit"))
        {
         //Command exit ========================================================
         sendtext ("Ok\r\n");
         Window1.SetToDestroy ();
         return 0;
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'g':
       if (!strncmp (cmd, "get ", 4))
        {
         //Command get==========================================================
         char * ptr;
         char * ptr2;
         Board = Window1.GetBoard ();

         if ((ptr = strstr (cmd, " board.in[")))
          {
           int in = (ptr[10] - '0')*10 + (ptr[11] - '0');

           if (in < Board->GetInputCount ())
            {
             Input = Board->GetInput (in);

             if (Input->status != NULL)
              {
               snprintf (lstemp, 100, "board.in[%02i]", in);
               ProcessInput (lstemp, Input, &ret);
               sendtext ("Ok\r\n>");
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\r\n>");

            }
          }
         else if ((ptr = strstr (cmd, " board.out[")))
          {
           int out = (ptr[11] - '0')*10 + (ptr[12] - '0');

           if (out < Board->GetOutputCount ())
            {
             Output = Board->GetOutput (out);

             if (Output->status != NULL)
              {
               snprintf (lstemp, 100, "board.out[%02i]", out);
               ProcessOutput (lstemp, Output, &ret);
               sendtext ("Ok\r\n>");
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\r\n>");

            }
          }
         else if ((ptr = strstr (cmd, " apin[")))
          {
           int pin = (ptr[6] - '0')*10 + (ptr[7] - '0');
           if (Board->GetUseSpareParts ())
            {
             pins = Window5.GetPinsValues ();
            }
           else
            {
             Board = Window1.GetBoard ();
             pins = Board->MGetPinsValues ();
            }
           snprintf (lstemp, 100, "apin[%02i]= %i \r\nOk\r\n>", pin, (int) pins[pin - 1].oavalue - 55);
           sendtext (lstemp);
          }
         else if ((ptr = strstr (cmd, " pin[")))
          {
           int pin = (ptr[5] - '0')*10 + (ptr[6] - '0');
           if (Board->GetUseSpareParts ())
            {
             pins = Window5.GetPinsValues ();
            }
           else
            {
             Board = Window1.GetBoard ();
             pins = Board->MGetPinsValues ();
            }
           snprintf (lstemp, 100, "pin[%02i]= %i \r\nOk\r\n>", pin, pins[pin - 1].value);
           sendtext (lstemp);
          }
         else if (Board->GetUseSpareParts ())
          {
           if ((ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].in[")))
            {
             int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
             int in = (ptr2[5] - '0')*10 + (ptr2[6] - '0');

             if (pn < Window5.GetPartsCount ())
              {
               Part = Window5.GetPart (pn);
               if (in < Part->GetInputCount ())
                {
                 Input = Part->GetInput (in);

                 if (Input->status != NULL)
                  {
                   snprintf (lstemp, 100, "part[%02i].in[%02i]", pn, in);
                   ProcessInput (lstemp, Input, &ret);
                   sendtext ("Ok\r\n>");
                  }
                 else
                  {
                   ret = sendtext ("ERROR\r\n>");
                  }
                }
               else
                {
                 ret = sendtext ("ERROR\r\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");

              }
            }
           else if ((ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].out[")))
            {
             int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
             int out = (ptr2[6] - '0')*10 + (ptr2[7] - '0');

             if (pn < Window5.GetPartsCount ())
              {
               Part = Window5.GetPart (pn);
               if (out < Part->GetOutputCount ())
                {
                 Output = Part->GetOutput (out);

                 if (Output->status != NULL)
                  {
                   snprintf (lstemp, 100, "part[%02i].out[%02i]", pn, out);
                   ProcessOutput (lstemp, Output, &ret);
                   sendtext ("Ok\r\n>");
                  }
                 else
                  {
                   ret = sendtext ("ERROR\r\n>");
                  }
                }
               else
                {
                 ret = sendtext ("ERROR\r\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\r\n>");
            }
          }
         else
          {
           ret = sendtext ("ERROR\r\n>");
          }
         return 0;
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'h':
       if (!strcmp (cmd, "help"))
        {
         //Command help ========================================================
         ret += sendtext ("List of supported commands:\r\n");
         ret += sendtext ("  exit      - shutdown PICSimLab\r\n");
         ret += sendtext ("  get ob    - get object value\r\n");
         ret += sendtext ("  help      - show this message\r\n");
         ret += sendtext ("  info      - show actual setup info and objects\r\n");
         ret += sendtext ("  pins      - show pins directions and values\r\n");
         ret += sendtext ("  pinsl     - show pins formated info\r\n");
         ret += sendtext ("  quit      - exit remote control interface\r\n");
         ret += sendtext ("  reset     - reset the board\r\n");
         ret += sendtext ("  set ob vl - set object with value\r\n");
         ret += sendtext ("  sync      - wait to syncronize with timer event\r\n");
         ret += sendtext ("  version   - show PICSimLab version\r\n");

         ret += sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'i':
       if (!strcmp (cmd, "info"))
        {
         //Command info ========================================================
         Board = Window1.GetBoard ();
         stemp.Printf ("Board:     %s\r\n", Board->GetName ().c_str ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Processor: %s\r\n", Board->GetProcessorName ().c_str ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Frequency: %10.0f Hz\r\n", Board->MGetFreq ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Use Spare: %i\r\n", Board->GetUseSpareParts ());
         ret += sendtext ((const char *) stemp.c_str ());

         for (i = 0; i < Board->GetInputCount (); i++)
          {
           Input = Board->GetInput (i);
           if ((Input->status != NULL))
            {
             snprintf (lstemp, 100, "    board.in[%02i]", i);
             ProcessInput (lstemp, Input, &ret);
            }
          }

         for (i = 0; i < Board->GetOutputCount (); i++)
          {
           Output = Board->GetOutput (i);
           if (Output->status != NULL)
            {
             snprintf (lstemp, 100, "    board.out[%02i]", i);
             ProcessOutput (lstemp, Output, &ret);
            }
          }

         if (Board->GetUseSpareParts ())
          {
           for (i = 0; i < Window5.GetPartsCount (); i++)
            {
             Part = Window5.GetPart (i);
             stemp.Printf ("  part[%02i]: %s\r\n", i, (const char *) Part->GetName ());
             ret += sendtext ((const char *) stemp.c_str ());

             for (j = 0; j < Part->GetInputCount (); j++)
              {
               Input = Part->GetInput (j);
               if (Input->status != NULL)
                {
                 snprintf (lstemp, 100, "    part[%02i].in[%02i]", i, j);
                 ProcessInput (lstemp, Input, &ret);
                }
              }
             for (j = 0; j < Part->GetOutputCount (); j++)
              {
               Output = Part->GetOutput (j);
               if (Output->status != NULL)
                {
                 snprintf (lstemp, 100, "    part[%02i].out[%02i]", i, j);
                 ProcessOutput (lstemp, Output, &ret);
                }
              }
            }
          }
         ret += sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'p':
       if (!strcmp (cmd, "pins"))
        {
         //Command pins ========================================================
         Board = Window1.GetBoard ();
         pins = Board->MGetPinsValues ();
         int p2 = Board->MGetPinCount () / 2;
         for (i = 0; i < p2; i++)
          {
           snprintf (lstemp, 100, "  pin[%02i] (%8s) %c %i                 pin[%02i] (%8s) %c %i \r\n",
                     i + 1, (const char *) Board->MGetPinName (i + 1).c_str (), (pins[i].dir == PD_IN) ? '<' : '>', pins[i].value,
                     i + 1 + p2, (const char *) Board->MGetPinName (i + 1 + p2).c_str (), (pins[i + p2].dir == PD_IN) ? '<' : '>', pins[i + p2].value);
           ret += sendtext (lstemp);
          }
         ret += sendtext ("Ok\r\n>");
        }
       else if (!strcmp (cmd, "pinsl"))
        {
         //Command pinsl ========================================================
         Board = Window1.GetBoard ();
         pins = Board->MGetPinsValues ();
         for (i = 0; i < Board->MGetPinCount (); i++)
          {
           snprintf (lstemp, 100, "  pin[%02i] %c %c %i %03i %5.3f \"%-8s\" \r\n",
                     i + 1,
                     pintypetoletter (pins[i].ptype),
                     (pins[i].dir == PD_IN) ? 'I' : 'O',
                     pins[i].value,
                     (int) (pins[i].oavalue - 55),
                     pins[i].avalue,
                     (const char *) Board->MGetPinName (i + 1).c_str ()
                     );
           ret += sendtext (lstemp);
          }
         ret += sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'q':
       if (!strcmp (cmd, "quit"))
        {
         //Command quit ========================================================
         sendtext ("Ok\r\n");
         ret = 1;
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'r':
       if (!strcmp (cmd, "reset"))
        {
         //Command reset =======================================================
         Window1.GetBoard ()->MReset (0);
         ret = sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 's':
       if (!strncmp (cmd, "set ", 4))
        {
         //Command set =========================================================
         char * ptr;
         char * ptr2;
         Board = Window1.GetBoard ();

         if ((ptr = strstr (cmd, " board.in[")))
          {
           int in = (ptr[10] - '0')*10 + (ptr[11] - '0');
           int value;

           sscanf (ptr + 13, "%i", &value);

           dprint ("board.in[%02i] = %i \r\n", in, value);

           if (in < Board->GetInputCount ())
            {
             Input = Board->GetInput (in);

             if (Input->status != NULL)
              {
               *((unsigned char *) Input->status) = value;
               sendtext ("Ok\r\n>");
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\r\n>");

            }
          }
         else if ((ptr = strstr (cmd, " apin[")))
          {
           int pin = (ptr[6] - '0')*10 + (ptr[7] - '0');
           int value;

           sscanf (ptr + 9, "%i", &value);

           dprint ("apin[%02i] = %i \r\n", pin, value);

           if (Board->GetUseSpareParts ())
            {
             Window5.SetAPin (pin, value / 40.0);
            }
           else
            {
             Board = Window1.GetBoard ();
             Board->MSetAPin (pin, value / 40.0);
            }
           sendtext ("Ok\r\n>");
          }
         else if ((ptr = strstr (cmd, " pin[")))
          {
           int pin = (ptr[5] - '0')*10 + (ptr[6] - '0');
           int value;

           sscanf (ptr + 8, "%i", &value);

           dprint ("pin[%02i] = %i \r\n", pin, value);

           if (Board->GetUseSpareParts ())
            {
             Window5.SetPin (pin, value);
            }
           else
            {
             Board = Window1.GetBoard ();
             Board->MSetPin (pin, value);
            }
           sendtext ("Ok\r\n>");
          }
         else if (Board->GetUseSpareParts ()&&(ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].in[")))
          {
           int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
           int in = (ptr2[5] - '0')*10 + (ptr2[6] - '0');
           int value;

           sscanf (ptr2 + 8, "%i", &value);

           dprint ("part[%02i].in[%02i] = %i \r\n", pn, in, value);

           if (pn < Window5.GetPartsCount ())
            {
             Part = Window5.GetPart (pn);

             if (in < Part->GetInputCount ())
              {
               Input = Part->GetInput (in);

               if (Input->status != NULL)
                {
                 if ((Input->name[0] == 'V')&&(Input->name[1] == 'S'))
                  {
                   *((unsigned char *) Input->status) = (value & 0xFF00) >> 8;
                   *(((unsigned char *) Input->status) + 1) = value & 0x00FF;
                  }
                 else
                  {
                   *((unsigned char *) Input->status) = value;
                  }
                 if (Input->update)
                  {
                   *Input->update = 1;
                  }
                 sendtext ("Ok\r\n>");
                }
               else
                {
                 ret = sendtext ("ERROR\r\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\r\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\r\n>");

            }
          }
         else
          {
           ret = sendtext ("ERROR\r\n>");
          }
         return 0;
        }
       if (!strcmp (cmd, "sync"))
        {
         //Command sync =====================================================
         Window1.SetSync (0);
         while (!Window1.GetSync ())
          {
           usleep (1); //FIXME avoid use of usleep to reduce cpu usage
          }
         ret = sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      case 'v':
       if (!strcmp (cmd, "version"))
        {
         //Command version =====================================================
         stemp.Printf (lxT ("Developed by L.C. Gamboa\r\n <lcgamboa@yahoo.com>\r\n Version: %s %s %s\r\n"), lxT (_VERSION_), lxT (_DATE_), lxT (_ARCH_));
         ret += sendtext ((const char *) stemp.c_str ());
         ret += sendtext ("Ok\r\n>");
        }
       else
        {
         ret = sendtext ("ERROR\r\n>");
        }
       break;
      default:
       //Uknown command ========================================================
       ret = sendtext ("ERROR\r\n>");
       break;
      }
    }
   else
    {
     bp += n;
     if (bp > BSIZE)bp = BSIZE;
    }
  }
 else
  {
   //socket close by client
   if (n == 0)ret = 1;
  }

 //close connection
 if (ret)rcontrol_stop ();

 return ret;
}

