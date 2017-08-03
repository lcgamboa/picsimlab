/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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
#include"picsimlab5.h"
#include"picsimlab5_d.cc"



CPWindow5 Window5;


//Implementation

void
CPWindow5::_EvOnShow(CControl * control)
{
  draw1.SetWidth (GetWidth () - 15);
  draw1.SetHeight (GetHeight () - 40);

};

void
CPWindow5::menu1_EvMenuActive(CControl * control)
{
  PartToCreate= ((CItemMenu*)control)->GetText ();
  wxSetCursor (wxCursor (wxCURSOR_CROSS));
};

void
CPWindow5::_EvOnCreate(CControl * control)
{
  char cbuf[10];
   
  PinNames.Clear ();
  
  for(int i=1; i<= pic->PINCOUNT;i++ )
    {
      String spin= getPinName(pic,i,cbuf);
      
      if(spin.Cmp(wxT("error")))
      {
        PinNames.AddLine (itoa(i)+"  "+spin);
      }
    }
};


void
CPWindow5::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)

{


  for (int i = 0; i < partsc; i++)
    {
      if (parts[i]->PointInside (x, y))
        {
          parts[i]->MouseButtonPress (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
          if (button == 3)
            {
               PartSelected=i;
               Widget->PopupMenu ((wxMenu*) (pmenu2.GetWidget ()), x, y);
            }
          return;
        }
    }

  //clique fora 
  if ((button == 1)&&(PartToCreate.size() > 0)&&(partsc < MAX_PARTS))
    {
       wxSetCursor (wxCursor (wxCURSOR_ARROW));   
       parts[partsc] = create_part ((char *)PartToCreate.char_str (), x, y);
       
       if(parts[partsc] == NULL)
         {
           Message(wxT("Erro creating part: ")+PartToCreate);
         }
       else
         {
           partsc++;
         }
       PartToCreate="";
      _EvOnShow (control);
    }


};



void
CPWindow5::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y,uint state)
{
  PartToMove=-1;
  wxSetCursor (wxCursor (wxCURSOR_ARROW));
  mdx=0;
  mdy=0;
  
    for (int i = 0; i < partsc; i++)
    {
      if (parts[i]->PointInside (x, y))
        {
          parts[i]->MouseButtonRelease (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
          return;
        }
    }
};

void
CPWindow5::pmenu2_Propierties_EvMenuActive(CControl * control)
{  
  CPWindow * wd1= new CPWindow;
 
  wd1->SetName("window1");//must be the same as in xml 
  Application->ACreateWindow (wd1);
  
  if(wd1->LoadXMLContextAndCreateChilds (Window1.GetSharePath()+parts[PartSelected]->GetPropiertiesWindowFile()))
  {
    wd1->SetCanDestroy(false);

    parts[PartSelected]->ConfigurePropiertsWindow(wd1);
  
    wd1->Draw();
    wd1->ShowExclusive();
  
    while (wd1->GetCanExitExclusive())
    {
      Application->ProcessEvents(wd1->GetWWidget());
      wxMilliSleep(100);
    }
  
    parts[PartSelected]->ReadPropiertsWindow();
  
    wd1->SetCanDestroy (true);
    wd1->WDestroy ();
  }
};

void
CPWindow5::timer1_EvOnTime(CControl * control)
{

  draw1.Canvas.Init (1.0, 1.0);

  draw1.Canvas.SetFgColor (50, 50, 50);
  draw1.Canvas.SetBgColor (50, 50, 50);
  draw1.Canvas.Rectangle (1, 0, 0, draw1.GetWidth (), draw1.GetHeight ());


  for (int i = 0; i < partsc; i++)
    {
      parts[i]->Draw ();
      draw1.Canvas.PutBitmap (parts[i]->GetBitmap (), parts[i]->GetX (), parts[i]->GetY ());
    }

  draw1.Canvas.End ();
  draw1.Update ();

};



void
CPWindow5::draw1_EvMouseMove(CControl * control, uint button, uint x, uint y,uint state)
{
  
  if(PartToMove >= 0)
  {
 
    if((mdx == 0)&&(mdy == 0))  
      {
        mdx=parts[PartToMove]->GetX()-x; 
        mdy=parts[PartToMove]->GetY()-y;
      }
      
    parts[PartToMove]->SetX (x+mdx); 
    parts[PartToMove]->SetY (y+mdy);
  }
  
};



void
CPWindow5::draw1_EvKeyboardPress(CControl * control, uint key, uint x, uint y,uint mask)
{
  //code here:)
  mprint(wxT("draw1_EvKeyboardPress\n"));
};

void
CPWindow5::draw1_EvKeyboardRelease(CControl * control, uint key, uint x, uint y,uint mask)
{
  //code here:)
  mprint(wxT("draw1_EvKeyboardRelease\n"));
};

void
CPWindow5::menu1_File_Saveconfiguration_EvMenuActive(CControl * control)
{
  //code here:)
  mprint(wxT("menu1_File_Saveconfiguration_EvMenuActive\n"));
};

void
CPWindow5::menu1_File_Loadconfiguration_EvMenuActive(CControl * control)
{
  //code here:)
  mprint(wxT("menu1_File_Loadconfiguration_EvMenuActive\n"));
};


void 
CPWindow5::Process(void)
{
    for (int i = 0; i < partsc; i++)
    {
        parts[i]->Process ();
    } 
};


void
CPWindow5::_EvOnHide(CControl * control)
{
   Window1.GetBoard ()->SetUseSpareParts (0);
};


void
CPWindow5::pmenu2_Move_EvMenuActive(CControl * control)
{
    PartToMove=PartSelected;
    wxSetCursor (wxCursor (wxCURSOR_SIZENWSE));           
};

void
CPWindow5::pmenu2_Delete_EvMenuActive(CControl * control)
{
    timer1.SetRunState (0);
    delete  parts[PartSelected];
  
    for (int i = PartSelected; i < partsc-1; i++)
    {
        parts[i]=parts[i+1];
    } 
    partsc--;
    timer1.SetRunState (1);
};


