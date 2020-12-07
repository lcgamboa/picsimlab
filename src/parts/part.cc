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
#include "../picsimlab5.h"
#include "part.h"

part::part ()
{
  inputc = 0;
  outputc = 0;
  orientation = 0;
}

void
part::ReadMaps (void)
{
  inputc = 0;
  outputc = 0;
  ReadInputMap (Window1.GetSharePath () + lxT ("parts/") + GetInputMapFile ());
  ReadOutputMap (Window1.GetSharePath () + lxT ("parts/") + GetOutputMapFile ());
}

void
part::ReadInputMap (lxString fname)
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

};

void
part::ReadOutputMap (lxString fname)
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

};

int
part::PointInside (int x, int y)
{

  switch (orientation)
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
part::PointInside (int x, int y, input_t input)
{
  int temp;
  int X1=0, X2=0;
  int Y1=0, Y2=0;

  switch (orientation)
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
part::GetOrientation (void)
{

  return orientation;
}

void
part::SetOrientation (int _orientation)
{

  if (orientation == _orientation)return;

  if (!Bitmap)return;

  orientation = _orientation;

  delete Bitmap;

  lxImage image;

  image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());

  Bitmap = lxGetBitmapRotated (&image, &Window5, orientation);
  image.Destroy ();
  canvas.Destroy ();
  canvas.Create (Window5.GetWWidget (), Bitmap);

}
