/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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


//Spare parts

#include"picsimlab1.h"
#include"picsimlab2.h"
#include"picsimlab5.h"
#include"picsimlab5_d.cc"


CPWindow5 Window5;


//Implementation

void
CPWindow5::_EvOnShow(CControl * control)
{
 draw1.SetWidth (Width - 15);
 draw1.SetHeight (Height - 40);

 timer1.SetRunState (1);

};

void
CPWindow5::menu1_EvMenuActive(CControl * control)
{
 PartToCreate = ((CItemMenu*) control)->GetText ();
 lxSetCursor (lxCursor (lxCURSOR_CROSS));
};

void
CPWindow5::_EvOnCreate(CControl * control)
{
 if (LoadConfigFile.length () > 0)
  LoadConfig (LoadConfigFile);

};

void
CPWindow5::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y, uint state)
{
 x = x / scale;
 y = y / scale;

 for (int i = 0; i < partsc; i++)
  {
   if (parts[i]->PointInside ((int) x, (int) y))
    {
     parts[i]->EvMouseButtonPress (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
     if (button == 3)
      {
       PartSelected = i;
       pmenu2.SetX (x * scale);
       pmenu2.SetY (y * scale);
#if defined(__WXGTK__)||defined(__WXMSW__)
       SetPopupMenu (&pmenu2); //FIXME problemas com as propriedades
#else
       draw1.SetPopupMenu (&pmenu2);
#endif
      }
     return;
    }
  }

 //clique fora 
 if ((button == 1)&&(PartToCreate.size () > 0)&&(partsc < MAX_PARTS))
  {
   //timer1.SetRunState (0); 
   lxSetCursor (lxCursor (lxCURSOR_ARROW));
   parts[partsc] = create_part ((char *) PartToCreate.char_str (), x, y);

   if (parts[partsc] == NULL)
    {
     Message (lxT ("Erro creating part: ") + PartToCreate);
    }
   else
    {
     partsc++;
    }
   PartToCreate = "";
   _EvOnShow (control);
  }


};

void
CPWindow5::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y, uint state)
{
 x = x / scale;
 y = y / scale;

 PartToMove = -1;


 lxSetCursor (lxCursor (lxCURSOR_ARROW));
 mdx = 0;
 mdy = 0;

 for (int i = 0; i < partsc; i++)
  {
   if (parts[i]->PointInside (x, y))
    {
     parts[i]->EvMouseButtonRelease (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
     return;
    }
  }
};

void
CPWindow5::pmenu2_Properties_EvMenuActive(CControl * control)
{

 wprop.SetName ("window1"); //must be the same as in xml 
 Application->ACreateWindow (&wprop);
 wprop.DestroyChilds ();
 if (wprop.LoadXMLContextAndCreateChilds (Window1.GetSharePath () + lxT ("parts/") + parts[PartSelected]->GetPropertiesWindowFile ()))
  {
   //wprop.SetCanDestroy (false);

   parts[PartSelected]->ConfigurePropertiesWindow (&wprop);

   wprop.SetX (parts[PartSelected]->GetX () + GetX ());
   wprop.SetY (parts[PartSelected]->GetY () + GetY ());

   wprop.Draw ();
   wprop.ShowExclusive ();

  }
}

void
CPWindow5::PropClose(int tag)
{
 if (tag)
  {
   parts[PartSelected]->ReadPropertiesWindow ();
  }
 wprop.HideExclusive ();
 //wprop.SetCanDestroy (true);
 wprop.WDestroy ();
}

void
CPWindow5::PropButtonRelease(CControl * control, uint button, uint x, uint y, uint state)
{
 Window5.PropClose (control->GetTag ());
}

void
CPWindow5::timer1_EvOnTime(CControl * control)
{

 draw1.Canvas.Init (1.0, 1.0);

 draw1.Canvas.SetFgColor (50, 50, 50);
 draw1.Canvas.SetBgColor (50, 50, 50);
 draw1.Canvas.Rectangle (1, 0, 0, draw1.GetWidth (), draw1.GetHeight ());

 draw1.Canvas.ChangeScale (scale, scale);

 for (int i = 0; i < partsc; i++)
  {
   parts[i]->Draw ();
   draw1.Canvas.PutBitmap (parts[i]->GetBitmap (), parts[i]->GetX (), parts[i]->GetY ());
  }

 draw1.Canvas.End ();
 draw1.Update ();

}

void
CPWindow5::draw1_EvMouseMove(CControl * control, uint button, uint x, uint y, uint state)
{
 x = x / scale;
 y = y / scale;

 if (PartToMove >= 0)
  {


   if ((mdx == 0)&&(mdy == 0))
    {
     mdx = parts[PartToMove]->GetX () - x;
     mdy = parts[PartToMove]->GetY () - y;
    }

   parts[PartToMove]->SetX (x + mdx);
   parts[PartToMove]->SetY (y + mdy);
  }
 else
  {
   for (int i = 0; i < partsc; i++)
    {
     if (parts[i]->PointInside (x, y))
      {
       parts[i]->EvMouseMove (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
       return;
      }
    }
  }

};

void
CPWindow5::draw1_EvKeyboardPress(CControl * control, const uint key, const uint hkey, const uint mask)
{
 switch (key)
  {
  case '='://+
   scale += 0.1;
   if (scale > 2)scale = 2;
   break;
  case '-':
   scale -= 0.1;
   if (scale < 0.1)scale = 0.1;
   break;
  default:
   for (int i = 0; i < partsc; i++)
    {
     parts[i]->EvKeyPress (key, mask);
    }
   break;
  }

};

void
CPWindow5::draw1_EvKeyboardRelease(CControl * control, const uint key, const uint hkey, const uint mask)
{
 for (int i = 0; i < partsc; i++)
  {
   parts[i]->EvKeyRelease (key, mask);
  }
};

bool
CPWindow5::SaveConfig(String fname)
{
 String temp;

 CStringList prefs;
 prefs.Clear ();

 temp.Printf ("scale,0,0:%f", scale);
 prefs.AddLine (temp);

 for (int i = 0; i < partsc; i++)
  {
   temp.Printf ("%s,%i,%i:%s", parts[i]->GetName ().c_str (), parts[i]->GetX (), parts[i]->GetY (), parts[i]->WritePreferences ().c_str ());
   prefs.AddLine (temp);
  }

 return prefs.SaveToFile (fname);
}

bool
CPWindow5::LoadConfig(String fname)
{
 char name[256];
 char temp[256];
 unsigned int x, y;
 CStringList prefs;

 IOPinsCount = 0;
 pboard = Window1.GetBoard ();

 GetPinsNames ();

 for (int i = 0; i < 256; i++)
  {
   if ((i > 0)&&(i <= pboard->MGetPinCount ()))
    {
     PinNames[i] = pboard->MGetPinName (i);
    }
   else
    {
     PinNames[i] = "NC";
    }
  }



 PinsCount = pboard->MGetPinCount ();
 Pins = (picpin*) pboard->MGetPinsValues ();

 memset (&Pins[PinsCount], 0, sizeof (picpin)* (256 - PinsCount));

 if (GetWin () == NULL)
  {
   LoadConfigFile = fname;
   return 0;
  }
 else
  {
   LoadConfigFile = "";
  }

 bool ret = lxFileExists (fname);

 if (ret)
  {
   int partsc_;
   prefs.LoadFromFile (fname);

   DeleteParts ();
   partsc_ = 0;
   for (unsigned int i = 0; i < prefs.GetLinesCount (); i++)
    {
     sscanf (prefs.GetLine (i).c_str (), "%255[^,],%i,%i:%255[^\n]", name, &x, &y, temp);

     if (strcmp (name, "scale") == 0)
      {
       sscanf (temp, "%f", &scale);
      }
     else if ((parts[partsc_] = create_part (name, x, y)))
      {
       parts[partsc_]->ReadPreferences (temp);
       partsc_++;
      }
     else
      {
       printf ("Erro loading part: %s \n", name);
      }
    }
   partsc = partsc_;
  }

 return ret;
}

void
CPWindow5::DeleteParts(void)
{
 int partsc_ = partsc;
 partsc = 0; //for disable process
 scale = 1.0;

 //delete previous parts

 for (int i = 0; i < partsc_; i++)
  {
   delete parts[i];
  }

}

void
CPWindow5::menu1_File_Newconfiguration_EvMenuActive(CControl * control)
{
 if (partsc > 0)
  {
   if (Dialog ("Save current configuration?"))
    {
     menu1_File_Saveconfiguration_EvMenuActive (control);
    }
   DeleteParts ();
  }
}

void
CPWindow5::menu1_File_Saveconfiguration_EvMenuActive(CControl * control)
{
 filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
 filedialog1.Run ();
}

void
CPWindow5::menu1_File_Loadconfiguration_EvMenuActive(CControl * control)
{
 filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog1.Run ();
}

void
CPWindow5::Process(void)
{
 memset (i2c_bus, 0, PinsCount);
 for (int i = 0; i < partsc; i++)
  {
   parts[i]->Process ();
  }
}

void
CPWindow5::_EvOnHide(CControl * control)
{
 timer1.SetRunState (0);
 //board * pboard = Window1.GetBoard ();
 if (pboard)
  {
   pboard->SetUseSpareParts (0);
  }
}

void
CPWindow5::pmenu2_Move_EvMenuActive(CControl * control)
{
 PartToMove = PartSelected;
 lxSetCursor (lxCursor (lxCURSOR_SIZENWSE));
}

void
CPWindow5::pmenu2_Delete_EvMenuActive(CControl * control)
{

 int partsc_ = partsc;
 partsc = 0; //disable process

 delete parts[PartSelected];

 for (int i = PartSelected; i < partsc_ - 1; i++)
  {
   parts[i] = parts[i + 1];
  }
 partsc_--;

 partsc = partsc_;
}

void
CPWindow5::menu1_Help_Contents_EvMenuActive(CControl * control)
{
#ifdef EXT_BROWSER
 //lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
 lxLaunchDefaultBrowser (lxT ("https://lcgamboa.github.io/picsimlab/"));
#else 
 Window2.html1.SetLoadFile (Window1.GetSharePath () + lxT ("docs/picsimlab.html"));
 Window2.Show ();
#endif
}

void
CPWindow5::menu1_Help_About_EvMenuActive(CControl * control)
{
 Message (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ") + String (lxT (_VERSION_)));
}

void
CPWindow5::WritePreferences(void)
{
 Window1.saveprefs (lxT ("spare_position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()) + lxT (",") + itoa (GetWidth ()) + lxT (",") + itoa (GetHeight ()));
}

void
CPWindow5::ReadPreferences(char *name, char *value)
{

 if (!strcmp (name, "spare_position"))
  {
   int x, y, w, h;
   sscanf (value, "%i,%i,%i,%i", &x, &y, &w, &h);
   SetX (x);
   SetY (y);
   if (w > 5000)w = 5000;
   if (h > 5000)h = 5000;
   SetWidth (w);
   SetHeight (h);
   draw1.SetWidth (w - 15);
   draw1.SetHeight (h - 40);
  }
}

void
CPWindow5::filedialog1_EvOnClose(int retId)
{

 if (retId && (filedialog1.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
  {
   if (lxFileExists (filedialog1.GetFileName ()))
    {

     if (!Dialog (String ("Overwriting file: ") + basename (filedialog1.GetFileName ()) + "?"))
      return;
    }
   SaveConfig (filedialog1.GetFileName ());
  }

 if (retId && (filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
  {
   LoadConfig (filedialog1.GetFileName ());
  }
}

void
CPWindow5::Set_i2c_bus(unsigned char pin, unsigned char value)
{
 if (pin < IOINIT)
  {
   i2c_bus[pin] |= value;
  }

}

unsigned char
CPWindow5::Get_i2c_bus(unsigned char pin)
{
 if (pin < IOINIT)
  return i2c_bus[pin];
 else
  return 0;
}

String
CPWindow5::GetPinsNames(void)
{
 String Items = "0  NC,";
 String spin;

 for (int i = 1; i <= pboard->MGetPinCount (); i++)
  {
   spin = PinNames[i];

   if (spin.Cmp (lxT ("error")))
    {
     Items = Items + itoa (i) + "  " + spin + ",";
    }
  }
 for (int i = 0; i < IOPinsCount; i++)
  {
   spin = PinNames[IOINIT + i];
   Items = Items + itoa (i + IOINIT) + "  " + spin + ",";
  }
 return Items;
}

String
CPWindow5::GetPinName(unsigned char pin)
{
 return PinNames[pin];
}

const picpin *
CPWindow5::GetPinsValues(void)
{
 return Pins;
}

void
CPWindow5::SetPin(unsigned char pin, unsigned char value, unsigned char force)
{
 pboard->MSetPin (pin, value);
 if ((pin > PinsCount)&&((Pins[pin - 1].dir)||force))
  {
   Pins[pin - 1].value = value;
   Pins[pin - 1].oavalue = 255*value;
  }
}

void
CPWindow5::SetAPin(unsigned char pin, float value)
{
 pboard->MSetAPin (pin, value);
 if (pin > PinsCount)
  {
   Pins[pin - 1].avalue = value;
  }
}

unsigned char
CPWindow5::RegisterIOpin(String pname)
{
 unsigned char pin = IOPinsCount + IOINIT;
 PinNames[pin] = pname;
 IOPinsCount++;

 return pin;
}