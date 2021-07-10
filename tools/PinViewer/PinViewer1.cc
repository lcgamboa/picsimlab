#include"PinViewer1.h"
#include"PinViewer1_d.cc"


#ifdef _WIN_
WORD wVersionRequested = 2;
WSADATA wsaData;

__attribute__ ((constructor))
static void
initialize_socket(void)
{
 WSAStartup (wVersionRequested, &wsaData);
 if (wsaData.wVersion != wVersionRequested)
  {
   fprintf (stderr, "\n Wrong version\n");
   return;
  }
}

__attribute__ ((destructor))
static void
finalize_socket(void)
{
 WSACleanup ();
}
#endif

CPWindow1 Window1;

//Implementation

void
CPWindow1::_EvOnCreate(CControl * control)
{
 font = new lxFont (10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
}

void
CPWindow1::_EvOnShow(CControl * control)
{
 need_resize = 0;
 timer1.SetRunState (1);
}

void
CPWindow1::_EvOnHide(CControl * control)
{
 timer1.SetRunState (0);
 if (connected)
  {
   send_cmd ("quit");
  }
}

void
CPWindow1::_EvOnDestroy(CControl * control)
{
 delete font;
}

int
CPWindow1::PointInside(int i, int x_, int y_)
{

 int X = 0;
 int Y = 0;
 int x = 0;
 int y = 0;
 int Width = 52;
 int Height = 8;

 if (pincount <= 8)
  {
   x = 0;
   y = i * 15 + 20;
  }
 else if (pincount <= 40)
  {
   if (i < pincount / 2)
    {
     x = 0;
     y = i * 15 + 20;
    }
   else
    {
     x = 300;
     y = (i - (pincount / 2)) * 15 + 20;
    }
  }
 else //pincount > 40 
  {
   if (i < pincount / 4)
    {
     x = 0;
     y = i * 15 + 20;
    }
   else if (i < pincount / 2)
    {
     x = 300;
     y = (i - (pincount / 4)) * 15 + 20;
    }
   else if (i < pincount * 3 / 4)
    {
     x = 600;
     y = (i - (pincount * 2 / 4)) * 15 + 20;
    }
   else
    {
     y = (i - (pincount * 3 / 4)) * 15 + 20;
     x = 900;
    }
  }

 //button offset 
 X = 185 + x;
 Y = y;

 if (((x_ >= X)&&(x_ <= (X + Width)))&&((y_ >= Y)&&(y_ <= (Y + Height))))
  {
   return 1;
  }

 X = 98 + x;
 Y = y;
 Width = 20;
 Height = 8;

 if (((x_ >= X)&&(x_ <= (X + Width)))&&((y_ >= Y)&&(y_ <= (Y + Height))))
  {
   return 2;
  }

 return 0;
}

void
CPWindow1::draw1_EvMouseButtonPress(CControl * control, const uint button, const uint x, const uint y, const uint state)
{
 float fx = x / scale;
 float fy = y / scale;
 lxString temp;

 for (int i = 0; i < pincount; i++)
  {
   if (PointInside (i, (int) (fx - offsetx), (int) (fy - offsety)) == 1)
    {

     if ((pins[i].type == 'D') && (pins[i].dir == 'I'))
      {
       //printf ("botão pino %i\n", i + 1);



       if (pins[i].dvalue == '0')
        {
         temp.Printf ("set pin[%02i] 1", i + 1);
        }
       else
        {
         temp.Printf ("set pin[%02i] 0", i + 1);
        }
       send_cmd (temp);
       //printf(buff);
       return;
      }
     else if (pins[i].type == 'A')
      {
       lxString temp;
       int x = 0;
       if (pincount <= 40)
        {
         if (i < pincount / 2)
          {
           x = 0;
          }
         else
          {
           x = 300;
          }
        }
       else //pincount > 40 
        {
         if (i < pincount / 4)
          {
           x = 0;
          }
         else if (i < pincount / 2)
          {
           x = 300;
          }
         else if (i < pincount * 3 / 4)
          {
           x = 600;
          }
         else
          {
           x = 900;
          }
        }
       float val = (fx - offsetx - 185 - x) / 10;
       if (val > 5.0)val = 5.0;
       temp.Printf ("set apin[%02i] %f", i + 1, val);
       send_cmd (temp);
       //printf(buff);
       return;
      }

     return;
    }
   if (PointInside (i, (int) (fx - offsetx), (int) (fy - offsety)) == 2)
    {
     if ((pins[i].type == 'D') && (pins[i].dir == 'I'))
      {
       temp.Printf ("set apin[%02i] 2.500", i + 1);
       send_cmd (temp);
      }
     else if (pins[i].type == 'A')
      {
       temp.Printf ("set pin[%02i] 0", i + 1);
       send_cmd (temp);
      }
    }
  }

 if (button == 1)
  {
   mouse_scroll = 1;
   lxSetCursor (lxCursor (lxCURSOR_SIZENWSE));
   mdx = fx;
   mdy = fy;
  }
 else if (button == 3)
  {
   offsetx = 0;
   offsety = 0;
  }
}

void
CPWindow1::draw1_EvMouseButtonRelease(CControl * control, const uint button, const uint x, const uint y, const uint state)
{
 //float  fx = x / scale;
 //float  fy = y / scale;

 mouse_scroll = 0;
 lxSetCursor (lxCursor (lxCURSOR_ARROW));
 mdx = 0;
 mdy = 0;
}

void
CPWindow1::draw1_EvMouseMove(CControl * control, const uint button, const uint x, const uint y, const uint state)
{
 float fx = x / scale;
 float fy = y / scale;

 if (mouse_scroll)
  {
   offsetx -= mdx - fx;
   offsety -= mdy - fy;
   mdx = fx;
   mdy = fy;
  }
}

void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
 WDestroy ();
}

void
CPWindow1::menu1_Help_Contents_EvMenuActive(CControl * control)
{
 lxString stemp;
 stemp.Printf (lxT ("https://lcgamboa.github.io/picsimlab_docs/%s/PinViewer.html"), lxT (_VERSION_));
 lxLaunchDefaultBrowser (stemp);
}

void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
 lxString stemp;
 stemp.Printf (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s"), lxT (_VERSION_), lxT (_DATE_), lxT (_ARCH_));
 Message_sz (stemp, 400, 200);
}

void
CPWindow1::timer1_EvOnTime(CControl * control)
{
 if (need_resize == 1)
  {
   draw1.SetWidth (Width - 15);
   //draw1.SetHeight (Height - 40);
#ifdef _WIN_
   draw1.SetHeight (Height - 75);
#else
   draw1.SetHeight (Height - 90);
#endif
  }

 need_resize++;

 if (!connected)
  {
   if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("socket error : %s \n", strerror (errno));
     exit (1);
    }
   memset (&servaddr, 0, sizeof (servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
   servaddr.sin_port = htons (5000);

   //setnblock (sockfd);
   if (connect (sockfd, (sockaddr *) & servaddr, sizeof (servaddr)) < 0)
    {
#ifdef _WIN_
     printf ("connect error number: %i \n", WSAGetLastError ());
#else         
     printf ("connect error : %s \n", strerror (errno));
#endif
     close (sockfd);
    }
   else
    {
     recv (sockfd, buff, 200, 0);
     //printf ("%s", buff);

     connected = 1;
    }
  }

 draw1.Canvas.SetFont (*font);

 draw1.Canvas.Init (1.0, 1.0);
 draw1.Canvas.SetFgColor (50, 50, 50);
 draw1.Canvas.SetBgColor (50, 50, 50);
 draw1.Canvas.Rectangle (1, 0, 0, draw1.GetWidth (), draw1.GetHeight ());
 if (connected)
  {
   char * str;
   char * type;
   char * dir;
   char * dvalue;
   char * mvalue;
   char * avalue;
   char * name;
   if (send_cmd ("pinsl") < 0)return;


   str = strtok (buff, "\r\n");
   int i = 0;
   int pinsc;


   sscanf (str, "%i", &pinsc);
   if (pinsc != pincount)
    {
     pincount = pinsc;
     int x = GetX ();
     int y = GetY ();
     SetX (x); //update position
     SetY (y);

     if (pincount <= 8)
      {
       SetHeight (pincount * 15 + 130);
       SetWidth (280);
      }
     else if (pincount <= 40)
      {
       SetHeight ((pincount / 2) * 15 + 130);
       SetWidth (580);
      }
     else
      {
       SetHeight ((pincount / 4) * 15 + 130);
       SetWidth (1170);
      }
     scale = 1.0;
     offsetx = 0;
     offsety = 0;
    }

   draw1.Canvas.ChangeScale (scale, scale);

   while (pinsc)
    {
     int y = 0;
     int x = 0;
     int mv;
     float av;

     if (pincount <= 8)
      {
       x = 0;
       y = i * 15 + 20;
      }
     else if (pincount <= 40)
      {
       if (i < pincount / 2)
        {
         x = 0;
         y = i * 15 + 20;
        }
       else
        {
         x = 300;
         y = (i - (pincount / 2)) * 15 + 20;
        }
      }
     else //pincount > 40 
      {
       if (i < pincount / 4)
        {
         x = 0;
         y = i * 15 + 20;
        }
       else if (i < pincount / 2)
        {
         x = 300;
         y = (i - (pincount / 4)) * 15 + 20;
        }
       else if (i < pincount * 3 / 4)
        {
         x = 600;
         y = (i - (pincount * 2 / 4)) * 15 + 20;
        }
       else
        {
         y = (i - (pincount * 3 / 4)) * 15 + 20;
         x = 900;
        }
      }

     strtok (NULL, " \"\r\n");
     type = strtok (NULL, " \"\r\n");
     dir = strtok (NULL, " \"\r\n");
     dvalue = strtok (NULL, " \"\r\n");
     mvalue = strtok (NULL, " \"\r\n");
     avalue = strtok (NULL, " \"\r\n");
     name = strtok (NULL, " \"\r\n");

     pins[i].type = type[0];
     pins[i].dir = dir[0];
     pins[i].dvalue = dvalue[0];

     switch (type[0])
      {
      case 'D':
       draw1.Canvas.SetFgColor (0, 0, 0);
       draw1.Canvas.SetBgColor (0, 0, 100);
       draw1.Canvas.Rectangle (1, 8 + offsetx + x, y - 1 + offsety, 238, 14);
       sprintf (line, "%2i %-8s %s %s %s ", i + 1, name, type, dir, dvalue);

       if (dir[0] == 'I')
        {
         draw1.Canvas.SetColor (0x6a, 0x2c, 0x29);
        }
       else
        {
         draw1.Canvas.SetColor (0x66, 0x65, 0x28);
        }
       draw1.Canvas.Rectangle (1, 98 + offsetx + x, y + 1 + offsety, 142, 10);

       if (dvalue[0] == '0')
        {
         draw1.Canvas.SetColor (200, 0, 0);
         ;
        }
       else
        {
         draw1.Canvas.SetColor (0, 0, 200);
        }
       draw1.Canvas.Rectangle (1, 134 + offsetx + x, y + 2 + offsety, 30, 8);

       draw1.Canvas.SetFgColor (255, 255, 255);
       draw1.Canvas.Text (line, 10 + offsetx + x, y - 1 + offsety);

       //togle and mean
       if (dir[0] == 'I')
        {
         draw1.Canvas.SetColor (200, 100, 30);
         draw1.Canvas.Rectangle (1, 185 + offsetx + x, y + 2 + offsety, 52, 8);
         draw1.Canvas.SetFgColor (255, 255, 255);
         draw1.Canvas.Text ("Toggle", 187 + offsetx + x, y - 1 + offsety);
        }
       else
        {
         sscanf (mvalue, "%3d", &mv);
         draw1.Canvas.SetColor (200, 200, 200);
         draw1.Canvas.Rectangle (1, 185 + offsetx + x, y + 2 + offsety, 52, 8);
         draw1.Canvas.SetColor (0, 0, 200);
         draw1.Canvas.Rectangle (1, 186 + offsetx + x, y + 3 + offsety, mv / 4, 6);
        }

       break;
      case 'A':
       sprintf (line, "%2i %-8s %s %s %s", i + 1, name, type, dir, avalue);
       draw1.Canvas.SetFgColor (0, 0, 0);
       draw1.Canvas.SetBgColor (0, 100, 0);
       draw1.Canvas.Rectangle (1, 8 + offsetx + x, y - 1 + offsety, 238, 14);
       draw1.Canvas.SetFgColor (255, 255, 255);
       draw1.Canvas.Text (line, 10 + offsetx + x, y - 1 + offsety);

       //draw gauge 
       sscanf (avalue, "%f", &av);
       mv = av * 10;
       draw1.Canvas.SetColor (200, 200, 200);
       draw1.Canvas.Rectangle (1, 185 + offsetx + x, y + 2 + offsety, 52, 8);
       draw1.Canvas.SetColor (255, 255, 0);
       draw1.Canvas.Rectangle (1, 186 + offsetx + x, y + 3 + offsety, mv, 6);
       pins[i].avalue = av;
       break;

      default:
       draw1.Canvas.SetFgColor (0, 0, 0);
       draw1.Canvas.SetBgColor (100, 100, 100);
       draw1.Canvas.Rectangle (1, 8 + offsetx + x, y - 1 + offsety, 238, 14);
       sprintf (line, "%2i %-8s %s", i + 1, name, type);
       draw1.Canvas.SetFgColor (255, 255, 255);
       draw1.Canvas.Text (line, 10 + offsetx + x, y - 1 + offsety);
       break;
      }

     pinsc--;
     i++;
    }

  }

 draw1.Canvas.End ();
 draw1.Update ();

 if (connected)
  {
   statusbar1.SetField (0, "Connected");
  }
 else
  {
   statusbar1.SetField (0, "Not connected");
  }

 lxString field;

 field.Printf ("Scale: %3.1f", scale);
 statusbar1.SetField (1, field);

 field.Printf ("Offset: %3i %3i", offsetx, offsety);
 statusbar1.SetField (2, field);

}

void
CPWindow1::draw1_EvMouseWheel(CControl * control, const int rotation)
{
 if (rotation > 0)
  {
   scale += 0.1;
   if (scale > 2)scale = 2;

   scale = trunc (scale * 10) / 10.0;
  }
 else
  {
   scale -= 0.1;
   if (scale < 0.8)scale = 0.8;

   scale = trunc (scale * 10) / 10.0;
  }
}

int
CPWindow1::send_cmd(const char * message)
{
 //printf ("sending '%s'\n", message);
 int n = strlen (message);
 if (send (sockfd, message, n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   connected = 0;
   close (sockfd);
   return -1;
  }
 n = 2;
 if (send (sockfd, "\r\n", n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   connected = 0;
   close (sockfd);
   return -1;
  }

 int bp = 0;
 buff[0] = 0;
 do
  {

   if ((n = recv (sockfd, buff + bp, 1, 0)) > 0)
    {
     bp += n;
     buff[bp] = 0;
     //printf ("%c", buff[bp-1]);
    }
   else
    {
     printf ("recv error : %s \n", strerror (errno));
     connected = 0;
     close (sockfd);
     return -1;
    }

  }
 while (((bp < 5) || (strcmp (&buff[bp - 5], "Ok\r\n>"))) && ((bp < 8) || strcmp (&buff[bp - 8], "ERROR\r\n>")));

 return bp;
}
