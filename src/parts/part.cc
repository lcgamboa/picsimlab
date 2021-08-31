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
#include "../picsimlab5.h"
#include "part.h"

part::part()
{
 aways_update = 0;
 inputc = 0;
 outputc = 0;
 Orientation = 0;
 Scale = 1.0;
 Update = 1;
}

void
part::ReadMaps(void)
{
 inputc = 0;
 outputc = 0;
 ReadInputMap (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetMapFile ()));
 ReadOutputMap (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetMapFile ()));

 for (int i = 0; i < inputc; i++)
  {
   input_ids[get_in_id (input[i].name)] = &input[i];
  }

 for (int i = 0; i < outputc; i++)
  {
   output_ids[get_out_id (output[i].name)] = &output[i];
  }
}

void
part::ReadInputMap(lxString fname)
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
           Width = x1;
           //Window1.SetplWidth(x1);
           //Window1.draw1.SetWidth(x1*Window1.GetScale());
           //Window1.SetWidth(185+x1*Window1.GetScale());
          }

         if (!strcmp ("height", name))
          {
           sscanf (value, "%i", &y1);
           Height = y1;
           //Window1.SetplHeight(y1);
           //Window1.draw1.SetHeight(y1*Window1.GetScale());
           /*
           #ifdef _WIN_
                       Window1.SetHeight(75+y1*Window1.GetScale());
           #else
                       Window1.SetHeight(90+y1*Window1.GetScale());
           #endif
            */
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

       if (((name[0] == 'I') || (name[0] == 'B'))&&(name[1] == '_'))
        {
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
         strcpy (input[inputc].name, name + 2);
         input[inputc].id = get_in_id (input[inputc].name);
         input[inputc].cx = ((input[inputc].x2 - input[inputc].x1) / 2.0) + input[inputc].x1;
         input[inputc].cy = ((input[inputc].y2 - input[inputc].y1) / 2.0) + input[inputc].y1;
         input[inputc].status = NULL;
         input[inputc].update = NULL;
         input[inputc].value = 0;
         input[inputc].value_s = 0;
         input[inputc].value_f = 0;
         inputc++;
        }
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
part::ReadOutputMap(lxString fname)
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
       if (((name[0] == 'O') || (name[0] == 'B'))&&(name[1] == '_'))
        {
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
           strcpy (output[outputc].name, name + 2);
           output[outputc].id = get_out_id (output[outputc].name);
           output[outputc].cx = ((output[outputc].x2 - output[outputc].x1) / 2.0) + output[outputc].x1;
           output[outputc].cy = ((output[outputc].y2 - output[outputc].y1) / 2.0) + output[outputc].y1;
           output[outputc].status = NULL;
           output[outputc].value = 0;
           output[outputc].value_s = 0;
           output[outputc].value_f = 0;
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
           //          output[outputc].lval=-1;
           strcpy (output[outputc].name, name + 2);
           output[outputc].id = get_out_id (output[outputc].name);
           output[outputc].cx = output[outputc].x1;
           output[outputc].cy = output[outputc].y1;
           output[outputc].status = NULL;
           output[outputc].value = 0;
           output[outputc].value_s = 0;
           output[outputc].value_f = 0;
           outputc++;
          }
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

int
part::PointInside(int x, int y)
{

 switch (Orientation)
  {
  case 0:
  case 2:
   if (((x >= X)&&(x <= (X + (int) Width)))&&((y >= Y)&&(y <= (Y + (int) Height))))
    return 1;
   else
    return 0;
   break;
  case 1:
  case 3:
   if (((x >= X)&&(x <= (X + (int) Height)))&&((y >= Y)&&(y <= (Y + (int) Width))))
    return 1;
   else
    return 0;
   break;
  }
 return 0;
}

int
part::PointInside(int x, int y, input_t input)
{
 int temp;
 int X1 = 0, X2 = 0;
 int Y1 = 0, Y2 = 0;

 switch (Orientation)
  {
  case 0:
   X1 = input.x1;
   Y1 = input.y1;
   X2 = input.x2;
   Y2 = input.y2;
   break;
  case 1:
   X1 = Height - input.y1;
   Y1 = input.x1;
   X2 = Height - input.y2;
   Y2 = input.x2;
   break;
  case 2:
   X1 = Width - input.x1;
   Y1 = Height - input.y1;
   X2 = Width - input.x2;
   Y2 = Height - input.y2;
   break;
  case 3:
   X1 = input.y1;
   Y1 = Width - input.x1;
   X2 = input.y2;
   Y2 = Width - input.x2;
   break;
  default:
   break;
  }

 if (X1 > X2)
  {
   temp = X1;
   X1 = X2;
   X2 = temp;
  }
 if (Y1 > Y2)
  {
   temp = Y1;
   Y1 = Y2;
   Y2 = temp;
  }

 if (((x >= X1)&&(x <= X2)) && ((y >= Y1)&&(y <= Y2)))
  return 1;
 else
  return 0;
}

int
part::GetOrientation(void)
{
 return Orientation;
}

void
part::SetOrientation(int orientation)
{

 if (Orientation == orientation)return;

 if (!Bitmap)return;

 Orientation = orientation;

 delete Bitmap;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 for (int i = 0; i < outputc; i++)
  {
   output[i].update = 1;
  }
}

float
part::GetScale(void)
{

 return Scale;
}

void
part::SetScale(double scale)
{

 if (Scale == scale)return;

 if (!Bitmap)return;

 Scale = scale;

 delete Bitmap;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale, 0, &Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 for (int i = 0; i < outputc; i++)
  {
   output[i].update = 1;
  }
}

int
part::GetInputCount(void)
{
 return inputc;
}

input_t *
part::GetInput(int n)
{
 if (n < inputc)
  {
   return &input[n];
  }
 return NULL;
}

int
part::GetOutputCount(void)
{
 return outputc;
}

output_t *
part::GetOutput(int n)
{
 if (n < outputc)
  {
   return &output[n];
  }
 return NULL;
}

void
part::RotateCoords(unsigned int * x, unsigned int *y)
{
 int temp;

 switch (Orientation)
  {
  case 1:
   temp = *x;
   *x = *y;
   *y = Height - temp;
   break;
  case 2:
   *x = Width - *x;
   *y = Height - *y;
   break;
  case 3:
   temp = *y;
   *y = *x;
   *x = Width - temp;
   break;
  }
}

unsigned char
part::GetUpdate(void)
{
 return Update;
}

void
part::SetUpdate(unsigned char upd)
{
 Update = upd;
 for (int i = 0; i < outputc; i++)
  {
   output[i].update = Update;
  }
}

lxString
part::GetPictureFileName(void)
{
 return GetName () + lxT ("/part.svg");
}

lxString
part::GetMapFile(void)
{
 return GetName () + lxT ("/part.map");
}

lxString
part::GetPropertiesWindowFile(void)
{
 return GetName () + lxT ("/part.lxrad");
}

lxString
part::GetHelpURL(void)
{
 char pname[50];
 strncpy (pname, (const char *) GetName ().c_str (), 49);

 char * ptr;
 //remove space from names
 while ((ptr = strchr (pname, ' ')))
  {
   memmove (ptr, ptr + 1, strlen (ptr) + 1);
  }
 //remove ( from names
 while ((ptr = strchr (pname, '(')))
  {
   memmove (ptr, ptr + 1, strlen (ptr) + 1);
  }
 //remove ) from names
 while ((ptr = strchr (pname, ')')))
  {
   memmove (ptr, ptr + 1, strlen (ptr) + 1);
  }
 //remove . from names
 while ((ptr = strchr (pname, '.')))
  {
   memmove (ptr, ptr + 1, strlen (ptr) + 1);
  }
 //remove - from names
 while ((ptr = strchr (pname, '-')))
  {
   memmove (ptr, ptr + 1, strlen (ptr) + 1);
  }

 lxString stemp;
 stemp.Printf (lxT ("%s.html"), pname);

 return stemp;
}

//Draw Functions

void
part::draw_slider(const output_t * output, const unsigned char pos, const lxString val, const lxFont font)
{
 float dy = pos / 1.66;
 canvas.SetFgColor (255, 255, 255);
 canvas.SetBgColor (89, 89, 89);
 canvas.Rectangle (1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1);
 canvas.SetColor (0, 0, 200);
 canvas.Rectangle (1, output->x1 + 1, output->y1 + 1 + dy, output->x2 - output->x1 - 2, output->y2 - output->y1 - 2 - dy);

 canvas.SetFgColor (0, 0, 0);
 canvas.SetBgColor (46, 46, 46);
 canvas.Rectangle (1, output->x1, output->y1 + pos / 1.66, 32, 19);
 canvas.SetColor (250, 250, 250);
 canvas.SetFont (font);
 canvas.RotatedText (val, output->x1 + 1, output->y1 + 5 + pos / 1.66, 0);
}

void
part::draw_potentiometer(const output_t * output, const unsigned char pos, const lxString val, const lxFont font)
{
 canvas.SetColor (179, 179, 179);
 canvas.Rectangle (1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1);
 canvas.SetFgColor (0, 0, 0);
 canvas.SetBgColor (96, 96, 96);
 canvas.Rectangle (1, output->x1 + 9, output->y1 + 9, output->x2 - output->x1 - 18, output->y2 - output->y1 - 18);
 canvas.SetBgColor (46, 46, 46);
 canvas.Rectangle (1, output->x1, output->y1 + (200 - pos) / 1.66, 32, 19);
 canvas.SetColor (250, 250, 250);
 canvas.SetFont (font);
 canvas.RotatedText (val, output->x1 + 4, output->y1 + 5 + (200 - pos) / 1.66, 0);
}

int
part::GetAwaysUpdate(void)
{
 return aways_update;
}