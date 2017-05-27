#include <wx-3.0/wx/string.h>

#include"picsimlab4.h"
#include"picsimlab1.h"

#include"picsimlab4_d.cc"


CPWindow4 Window4;


//Implementation


#define WMAX 500
#define HMAX 400

#define NPOINTS 1000
//flip buffers+ 2 channels + 500 points
double data[2][2][NPOINTS];
int fp=0;

double *ch[2];

int tch=0;
    
void
CPWindow4::DrawScreen (void)
{
    draw1.Canvas.Init ();
    
    //Clear background
    draw1.Canvas.SetFgColor (0,0,0);
    draw1.Canvas.SetBgColor (0,0,0);
    draw1.Canvas.Rectangle (1,0,0,WMAX,HMAX);
    //draw lines
    draw1.Canvas.SetFgColor (50,50,50);
    
    int inch=HMAX/10;
    for(int i=1;i<10;i++)
      draw1.Canvas.Line (0,inch*i,WMAX,inch*i);
    
    int incw=WMAX/10;
    for(int i=1;i<10;i++)
      draw1.Canvas.Line (incw*i,0,incw*i,HMAX);
    
    draw1.Canvas.SetFgColor (140,140,140);
    draw1.Canvas.Line (0,HMAX/2,WMAX,HMAX/2);
    draw1.Canvas.Line (WMAX/2,0,WMAX/2,HMAX);    

    float gain[3],nivel[3];
    wxPoint pts[3]; 
        
    
    //draw ch 0
    if(togglebutton1.GetCheck ())
      {
        gain[0]= (HMAX/spind1.GetValue ())/10.0;
        nivel[0]= (gain[0]*-spind2.GetValue ())+(HMAX/2.0);
        if(togglebutton3.GetCheck ())gain[0]=-gain[0];
        draw1.Canvas.SetFgColor (button1.GetColor());
        
        pts[0].x=0;
        pts[0].y=nivel[0]-8;
        pts[1].x=8;
        pts[1].y=nivel[0];
        pts[2].x=0;
        pts[2].y=nivel[0]+8;        
        draw1.Canvas.Polygon (1,pts,3);
                
        draw1.Canvas.SetLineWidth (2);
        for(int t=0;t< (NPOINTS/2)-1;t++)
        {
          draw1.Canvas.Line (t,gain[0]*ch[0][t]+nivel[0],t+1,gain[0]*ch[0][t+1]+nivel[0]);    
        }
      }
    draw1.Canvas.SetLineWidth (1);
    //draw ch 1
    if(togglebutton2.GetCheck ())
      {
        gain[1]= (HMAX/spind3.GetValue ())/10.0;
        nivel[1]= (gain[1]*-spind4.GetValue ())+(HMAX/2.0);
        if(togglebutton4.GetCheck ())gain[1]=-gain[1];
        draw1.Canvas.SetFgColor (button2.GetColor());
        
        pts[0].x=0;
        pts[0].y=nivel[1]-5;
        pts[1].x=5;
        pts[1].y=nivel[1];
        pts[2].x=0;
        pts[2].y=nivel[1]+5;        
        draw1.Canvas.Polygon (1,pts,3);
       
        draw1.Canvas.SetLineWidth (2);
        for(int t=0;t< (NPOINTS/2)-1;t++)
        {
          draw1.Canvas.Line (t,gain[1]*ch[1][t]+nivel[1],t+1,gain[1]*ch[1][t+1]+nivel[1]);    
        }
      }
      draw1.Canvas.SetLineWidth (1);
    
    //draw trigger level
    if(usetrigger)
    {
      if(combo1.GetText().compare (wxT("1")) == 0)
        tch=0;
      else
        tch=1;
    
      draw1.Canvas.SetFgColor (255,255,0); 
      nivel[2]= (gain[tch]*-spind7.GetValue ())+nivel[tch];
      pts[0].x=0;
      pts[0].y=nivel[2]-3;
      pts[1].x=3;
      pts[1].y=nivel[2];
      pts[2].x=0;
      pts[2].y=nivel[2]+3;        
      draw1.Canvas.Polygon (1,pts,3);
    }
      
    //draw toffset level
   
    draw1.Canvas.SetFgColor (255,255,0); 
    nivel[2]= WMAX-toffset;
    pts[0].y=1;
    pts[0].x=nivel[2]-3;
    pts[1].y=1+3;
    pts[1].x=nivel[2];
    pts[2].y=1;
    pts[2].x=nivel[2]+3;        
    draw1.Canvas.Polygon (1,pts,3);
    
    
    draw1.Canvas.End (); 

};



void
CPWindow4::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  colordialog1.SetColor (button1.GetColor());
  if(colordialog1.Run ())
    {
      button1.SetColor (colordialog1.GetColor ());
    }
};



void
CPWindow4::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  colordialog1.SetColor (button2.GetColor());
  if(colordialog1.Run ())
    {
      button2.SetColor (colordialog1.GetColor ());
    }
};



void
CPWindow4::draw1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(wxT("draw1_EvMouseButtonClick\n"));
};



void 
CPWindow4::SetBaseTimer(void)
{
  if(pic == NULL)return;

  Dt=4.0/pic->freq;
  int chp[2];
  char cbuf[10];

 //printf("Dt=%e  Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt); 
 
 chp[0]=atoi(combo2.GetText ());
 chp[1]=atoi(combo3.GetText ());
 
 
  combo2.DeleteItems ();
  combo3.DeleteItems ();
  for(int i=1; i<= pic->PINCOUNT;i++ )
    {
      String spin= getPinName(pic,i,cbuf);
      
      if(spin.Cmp(wxT("error")))
      {
        combo2.AddItem (itoa(i)+"  "+spin);
        combo3.AddItem (itoa(i)+"  "+spin);
      }
    }
  
  if(chp[0] < pic->PINCOUNT)
    {
      String spin= getPinName(pic,chp[0],cbuf);  
      combo2.SetText (itoa(chp[0])+"  "+spin);
    }
  else
    combo2.SetText ("1");
  
  if(chp[1] < pic->PINCOUNT)
    {
      String spin= getPinName(pic,chp[1],cbuf);  
      combo3.SetText (itoa(chp[1])+"  "+spin);
    }
  else
    combo3.SetText ("2");
  
  spind5_EvOnChangeSpinDouble(this);
}

int is=0;
double t=0;
int tr=0;
double pins_[2];
int update=0;

void 
CPWindow4::SetSample(void)
{
  double pins[2];
  
  if(pic->pins[chpin[0]].ptype == PT_ANALOG)
    pins[0]=pic->pins[chpin[0]].avalue;
  else
    pins[0]=pic->pins[chpin[0]].value*5.0;
  
  if(pic->pins[chpin[1]].ptype == PT_ANALOG)
    pins[1]=pic->pins[chpin[1]].avalue;
  else
    pins[1]=pic->pins[chpin[1]].value*5.0;
  
  if(t > Rt)
  {
    t-=Rt;
    data[fp][0][is]=-pins[0]+((1.0*rand()/RAND_MAX)-0.5)*0.1;
    data[fp][1][is]=-pins[1]+((1.0*rand()/RAND_MAX)-0.5)*0.1;
    is++;
    if(is >= NPOINTS)
    {
        is=0;
        tr=0;
        ch[0]=&data[fp][0][toffset];
        ch[1]=&data[fp][1][toffset];
        fp=!fp;//togle fp
        update=1;
       // DrawScreen();
    }
   
  }
  t+=Dt;
  
  //trigger
  if(usetrigger)
    {
      if((!tr)&&(is >= NPOINTS/2))
        {
          if ((pins_[tch] < triggerlv)&&(pins[tch] >= triggerlv))
            {
              tr=1;
              
              is=is-(NPOINTS/2);
              
              for(int i =0;i< NPOINTS/2; i++ )
              {
                data[fp][0][i]=data[fp][0][i+is];
                data[fp][1][i]=data[fp][1][i+is];
               }
              is=(NPOINTS/2);
            }
        }
    }

  
  pins_[0]=pins[0];
  pins_[1]=pins[1];
}

void
CPWindow4::spind5_EvOnChangeSpinDouble(CControl * control)
{
  Rt=  ((spind5.GetValue ()*1e-3*10)/WMAX);  
  
  spind6.SetMin (-5*spind5.GetValue ());
  spind6.SetMax (5*spind5.GetValue ());
  
  spind6.SetValue (0);
  toffset=250;
  //printf("Dt=%e Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt);
};


void
CPWindow4::togglebutton5_EvOnToggleButton(CControl * control)
{
  usetrigger=togglebutton5.GetCheck ();
};


void
CPWindow4::spind7_EvOnChangeSpinDouble(CControl * control)
{
  triggerlv=spind7.GetValue ();
};


void
CPWindow4::timer1_EvOnTime(CControl * control)
{
   if(update)
     {
       update=0;
       DrawScreen();
     }
};


void
CPWindow4::combo2_EvOnComboChange(CControl * control)
{
  chpin[0]=atoi(combo2.GetText ())-1;
};

void
CPWindow4::combo3_EvOnComboChange(CControl * control)
{
  chpin[1]=atoi(combo3.GetText ())-1; 
};

void 
CPWindow4::WritePreferences(void)
{
    //TODO write all preferences
  Window1.saveprefs(wxT("osc_scale1"),ftoa(spind1.GetValue()));
  Window1.saveprefs(wxT("osc_offset1"),ftoa(spind2.GetValue()));
  Window1.saveprefs(wxT("osc_on1"),itoa(togglebutton1.GetCheck()));
  Window1.saveprefs(wxT("osc_color1"),button1.GetColor().GetAsString (wxC2S_HTML_SYNTAX));
  Window1.saveprefs(wxT("osc_inv"),itoa(togglebutton3.GetCheck()));
  Window1.saveprefs(wxT("osc_ch1"),combo2.GetText());
    
  Window1.saveprefs(wxT("osc_scale2"),ftoa(spind3.GetValue()));
  Window1.saveprefs(wxT("osc_offset2"),ftoa(spind4.GetValue()));
  Window1.saveprefs(wxT("osc_on2"),itoa(togglebutton2.GetCheck()));
  Window1.saveprefs(wxT("osc_color2"),button2.GetColor().GetAsString (wxC2S_HTML_SYNTAX));
  Window1.saveprefs(wxT("osc_inv2"),itoa(togglebutton4.GetCheck()));    
  Window1.saveprefs(wxT("osc_ch2"),combo3.GetText());
  
  Window1.saveprefs(wxT("osc_tscale"),ftoa(spind5.GetValue()));
  //Window1.saveprefs(wxT("osc_toffset"),ftoa(spind6.GetValue()));
  Window1.saveprefs(wxT("osc_usetrigger"),itoa(togglebutton5.GetCheck()));
  Window1.saveprefs(wxT("osc_tch"),combo1.GetText());
  Window1.saveprefs(wxT("osc_tlevel"),ftoa(spind7.GetValue()));

};


void 
CPWindow4::ReadPreferences(char *name,char *value)
{
    if(!strcmp(name,"osc_scale1"))
    {
      spind1.SetValue(atof(value));
    }

    if(!strcmp(name,"osc_offset1"))
    {
      spind2.SetValue(atof(value));
    }
    
    if(!strcmp(name,"osc_on1"))
    {
      togglebutton1.SetCheck(atoi(value));
    }
    
     if(!strcmp(name,"osc_color1"))
    {
      button1.SetColor (wxColor(value));
    }

    if(!strcmp(name,"osc_inv1"))
    {
      togglebutton3.SetCheck(atoi(value));
    }
    
    if(!strcmp(name,"osc_ch1"))
    {
      combo2.SetText(value); 
      chpin[0]=atoi(combo2.GetText ())-1;
    }
    
    if(!strcmp(name,"osc_scale2"))
    {
      spind3.SetValue(atof(value));
    }

    if(!strcmp(name,"osc_offset2"))
    {
      spind4.SetValue(atof(value));
    }
    
    if(!strcmp(name,"osc_on2"))
    {
      togglebutton2.SetCheck(atoi(value));
    }
    
     if(!strcmp(name,"osc_color2"))
    {
      button2.SetColor (wxColor(value));
    }

    if(!strcmp(name,"osc_inv2"))
    {
      togglebutton4.SetCheck(atoi(value));
    }
    
    if(!strcmp(name,"osc_ch2"))
    {
      combo3.SetText(value); 
      chpin[1]=atoi(combo3.GetText ())-1;
    }
    
    if(!strcmp(name,"osc_tscale"))
    {
      spind5.SetValue(atof(value));
    }
    
    if(!strcmp(name,"osc_usetrigger"))
    {
      togglebutton5.SetCheck(atoi(value));
      usetrigger=atoi(value);
    }
    
    if(!strcmp(name,"osc_tch"))
    {
      combo1.SetText(value); 
    }
    
    if(!strcmp(name,"osc_tlevel"))
    {
      spind7.SetValue(atof(value));
    }
};


void
CPWindow4::_EvOnDestroy(CControl * control)
{
  //code here:)
  //mprint(wxT("_EvOnDestroy\n"));
};

void
CPWindow4::_EvOnShow(CControl * control)
{
  //code here:)
  //mprint(wxT("_EvOnShow\n"));
};

void
CPWindow4::_EvOnHide(CControl * control)
{
  Window1.GetBoard ()->SetUseOscilloscope (0);
};



void
CPWindow4::spind6_EvOnChangeSpinDouble(CControl * control)
{
 
  
  toffset=250- ((250*spind6.GetValue ())/(5*spind5.GetValue ()));
};


