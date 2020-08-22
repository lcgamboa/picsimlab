/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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
 Proc=""; 
 p_MCLR = 1;
}

board::~board(void) { }

void
board::ReadMaps(void)
{
 inputc = 0;
 outputc = 0;
 ReadInputMap (Window1.GetSharePath () + lxT ("boards/") + GetInputMapFile ());
 ReadOutputMap (Window1.GetSharePath () + lxT ("boards/") + GetOutputMapFile ());
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
           sscanf (value, "%i", &x1);
           Window1.SetplWidth (x1);
           Window1.draw1.SetWidth (x1 * Window1.GetScale ());
           Window1.SetWidth (185 + x1 * Window1.GetScale ());
          }

         if (!strcmp ("height", name))
          {
           sscanf (value, "%i", &y1);
           Window1.SetplHeight (y1);
           Window1.draw1.SetHeight (y1 * Window1.GetScale ());
#ifdef _WIN_
           Window1.SetHeight (75 + y1 * Window1.GetScale ());
#else
           Window1.SetHeight (90 + y1 * Window1.GetScale ());
#endif
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
         //          output[outputc].lval=-1;
         strcpy (output[outputc].name, name);
         output[outputc].id = get_out_id (output[outputc].name);
         outputc++;
        }
       else
        {
         sscanf (coords, "%i,%i,%i\n", &x1, &y1, &r);
         //          printf("circle=%i,%i,%i\n",x1,y1,r);

         output[outputc].x1 = x1;
         output[outputc].y1 = y1;
         output[outputc].r = r;
         //          output[outputc].lval=-1;
         strcpy (output[outputc].name, name);
         output[outputc].id = get_out_id (output[outputc].name);
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
board::SetProcessorName(lxString proc)
{
 Proc = proc;
}

lxString
board::GetProcessorName(void)
{
 return Proc;
}
