
#include"getlinks1.h"
#include"getlinks1_d.cc"

#include<sys/stat.h>

CPWindow1 Window1;

//Implementation

void
CPWindow1::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  if(filedialog1.Run())
  {
  lxTextFile fin;
  String line1,line2,line3,line4;
  int i,f,s;

  fin.Open (filedialog1.GetFileName());
  fin.GoToLine(-1);

  if(fin.IsOpened() == true)
  {
    i=-1; 	  
    list1.DeleteItems();
    while(fgetline(fin,line1))
    {
      list1.SetVisible(false,false);
      i=line1.find(lxT("href=\""));
      if(i <= 0)i=line1.find(lxT("HREF=\""));
      if(i <= 0)i=line1.find(lxT("Href=\""));
      if(i >= 0)
      {
      line2=line1.substr(i+6,line1.size());
      line3=lxT("");
      f=-1; 
      while(f<0)
      {
        f=line2.find(lxT("\""));
        if(f<0)
        {
          fgetline(fin,line3);
	  line2+=line3;
	}
      };
      //cout<<"-"<<line2.substr(0,f)<<endl;
      line4=lxT("\"")+edit1.GetText()+line2.substr(0,f)+lxT("\"");
      s=-1;      
      s=line4.find(lxT(" "));
      while(s>=0)
      {
        line4[s]='%';
        line4=line4.substr(0,s+1)+lxT("20")+line4.substr(s+1,line4.size());
        s=line4.find(lxT(" "));
      };
      list1.AddItem(line4);
      };
    };
    list1.SetVisible(true);
    list1.Draw();
    fin.Close();
  };
  };
};

void
CPWindow1::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  if(filedialog1.Run())
  {
  list1.SaveItemsToFile(filedialog1.GetFileName());
  };  
};

void
CPWindow1::button3_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
    String cmd;	
    String file;
    struct stat statf;
    
    int g,d=0;
    for(int c=0;c < list1.GetItemsCount() ;c++)
    {
      if(d >=50)
      {
         Message(lxT("More 50?"));
	 d=0;
      };	     
      file=list1.GetItem(c);
      g=-1;
      g=file.rfind(lxT("/"));
      if(g<0)g=0;
      file=file.substr(g+1,file.size()-1);
      
      if((stat((char *)file.char_str(),&statf) == -1)&&(file.size() > 0))
      {
        cmd=lxT("xterm -e wget ")+list1.GetItem(c)+lxT(" &");	    
        //cout<<cmd<<endl;
        system((char *)cmd.char_str());
        d++;
      }
      else
      {
       if(file.size() > 2)printf("%s OK!\n",(char *)file.char_str());
      };
    };
};

void
CPWindow1::button4_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{
  list1.DeleteItem(list1.GetSelectedItemN());
};









