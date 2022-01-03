/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2021  Luis Claudio Gambôa Lopes

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

//include files
#include"../picsimlab1.h"
#include"../picsimlab4.h" //Oscilloscope
#include"../picsimlab5.h" //Spare Parts
#include"exp_board_RemoteTCP.h"

#ifndef _WIN_
#include<netinet/in.h>
#define INVALID_HANDLE_VALUE -1;
#else
#include<winsock2.h>
#include<ws2tcpip.h>
#define MSG_NOSIGNAL 0
#endif

#define dprintf if (1) {} else printf


const char json_info[] = "{"
    "  \"name\": \"PICSimLab\","
    "  \"description\": \"PICSimLab IO\","
    "  \"base address\": 0,"
    "  \"address width\": 2,"
    "  \"symbols\": {"
    "    \"PORTA\": 0,"
    "    \"DIRA\":  2,"
    "    \"PORTB\": 4,"
    "    \"DIRB\":  6,"
    "    \"T0CNT\": 8,"
    "    \"T0STA\": 10,"
    "    \"T0CON\": 12,"
    "    \"T0PR\":  14,"
    "    \"PFREQ\": 16"
    "  }"
    "}";

#define PORTA 0
#define DIRA  2
#define PORTB 4
#define DIRB  6
#define T0CNT 8
#define T0STA 10
#define T0CON 12
#define T0PR  14
#define PFREQ 16

/* ids of inputs of input map*/
enum
{
 I_ICSP, //ICSP connector
 I_PWR, //Power button
 I_RST //Reset button
};

/* ids of outputs of output map*/
enum
{
 O_LPWR, //Power LED
 O_MP, //uController name
 O_RST //Reset button    
};
//return the input ids numbers of names used in input map

unsigned short
cboard_RemoteTCP::get_in_id(char * name)
{
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PB_RST") == 0)return I_RST;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_RemoteTCP::get_out_id(char * name)
{
 if (strcmp (name, "LD_LPWR") == 0)return O_LPWR;
 if (strcmp (name, "IC_CPU") == 0)return O_MP;
 if (strcmp (name, "PB_RST") == 0)return O_RST;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_RemoteTCP::cboard_RemoteTCP(void) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 Proc = "Ripes"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps
 lxImage image (&Window1);
 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("boards/Common/ic48.svg")), 0, Scale, Scale, 1);
 micbmp = new lxBitmap (&image, &Window1);
}

//Destructor called once on board destruction 

cboard_RemoteTCP::~cboard_RemoteTCP(void)
{
 delete micbmp;
 micbmp = NULL;
}

//Reset board status

void
cboard_RemoteTCP::Reset(void)
{
 MReset (1);

 Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE));

 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

int
cboard_RemoteTCP::MInit(const char * processor, const char * fname, float freq)
{
 int ret = 0;

 ret = bsim_remote::MInit (processor, fname, freq);


 lxImage image (&Window1);


 if (!image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg")), 0, Scale, Scale, 1))
  {
   image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("boards/Common/ic6.svg")), 0, Scale, Scale, 1);
   printf ("picsimlab: IC package with %i pins not found!\n", MGetPinCount ());
   printf ("picsimlab: %s not found!\n", (const char *) (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg")).c_str ());
  }

 if (micbmp) delete micbmp;
 micbmp = new lxBitmap (&image, &Window1);

 Window1.Set_mcupwr (0);

 return ret;
}

void
cboard_RemoteTCP::RegisterRemoteControl(void) { }

//Called ever 1s to refresh status

void
cboard_RemoteTCP::RefreshStatus(void)
{
 output_ids[O_LPWR]->update = 1;
 Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE));
}

//Called to save board preferences in configuration file

void
cboard_RemoteTCP::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("RemoteTCP_proc"), Proc);
 //write microcontroller clock to preferences
 Window1.saveprefs (lxT ("RemoteTCP_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

//Called whe configuration file load  preferences 

void
cboard_RemoteTCP::ReadPreferences(char *name, char *value)
{

 //read microcontroller of preferences
 if (!strcmp (name, "RemoteTCP_proc"))
  {
   Proc = value;
  }
 //read microcontroller clock
 if (!strcmp (name, "RemoteTCP_clock"))
  {
   Window1.SetClock (atof (value));
  }
}


//Event on the board

void
cboard_RemoteTCP::EvKeyPress(uint key, uint mask) { }

//Event on the board

void
cboard_RemoteTCP::EvKeyRelease(uint key, uint mask) { }

//Event on the board

void
cboard_RemoteTCP::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 //search for the input area which owner the event    
 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&
                                                  (input[i].y2 >= y)))
    {

     switch (input[i].id)
      {
       //if event is over I_ISCP area then load hex file  
      case I_ICSP:
       Window1.menu1_File_LoadHex_EvMenuActive (NULL);
       break;
       //if event is over I_PWR area then toggle board on/off
       /* 
       case I_PWR:
        if (Window1.Get_mcupwr ()) //if on turn off
         {
          Window1.Set_mcupwr (0);
          Reset ();
         }
        else //if off turn on
         {
          Window1.Set_mcupwr (1);
          Reset ();
         }
        output_ids[O_LPWR]->update = 1;
        break;
        */
       //if event is over I_RST area then turn off and reset
      case I_RST:
       /*
       if (Window1.Get_mcupwr () && reset (-1))//if powered
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
        */
       MReset (-1);
       p_RST = 0;
       output_ids[O_RST]->update = 1;
       break;
      }
    }
  }

}

//Event on the board

void
cboard_RemoteTCP::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 //search for the input area which owner the event  
 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&
                                                  (input[i].y2 >= y)))
    {
     switch (input[i].id)
      {
       //if event is over I_RST area then turn on  
      case I_RST:
       if (Window1.Get_mcurst ())//if powered
        {
         Window1.Set_mcupwr (1);
         Window1.Set_mcurst (0);
         /*
                  if (reset (-1))
                   {
                    Reset ();
                   }
          */
        }
       p_RST = 1;
       output_ids[O_RST]->update = 1;
       break;
      }
    }
  }

}


//Called ever 100ms to draw board
//This is the critical code for simulator running speed

void
cboard_RemoteTCP::Draw(CDraw *draw)
{
 int i;
 lxRect rec;
 lxSize ps;
 int update = 0; //verifiy if updated is needed

 //board_0 draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!update)
      {
       draw->Canvas.Init (Scale, Scale);
      }
     update++; //set to update buffer

     draw->Canvas.SetFgColor (0, 0, 0); //black

     switch (output[i].id)//search for color of output
      {
      case O_LPWR: //Blue using mcupwr value
       draw->Canvas.SetColor (0, 0, 200 * Window1.Get_mcupwr () + 55);
       draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_MP:
       font.SetPointSize (((MGetPinCount () >= 44) || (MGetPinCount () <= 8)) ? 6 : ((MGetPinCount () > 14) ? 12 : 10));
       draw->Canvas.SetFont (font);

       ps = micbmp->GetSize ();
       draw->Canvas.ChangeScale (1.0, 1.0);
       draw->Canvas.PutBitmap (micbmp, output[i].x1*Scale, output[i].y1 * Scale);
       draw->Canvas.ChangeScale (Scale, Scale);
       draw->Canvas.SetFgColor (230, 230, 230);

       rec.x = output[i].x1;
       rec.y = output[i].y1;
       rec.width = ps.GetWidth () / Scale;
       rec.height = ps.GetHeight () / Scale;
       draw->Canvas.TextOnRect (Proc, rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL);
       break;
      case O_RST:
       draw->Canvas.SetColor (100, 100, 100);
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 11);
       if (p_RST)
        {
         draw->Canvas.SetColor (15, 15, 15);
        }
       else
        {
         draw->Canvas.SetColor (55, 55, 55);
        }
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 9);
       break;
      }
    }
  }

 //end draw
 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }
}

void
cboard_RemoteTCP::Run_CPU(void)
{
 int i;
 //int j;
 unsigned char pi;
 unsigned int alm[48];
 const int pinc = MGetPinCount ();

 //const int JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
 const long int NSTEP = 4.0 * Window1.GetNSTEP (); //number of steps in 100ms
 const float RNSTEP = 200.0 * pinc / NSTEP;

 //reset pins mean value
 memset (alm, 0, 48 * sizeof (unsigned int));

 if (!TestConnection ())return;

 Window1.Set_mcupwr (1);

 //Spare parts window pre process
 if (use_spare)Window5.PreProcess ();


 double delay = (Window1.GetIdleMs ()*1000) / NSTEP;

 unsigned int countM = 1100 / delay; //added 100us of overhead
 unsigned int count = countM;

 //printf ("idle %6.1f   %10.5f  %u\n", Window1.GetIdleMs (), delay, count);


 //j = JUMPSTEPS; //step counter
 pi = 0;
 if (Window1.Get_mcupwr ()) //if powered
  for (i = 0; i < NSTEP; i++) //repeat for number of steps in 100ms
   {

    /*
    if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
     {
     }
     */
    //verify if a breakpoint is reached if not run one instruction 
    //MStep ();

    --count;
    if (!count)
     {
#ifdef _WIN_
      Sleep (1);
#else      
      usleep (1000);
#endif      
      count = countM;
     }

    if (t0CON & 0x8000)//Timer on
     {
      t0iclk++; //prescaler clk
      if (t0iclk == (t0CON & 0x7FFF))
       {
        t0iclk = 0;
        t0CNT++;
        if (t0CNT == t0PR) //max value
         {
          t0CNT = 0;
          t0STA |= 1; //overflow
         }
       }
     }

    //Oscilloscope window process
    if (use_oscope)Window4.SetSample ();
    //Spare parts window process
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[pi] += pins[pi].value;
    pi++;
    if (pi == pinc)pi = 0;
    /*
        if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
         {
          j = -1; //reset counter
         }

        j++; //counter increment
     */
   }

 //calculate mean value
 for (pi = 0; pi < MGetPinCount (); pi++)
  {
   pins[pi].oavalue = (int) ((alm[pi] * RNSTEP) + 55);
  }

 //Spare parts window pre post process
 if (use_spare)Window5.PostProcess ();

}

void
cboard_RemoteTCP::MStep(void) {
 /*
 if (DataAvaliable ())
  {

  }
  */ }

void
cboard_RemoteTCP::EvThreadRun(CThread& thread)
{
 do
  {
   cmd_header_t cmd_header;

   if (recv_cmd (&cmd_header) < 0)
    {
     ConnectionError ("recv_cmd");
     return;
    }

   dprintf ("MSG type = %i size=%i ", cmd_header.msg_type, cmd_header.payload_size);

   switch (cmd_header.msg_type)
    {
    case VB_PINFO:
     {
      if (send_cmd (VB_PINFO, json_info, strlen (json_info)) < 0)
       {
        ConnectionError ("send_cmd");
        break;
       }

      dprintf ("VB_PINFO %s\n", json_info);
     }
     break;
    case VB_PWRITE:
     {
      if (cmd_header.payload_size)
       {
        uint32_t * payload = new uint32_t[cmd_header.payload_size / 4];
        if (recv_payload ((char *) payload, cmd_header.payload_size) < 0)
         {
          ConnectionError ("recv_payload");
          break;
         }
        for (uint32_t i = 0; i < (cmd_header.payload_size / 4); i++)
         {
          payload[i] = ntohl (payload[i]);
         }

        switch (payload[0])
         {
         case PORTA:
          Ports[0] = (payload[1] & (~Dirs[0])) | (Ports[0] & Dirs[0]);
          for (int pin = 0; pin < 16; pin++)
           {
            if (Ports[0] & (1 << pins[pin].pord))
             {
              pins[pin].value = 1;
             }
            else
             {
              pins[pin].value = 0;
             }
           }
          break;
         case DIRA:
          Dirs[0] = payload[1];
          for (int pin = 0; pin < 16; pin++)
           {
            if (Dirs[0] & (1 << pins[pin].pord))
             {
              pins[pin].dir = PD_IN;
             }
            else
             {
              pins[pin].dir = PD_OUT;
             }
           }
          break;
         case PORTB:
          Ports[1] = (payload[1] & (~Dirs[1])) | (Ports[1] & Dirs[1]);
          for (int pin = 16; pin < 32; pin++)
           {
            if (Ports[1] & (1 << pins[pin].pord))
             {
              pins[pin].value = 1;
             }
            else
             {
              pins[pin].value = 0;
             }
           }
          break;
         case DIRB:
          Dirs[1] = payload[1];
          for (int pin = 16; pin < 32; pin++)
           {
            if (Dirs[1] & (1 << pins[pin].pord))
             {
              pins[pin].dir = PD_IN;
             }
            else
             {
              pins[pin].dir = PD_OUT;
             }
           }
          break;
         case T0CNT:
          t0CNT = payload[1];
          break;
         case T0CON:
          t0CON = payload[1];
          break;
         case T0STA:
          t0STA = payload[1];
          break;
         case T0PR:
          t0PR = payload[1];
          break;
         }

        dprintf ("VB_PWRITE reg[%i] = %x\n", payload[0], payload[1]);
        delete[] payload;
       }
      if (send_cmd (cmd_header.msg_type) < 0)
       {
        ConnectionError ("send_cmd");
        break;
       }
     }
     break;
    case VB_PREAD:
     {
      uint32_t addr = 0;
      if (cmd_header.payload_size)
       {
        recv_payload ((char *) &addr, 4);
        addr = ntohl (addr);
       }

      uint32_t payload[2];
      payload[0] = htonl (addr);

      switch (addr)
       {
       case PORTA:
        payload[1] = htonl (Ports[0]);
        break;
       case DIRA:
        payload[1] = htonl (Dirs[0]);
        break;
       case PORTB:
        payload[1] = htonl (Ports[1]);
        break;
       case DIRB:
        payload[1] = htonl (Dirs[1]);
        break;
       case T0CNT:
        payload[1] = htonl (t0CNT);
        break;
       case T0CON:
        payload[1] = htonl (t0CON);
        break;
       case T0STA:
        payload[1] = htonl (t0STA);
        break;
       case T0PR:
        payload[1] = htonl (t0PR);
        break;
       case PFREQ:
        payload[1] = htonl (MGetFreq () / 1000000);
        break;
       default:
        payload[1] = htonl (0);
        printf ("Read invalid reg addr %i !!!!!!!!!!!!!!!!!!\n", addr);
        break;
       }

      if (send_cmd (cmd_header.msg_type, (const char *) &payload, 8) < 0)
       {
        ConnectionError ("send_cmd");
        break;
       }
      dprintf ("VB_PREAD  reg[%x] = %x \n", addr, ntohl (payload[1]));
     }
     break;
    case VB_QUIT:
     send_cmd (VB_QUIT);
     Disconnect ();
     break;
    case VB_DMARD:
    case VB_DMAWR:
    case VB_PSTATUS:
     //break;
    default:
     printf ("Invalid cmd !!!!!!!!!!!!\n");
     if (send_cmd (VB_LAST))
      {
       ConnectionError ("send_cmd");
       break;
      }
     break;
    }
  }
 while (!thread.TestDestroy ());

}

//Register the board in PICSimLab
board_init(BOARD_RemoteTCP_Name, cboard_RemoteTCP);
