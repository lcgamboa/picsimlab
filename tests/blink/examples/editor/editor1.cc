
#include"editor1.h"
#include"editor1_d.cc"

CPWindow1 Window1;

//Implementation

/* XPM */
static const char * blue_xpm[] = {
"24 24 2 1",
" 	c None",
".	c #1500FF",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................"};

/* XPM */
static const char * red_xpm[] = {
"24 24 2 1",
" 	c None",
".	c #FF0000",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................",
"........................"};


bool fig[100];


void
CPWindow1::toolbuttonMouseButtonPress(CControl* control,uint button,uint x,uint y,uint mask)
{

  if(button == 1)
  {
    CToolButton* tbutton;
    tbutton=dynamic_cast<CToolButton*>(control);
    if(fig[tbutton->GetTag()])
    {	  
      tbutton->SetImgData((char**)red_xpm);
      fig[tbutton->GetTag()]=false;
    }  
    else 
    {	    
      tbutton->SetImgData((char**)blue_xpm);
      fig[tbutton->GetTag()]=true;
    };
  };

};


void
CPWindow1::_EvOnCreate(CControl * control)
{
  CToolButton* tbutton;
  for(int i=1;i<=100;i++)
  {
    tbutton=dynamic_cast<CToolButton*>(Window1.GetChildByName(lxT("toolbutton")+itoa(i)));
    tbutton->SetImgData((char **)red_xpm);
    tbutton->SetBorder(0);
    fig[tbutton->GetTag()]=false;
    tbutton->Draw();
  };
};

void
CPWindow1::menu1_Editar_Limpar_EvMenuActive(CControl * control)
{
  CToolButton* tbutton;
  for(int i=1;i<=100;i++)
  {
    tbutton=dynamic_cast<CToolButton*>(Window1.GetChildByName(lxT("toolbutton")+itoa(i)));
    tbutton->SetImgData((char**)red_xpm);
    fig[tbutton->GetTag()]=false;
    tbutton->Draw();
  };
};

void
CPWindow1::menu1_Ajuda_Sobre_EvMenuActive(CControl * control)
{
  Message(lxT("Desenvolvido por LCGamboa"));	
};

void
CPWindow1::menu1_Arquivo_Abrir_EvMenuActive(CControl * control)
{
  Window1.filedialog1.Run();
}

void
CPWindow1::menu1_Arquivo_Salvar_EvMenuActive(CControl * control)
{
  Window1.filedialog2.Run();
};

void
CPWindow1::menu1_Arquivo_Sair_EvMenuActive(CControl * control)
{
  Window1.WDestroy();
};


void
CPWindow1::toolbutton1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton2_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton3_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton4_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton5_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton6_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton7_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton8_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton9_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton10_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton11_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton12_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton13_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton14_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton15_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton16_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton17_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton18_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton19_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton20_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton21_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton22_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton23_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton24_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton25_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton26_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton27_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton28_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton29_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton30_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton31_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton32_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton33_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton34_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton35_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton36_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton37_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton38_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton39_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton40_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton41_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton42_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton43_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton44_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton45_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton46_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton47_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton48_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton49_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton50_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton51_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton52_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton53_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton54_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton55_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton56_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton57_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton58_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton59_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton60_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton61_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton62_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton63_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton64_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton65_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton66_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton67_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton68_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton69_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton70_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton71_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton72_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton73_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton74_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton75_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton76_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton77_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton78_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton79_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton80_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton81_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton82_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton83_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton84_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton85_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton86_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton87_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton88_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton89_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton90_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton91_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton92_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton93_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton94_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton95_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton96_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton97_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton98_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton99_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
};

void
CPWindow1::toolbutton100_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  toolbuttonMouseButtonPress(control,button,x,y,state);
}


void
CPWindow1::filedialog1_EvOnClose(int retId)
{
  char ch;
  FILE* file;
  int c,d,g;
  CToolButton* tbutton;
  if(retId)
    {	
    Window1.filedialog2.SetFileName(Window1.filedialog1.GetFileName());	    
    file=fopen((char*)Window1.filedialog1.GetFileName().char_str(),"r");
     
    if(file)
    { 
      for(c=0;c<=9;c++)
        {      
        for(d=0;d<=9;d++)
	  {	
          g=(c*10)+d;
          ch=fgetc(file);
          tbutton=dynamic_cast<CToolButton*>(Window1.GetChildByName(lxT("toolbutton")+itoa(g+1)));
          if(ch == '-')
	    {	  
            tbutton->SetImgData((char**)blue_xpm);
            tbutton->Draw();
            fig[g]=true;
	    }
          else 
	    {	  
            tbutton->SetImgData((char**)red_xpm);
            tbutton->Draw();
            fig[g]=false;
	    };
          ch=fgetc(file);
	  };
       ch=fgetc(file);
       ch=fgetc(file);
       };
    fclose(file);
    }
    else
     Message(lxT("Error Open File!"));
  };
}

void
CPWindow1::filedialog2_EvOnClose(int retId)
{
  int c,d,g;
  FILE* file;
  if(retId)
    {	  
    file = fopen((char *)Window1.filedialog2.GetFileName().char_str(),"w");
      for(c=0;c<= 9;c++)
        {
        for(d=0;d<=9;d++)
	  {	
          g=(c*10)+d;
          if(fig[g])
            fprintf(file,"-1");
          else
            fprintf(file," 1");
	  };
          fprintf(file," \n");
        };
    fclose(file);
  };
}


