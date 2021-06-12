/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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
#include "board.h"

board::board(void)
{
 inputc = 0;
 outputc = 0;
 use_oscope = 0;
 use_spare = 0;
 Proc = "";
 p_RST = 1;
 Scale = Window1.GetScale ();
}

board::~board(void) { }

void
board::ReadMaps(void)
{
 inputc = 0;
 outputc = 0;
 ReadInputMap (Window1.GetSharePath () + lxT ("boards/") + GetInputMapFile ());
 ReadOutputMap (Window1.GetSharePath () + lxT ("boards/") + GetOutputMapFile ());

 for (int i = 0; i < outputc; i++)
  {
   output_ids[get_out_id (output[i].name)] = &output[i];
  }
}

void
board::ReadInputMap(lxString fname)
{
 FILE *fin;

 char line[256];

 char *it;
 char *shape;
 char *coords;
 char *name;
 char *value;

 int board_w = 0;
 int board_h = 0;


 int x1, y1, x2, y2, r;


 fin = fopen (fname.c_str (), "r");

 if (fin)
  {
   while (fgets (line, 256, fin))
    {
     it = strtok (line, "< =\"");
     if (!strcmp ("img", it))
      {
       do
        {
         name = strtok (NULL, "< =\"");
         value = strtok (NULL, "<=\"");

         if (!strcmp ("width", name))
          {
           sscanf (value, "%i", &board_w);
           Window1.SetplWidth (board_w);


           unsigned int ww = 185 + board_w * Window1.GetScale ();
           if (ww > lxGetDisplayWidth (0))
            {
             float scalex = ((lxGetDisplayWidth (0) - 185)*1.0) / board_w;
             Window1.draw1.SetWidth (board_w * scalex);
             Window1.SetWidth (lxGetDisplayWidth (0));
             if (scalex < Scale)
              {
               Scale = scalex;
              }
            }
           else
            {
             Window1.draw1.SetWidth (board_w * Window1.GetScale ());
             Window1.SetWidth (ww);
            }
          }

         if (!strcmp ("height", name))
          {
           sscanf (value, "%i", &board_h);
           Window1.SetplHeight (board_h);

           unsigned int wh = 90 + board_h * Window1.GetScale ();

           if (wh > lxGetDisplayHeight (0))
            {
             float scaley = ((lxGetDisplayHeight (0) - 90)*1.0) / board_h;

             Window1.draw1.SetHeight (board_h * scaley);
             Window1.SetHeight (lxGetDisplayHeight (0));
             Window1.SetWidth (185 + board_w * scaley);
             if (scaley < Scale)
              {
               Scale = scaley;
              }
            }
           else
            {
             Window1.draw1.SetHeight (board_h * Window1.GetScale ());
             Window1.SetHeight (wh);
            }
          }

        }
       while (value != NULL);

       //        printf("%s %s %s\n",name,shape,coords);

      }
     else if (!strcmp ("area", it))
      {
       strtok (NULL, "< =\"");
       shape = strtok (NULL, "< =\"");
       strtok (NULL, "< =\"");
       coords = strtok (NULL, "< =\"");
       strtok (NULL, "< =\"");
       name = strtok (NULL, "< =\"");

       //        printf("%s %s %s\n",name,shape,coords);

       if (strcmp ("rect", shape) == 0)
        {
         sscanf (coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
         //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);

         input[inputc].x1 = x1;
         input[inputc].y1 = y1;
         input[inputc].x2 = x2;
         input[inputc].y2 = y2;
        }
       else
        {
         sscanf (coords, "%i,%i,%i\n", &x1, &y1, &r);
         //          printf("circle=%i,%i,%i\n",x1,y1,r);
         input[inputc].x1 = x1 - r;
         input[inputc].y1 = y1 - r;
         input[inputc].x2 = x1 + r;
         input[inputc].y2 = y1 + r;
        }
       strcpy (input[inputc].name, name);
       input[inputc].id = get_in_id (input[inputc].name);
       input[inputc].cx = ((input[inputc].x2 - input[inputc].x1) / 2.0) + input[inputc].x1;
       input[inputc].cy = ((input[inputc].y2 - input[inputc].y1) / 2.0) + input[inputc].y1;
       input[inputc].status = NULL;
       input[inputc].update = NULL;
       inputc++;
      }
    }
   fclose (fin);
  }
 else
  {
   //Message(lxT("Error open input.map")); Not use this in create!!
   printf ("Error open input.map \"%s\"!\n", (const char*) fname.c_str ());
  }

}

void
board::ReadOutputMap(lxString fname)
{
 FILE *fin;

 char line[256];

 char *it;
 char *shape;
 char *coords;
 char *name;


 int x1, y1, x2, y2, r;

 fin = fopen (fname.c_str (), "r");

 if (fin)
  {
   while (fgets (line, 256, fin))
    {
     it = strtok (line, "< =\"");
     if (!strcmp ("area", it))
      {
       strtok (NULL, "< =\"");
       shape = strtok (NULL, "< =\"");
       strtok (NULL, "< =\"");
       coords = strtok (NULL, "< =\"");
       strtok (NULL, "< =\"");
       name = strtok (NULL, "< =\"");

       //        printf("%s %s %s\n",name,shape,coords);

       if (!strcmp ("rect", shape))
        {
         sscanf (coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
         //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);
         output[outputc].x1 = x1;
         output[outputc].y1 = y1;
         output[outputc].x2 = x2;
         output[outputc].y2 = y2;
         output[outputc].r = 0;
         strcpy (output[outputc].name, name);
         output[outputc].id = get_out_id (output[outputc].name);
         output[outputc].cx = ((output[outputc].x2 - output[outputc].x1) / 2.0) + output[outputc].x1;
         output[outputc].cy = ((output[outputc].y2 - output[outputc].y1) / 2.0) + output[outputc].y1;
         output[outputc].status = NULL;
         outputc++;
        }
       else
        {
         sscanf (coords, "%i,%i,%i\n", &x1, &y1, &r);
         //          printf("circle=%i,%i,%i\n",x1,y1,r);
         output[outputc].x1 = x1;
         output[outputc].y1 = y1;
         output[outputc].x2 = 0;
         output[outputc].y2 = 0;
         output[outputc].r = r;
         strcpy (output[outputc].name, name);
         output[outputc].id = get_out_id (output[outputc].name);
         output[outputc].cx = output[outputc].x1;
         output[outputc].cy = output[outputc].y1;
         output[outputc].status = NULL;
         outputc++;
        }
      }
    }

   fclose (fin);
  }
 else
  {
   //Message(lxT("Error open output.map")); not use this in create!!!
   printf ("Error open output.map \"%s\"!\n", (const char *) fname.c_str ());
  }

}

void
board::RefreshStatus(void)
{
 Window1.statusbar1.SetField (2, lxT (""));
}

void
board::SetUseOscilloscope(int uo)
{
 use_oscope = uo;
}

void
board::SetUseSpareParts(int sp)
{
 use_spare = sp;
}

int
board::GetUseOscilloscope(void)
{
 return use_oscope;
}

int
board::GetUseSpareParts(void)
{
 return use_spare;
}

void
board::SetName(lxString name)
{
 Name = name;
}

lxString
board::GetName(void)
{
 return Name;
}

void
board::SetProcessorName(lxString proc)
{
 Proc = proc;
}

lxString
board::GetProcessorName(void)
{
 return Proc;
}

int
board::GetInputCount(void)
{
 return inputc;
}

input_t *
board::GetInput(int n)
{
 if (n < inputc)
  {
   return &input[n];
  }
 return NULL;
}

int
board::GetOutputCount(void)
{
 return outputc;
}

output_t *
board::GetOutput(int n)
{
 if (n < outputc)
  {
   return &output[n];
  }
 return NULL;
}

unsigned char
board::CalcAngle(int in, int x, int y)
{
 int dx = input[in].cx - x;
 int dy = y - input[in].cy;
 double angle = 0;

 if ((dx >= 0)&&(dy >= 0))
  {
   angle = atan2 (dx, dy) * 180 / M_PI;
  }
 else if ((dx >= 0)&&(dy < 0))
  {
   angle = 180 - (atan2 (dx, -dy) * 180 / M_PI);
  }
 else if ((dx < 0)&&(dy < 0))
  {
   angle = (atan2 (-dx, -dy) * 180 / M_PI) + 180;
  }
 else if ((dx < 0)&&(dy >= 0))
  {
   angle = 360 - (atan2 (-dx, dy) * 180 / M_PI);
  }

 if (angle > 340) angle = 340;
 if (angle < 20) angle = 20;

 return (199 * (angle - 20) / 320.0);
}

void
board::SetScale(double scale)
{
 Scale = scale;
}

double
board::GetScale(void)
{
 return Scale;
}

void
board::EvOnShow(void)
{
 for (int i = 0; i < outputc; i++)
  {
   output[i].update = 1;
  }
}