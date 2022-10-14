
#include"LCDfont1.h"
#include"LCDfont1_d.cc"

CPWindow1 Window1;


//Implementation


unsigned char font[5][8]={0};
int w,h;

void
CPWindow1::_EvOnShow(CControl * control)
{
  //code here:)
  mprint(lxT("_EvOnShow\n"));  
  
    draw1_EvOnDraw(control);
};



void
CPWindow1::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(lxT("draw1_EvMouseButtonPress ")+itoa(x)+"  "+itoa(y)+"\n" );
  
    font[x/(w/5)][y/(h/8)]^=0x01;
  
    draw1_EvOnDraw(control);
};


//add
void
CPWindow1::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(lxT("button1_EvMouseButtonClick\n"));
  
  text1.AddLine(edit1.GetText());
  
};


//save to file
void
CPWindow1::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(lxT("button2_EvMouseButtonClick\n"));
  
  if(filedialog1.Run())
  {
      text1.SaveToFile(filedialog1.GetFileName());
  }
  
};



//clear
void
CPWindow1::button3_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  //code here:)
  mprint(lxT("button3_EvMouseButtonClick\n"));
  
  for(int i=0;i<5;i++)
  {
    for(int j=0;j<8;j++)
    {
      font[i][j]=0;
    }
  }
  
  draw1_EvOnDraw(control);
};




void
CPWindow1::draw1_EvOnDraw(CControl * control)
{
  //code here:)
  mprint(lxT("draw1_EvOnDraw\n"));
  
 w= draw1.GetWidth();
 h= draw1.GetHeight();
  
  draw1.Canvas.Init();
  
  
  //limpa tela
  draw1.Canvas.SetBgColor(lxString(lxT("green")));
  draw1.Canvas.Rectangle(1,0,0,w,h);
  
  //desenha linhas
  for(int i=1;i<9;i++)
  {
      draw1.Canvas.Line(0,i*(h/8),w,i*(h/8));
  }
  
  for(int i=1;i<6;i++)
  {
      draw1.Canvas.Line(i*(w/5),0,i*(w/5),h);
  }
  

  
  //Draw box
  draw1.Canvas.SetBgColor(String(lxT("black")));
  for(int i=0;i<5;i++)
  {
    for(int j=0;j<8;j++)
    {
      if(font[i][j])  
        draw1.Canvas.Rectangle(1,i*(w/5),j*(h/8),(h/8),(w/5));
    }  
  }
  
  
  draw1.Canvas.End();
  String t=lxT("{");
  
  unsigned char x[6];
  
  for(int i=0;i<5;i++)
  {
    x[i]=0;  
    for(int j=0;j<8;j++)
    {
      x[i]|=font[i][j]<<j;  
    }  
    if(i< 4)
      t+=String().Format(lxT("0x%02X"),x[i])+lxT(" ,");
    else
      t+=String().Format(lxT("0x%02X"),x[i])+lxT("},\n");     
  }
  
  edit1.SetText(t);
};



void
CPWindow1::_EvOnCreate(CControl * control)
{
  //code here:)
  mprint(lxT("_EvOnCreate\n"));
  button3_EvMouseButtonClick(control, 0, 0, 0,0);
};


