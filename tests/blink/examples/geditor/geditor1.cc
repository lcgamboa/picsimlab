
#include"geditor1.h"
#include"geditor1_d.cc"

CPWindow1 Window1;

//Implementation

bool fig[10][10];


void
CPWindow1::_EvOnCreate(CControl * control)
{
  for(int c=0;c<=9;c++)
  {      
    for(int d=0;d<=9;d++)
    {	
      grid1.SetCellBgColor(c,d,*lxRED);
      fig[c][d]=false;
    }
  };
  grid1.Draw();
};


void
CPWindow1::menu1_Editar_Limpar_EvMenuActive(CControl * control)
{
  for(int c=0;c<=9;c++)
  {      
    for(int d=0;d<=9;d++)
    {	
      grid1.SetCellBgColor(c,d,*lxRED);
      fig[c][d]=false;
    }
  };
  grid1.Draw();
};

void
CPWindow1::menu1_Ajuda_Sobre_EvMenuActive(CControl * control)
{
  Message(lxT("Desenvolvido por LCGamboa"));	
};

void
CPWindow1::menu1_Arquivo_Abrir_EvMenuActive(CControl * control)
{
  char ch;
  FILE* file;
  int c,d;
  if(Window1.filedialog1.Run())
    {	
    Window1.filedialog2.SetFileName(Window1.filedialog1.GetFileName());	    
    file=fopen((char*)Window1.filedialog1.GetFileName().char_str(),"r");
     
    if(file)
    { 
      for(c=0;c<=9;c++)
        {      
        for(d=0;d<=9;d++)
	  {	
          ch=fgetc(file);
          if(ch == '-')
	    {	  
            grid1.SetCellBgColor(c,d,*lxBLUE);
            fig[c][d]=true;
	    }
          else 
	    {	  
            grid1.SetCellBgColor(c,d,*lxRED);
            fig[c][d]=false;
	    };
          ch=fgetc(file);
	  };
       ch=fgetc(file);
       ch=fgetc(file);
       };
    fclose(file);
    grid1.Draw();
    }
    else
     Message(lxT("Error Open File!"));
  };
};

void
CPWindow1::menu1_Arquivo_Salvar_EvMenuActive(CControl * control)
{
  int c,d;
  FILE* file;
  if(Window1.filedialog2.Run())
    {	  
    file = fopen((char *)Window1.filedialog2.GetFileName().char_str(),"w");
      for(c=0;c<= 9;c++)
        {
        for(d=0;d<=9;d++)
	  {	
          if(fig[c][d])
            fprintf(file,"-1");
          else
            fprintf(file," 1");
	  };
          fprintf(file," \n");
        };
    fclose(file);
  };
};

void
CPWindow1::menu1_Arquivo_Sair_EvMenuActive(CControl * control)
{
  Window1.WDestroy();
};



void
CPWindow1::grid1_EvOnGridCellLeftClick(CControl * control)
{
     
    int c=grid1.GetActiveRow();
    int d=grid1.GetActiveCol();
  
    if(fig[c][d])
    {	  
      grid1.SetCellBgColor(c,d,*lxRED);
      fig[c][d]=false;
    }  
    else 
    {	    
      grid1.SetCellBgColor(c,d,*lxBLUE);
      fig[c][d]=true;
    };

   grid1.Draw();
};











