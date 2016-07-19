#include <wx-3.0/wx/string.h>

#include"oscilloscope1.h"
#include"oscilloscope1_d.cc"


using namespace oscilloscope;


namespace oscilloscope{
CPWindow1 Window1;
}

//Implementation


#define WMAX 500
#define HMAX 400

double ch[2][500];

int tch=0;
    
void
CPWindow1::DrawScreen (void)
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
                
        
        for(int t=0;t<499;t++)
        {
          draw1.Canvas.Line (t,gain[0]*ch[0][t]+nivel[0],t+1,gain[0]*ch[0][t+1]+nivel[0]);    
        }
      }
    
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
       
        for(int t=0;t<499;t++)
        {
          draw1.Canvas.Line (t,gain[1]*ch[1][t]+nivel[1],t+1,gain[1]*ch[1][t+1]+nivel[1]);    
        }
      }
    
    //trigger
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
       
    
    draw1.Canvas.End (); 

};



void
CPWindow1::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  colordialog1.SetColor (button1.GetColor());
  if(colordialog1.Run ())
    {
      button1.SetColor (colordialog1.GetColor ());
    }
};



void
CPWindow1::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  colordialog1.SetColor (button2.GetColor());
  if(colordialog1.Run ())
    {
      button2.SetColor (colordialog1.GetColor ());
    }
};



void
CPWindow1::draw1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(wxT("draw1_EvMouseButtonClick\n"));
};



void 
CPWindow1::SetBaseTimer(_pic * pic)
{
  Dt=4.0/pic->freq;
  int ch[2];

 //printf("Dt=%e  Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt); 
 
 ch[0]=atoi(combo2.GetText ());
 ch[1]=atoi(combo3.GetText ());
 
 
  combo2.DeleteItems ();
  combo3.DeleteItems ();
  for(int i=1; i<= pic->PINCOUNT;i++ )
    {
      combo2.AddItem (itoa(i));
      combo3.AddItem (itoa(i));
    }
  
  if(ch[0] < pic->PINCOUNT)
    combo2.SetText (itoa(ch[0]));
  else
    combo2.SetText ("1");
  
  if(ch[1] < pic->PINCOUNT)
    combo3.SetText (itoa(ch[1]));
  else
    combo3.SetText ("2");
  
}

int is=0;
double t=0;
int tr=0;
double pins_[2];
int update=0;

void 
CPWindow1::SetSample(_pic  * pic)
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
    ch[0][is]=-pins[0]+((1.0*rand()/RAND_MAX)-0.5)*0.1;
    ch[1][is]=-pins[1]+((1.0*rand()/RAND_MAX)-0.5)*0.1;
    is++;
    if(is >= 500)
    {
        is=0;
        tr=0;
        update=1;
       // DrawScreen();
    }
  }
  t+=Dt;
  
  //trigger
  if(usetrigger)
    {
      if(!tr)
        {
          if ((pins_[tch] < triggerlv)&&(pins[tch] >= triggerlv))
            {
              tr=1;
              is=0;
            }
        }
    }

  
  pins_[0]=pins[0];
  pins_[1]=pins[1];
/*  
      for(int t=0;t<500;t++)
      {
        ch[0][t]=4*sin((t*1.0/spind5.GetValue ())+spind6.GetValue ())+((1.0*rand()/RAND_MAX)-0.5)*0.1; 
        ch[1][t]=3*cos((t*1.0/spind5.GetValue ())+spind6.GetValue ())+((1.0*rand()/RAND_MAX)-0.5)*0.1;
      }
  */    
}

void
CPWindow1::spind5_EvOnChangeSpinDouble(CControl * control)
{
  Rt=8e-5*spind5.GetValue ();
  
  //printf("Dt=%e Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt);
};


void
CPWindow1::togglebutton5_EvOnToggleButton(CControl * control)
{
  usetrigger=togglebutton5.GetCheck ();
};


void
CPWindow1::spind7_EvOnChangeSpinDouble(CControl * control)
{
  triggerlv=spind7.GetValue ();
};


void
CPWindow1::timer1_EvOnTime(CControl * control)
{
   if(update)
     {
       update=0;
       DrawScreen();
     }
};


void
CPWindow1::combo2_EvOnComboChange(CControl * control)
{
  chpin[0]=atoi(combo2.GetText ())-1;
};

void
CPWindow1::combo3_EvOnComboChange(CControl * control)
{
  chpin[1]=atoi(combo3.GetText ())-1; 
};


