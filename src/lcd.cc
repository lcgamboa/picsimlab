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

#include"lcd.h"
#include<stdio.h>




void lcd_cmd(lcd_t * lcd, char cmd)
{
  int i,j;


  //switch betwwen 8 ou 4 bits communication
  if(!( lcd->flags & L_DL))
  {
     if(lcd->bc)
     {
       lcd->bc=0; 
       cmd= lcd->buff | (((unsigned char)cmd) >>4);
     }
     else
     {
       lcd->bc=1;
       lcd->buff=cmd&0xF0;
       return;
     }
  }

  //printf("LCD cmd=%#04X\n",(unsigned char)cmd);
  

//Set DDRAM address  
  if(cmd & 0x80 )
  {
    
    i=cmd & 0x7F;
 
    if(i < 40)
    {
       lcd->ddram_ad=i;
    }
    else
    {
       lcd->ddram_ad=i-24;
    }
     
    if(lcd->ddram_ad >= DDRMAX)lcd->ddram_ad =0;
    lcd->cgram_ad=0xFF;
   
    return;
  }
  
 //Set CGRAM address
  if(cmd & 0x40 )
  {
  
    lcd->cgram_ad=cmd & 0x3F;

    return;
  }

 //Function set 
  if(cmd & 0x20)
  {

//Sets interface data length
    if(cmd & 0x10) 
    {
      lcd->flags|=L_DL;
    }
    else
    {
      lcd->flags&=~L_DL;
    }
    
//Sets number of display line
    if(cmd & 0x08) 
    {
      lcd->flags|=L_NLI;
    }
    else
    {
      lcd->flags&=~L_NLI;
    }

//Sets character font
    if(cmd & 0x04) 
    {
      lcd->flags|=L_FNT;
    }
    else
    {
      lcd->flags&=~L_FNT;
    }

    return;
  }
 
  //Cursor/display shift
  if(cmd & 0x10 )
  {

//Sets shift direction  
    if(cmd & 0x04) 
    {
      lcd->flags|=L_LR;
    }
    else
    {
      lcd->flags&=~L_LR;
    }
  
//Sets cursor-move or display-shift  
    if(cmd & 0x08) 
    {
     //display shift   
      lcd->flags|=L_CD;
      
      if(lcd->flags & L_LR)
      {
        lcd->shift++;
        if(lcd->shift > 40)lcd->shift=lcd->shift-40;
      }
      else
      {
        lcd->shift--;
        if(lcd->shift < -40)lcd->shift=lcd->shift+40;
      }   
    }
    else
    {
     //cursor move   
      lcd->flags&=~L_CD;
      
      if(lcd->flags & L_LR)
        lcd->ddram_ad++;
      else
        lcd->ddram_ad--;  
    }

    lcd->update=1;
    return;
  }
 
  //Display On/Off control
  if(cmd & 0x08 )
  {
//Sets On/Off of all display
    if(cmd & 0x04) 
    {
      lcd->flags|=L_DON;
    }
    else
    {
      lcd->flags&=~L_DON;
    }
    
//Sets cursor On/Off 
    if(cmd & 0x02) 
    {
      lcd->flags|=L_CON;
    }
    else
    {
      lcd->flags&=~L_CON;
    }

//Set blink of cursor position character    
    if(cmd & 0x01) 
    {
      lcd->flags|=L_CBL;
    }
    else
    {
      lcd->flags&=~L_CBL;
    }

    lcd->update=1;  
    return;
  }
 
  
  //Entry mode set
  if(cmd & 0x04)
  {
         
//Sets cursor move direction    
    if(cmd & 0x02) 
    {
      lcd->flags|=L_DID;
    }
    else
    {
      lcd->flags&=~L_DID;
    }
   
//specifies to shift the display  
    if(cmd & 0x01) 
    {
      lcd->flags|=L_DSH;
    }
    else
    {
      lcd->flags&=~L_DSH;
    }

    return;
  }

 //Cursor home 
  if(cmd & 0x02 )
  {
    lcd->ddram_ad=0;
    lcd->update=1;  
    return;
  }
 

  //Clear display
  if(cmd & 0x01 )
  {
    for(i=0;i<DDRMAX;i++)
    {
      for(j=0;j<5;j++)
      {
        lcd->ddram[i][j]=0;
      }
    }
    lcd->ddram_ad=0;
    lcd->shift=0;
    lcd->flags|=L_DID;
    lcd->update=1; 
    return;
  }
 

};


void lcd_data(lcd_t * lcd, char data)
{
  int j;
  int fp;
  
   if( !(lcd->flags & L_DON))
   {
      lcd_cmd(lcd,data);
      return;
   }

  //switch betwwen 8 ou 4 bits communication
  if(!( lcd->flags & L_DL))
  {
     if(lcd->bc)
     {
       lcd->bc=0; 
       data= lcd->buff | (((unsigned char)data) >>4);
     }
     else
     {
       lcd->bc=1;
       lcd->buff=data&0xF0;
       return;
     }
  }
      

  /*
  if(data < 0x20)
  {
    printf("LCD dat=ERROR!\n");
    return;
  }
*/
  fp=((unsigned char)data)-0x20;


//  printf("LCD dat=%#04X  (%c)\n",(unsigned char)data,data);

  if(lcd->cgram_ad == 0xFF)
  {
    for(j=0;j<5;j++)
    {
      if(fp >= 0)
      {
          lcd->ddram[lcd->ddram_ad][j]=LCDfont[fp][j];
      }
      else
      {
        lcd->ddram[lcd->ddram_ad][j]=lcd->cgram[data&0x07][j];
      }
    }
    if(lcd->flags & L_DID)
    {
      lcd->ddram_ad++;
      if(lcd->ddram_ad >= DDRMAX)lcd->ddram_ad=0;
      if(lcd->flags & L_DSH)
      {
        lcd->shift--;
        if(lcd->shift < -40)lcd->shift=lcd->shift+40;
      }
    }
    else
    {
      lcd->ddram_ad--;
      if(lcd->ddram_ad >= DDRMAX)lcd->ddram_ad=DDRMAX;
      if(lcd->flags & L_DSH)
      {
        lcd->shift++;
         if(lcd->shift > 40)lcd->shift=lcd->shift-40;
      }
    }
 
    
    lcd->update=1; 
  }
  else
  {
    for(j=0;j<5;j++)
    {
      if((data & (0x01<<(4-j))) > 0) 
      {
        lcd->cgram[lcd->cgram_ad >>3][j]|=  (0x01<<(lcd->cgram_ad &0x07));
      }
      else
      { 
        lcd->cgram[lcd->cgram_ad >>3][j]&= ~(0x01<<(lcd->cgram_ad &0x07));
      };  
    }

    lcd->cgram_ad++;	
    if(lcd->cgram_ad >=64)lcd->cgram_ad=0;	
  }  

};



void lcd_rst(lcd_t * lcd)
{
int i,j;

//    printf("LCD rst--------------------------\n");

    for(i=0;i<DDRMAX;i++)
    {
      for(j=0;j<5;j++)
      {
        lcd->ddram[i][j]=0;
      }
    }
    for(i=0;i<8;i++)
    {
      for(j=0;j<5;j++)
      {
        lcd->cgram[i][j]=0;
      }
    }
    lcd->ddram_ad=0;
    lcd->cgram_ad=0xFF;
    lcd->update=1; 
    lcd->bc=0;
    
    lcd->blink=0;
    lcd->blinkc=0;
    lcd->shift=0;
    lcd->flags=0;
    return;
};

void lcd_on(lcd_t * lcd,int onoff)
{
  if(onoff == 1 )
  {
    lcd->flags=0;
  };
  lcd_rst(lcd);
};



void lcd_blink(lcd_t * lcd)
{
   if((lcd->flags & L_CON )&&(lcd->flags & L_CBL))
          {
            lcd->blinkc++;
            if(lcd->blinkc > 4)
            {
              lcd->blinkc=0;   
              lcd->update=1;
              lcd->blink^=1;
            }
          }
          else
           lcd->blink=0;   
}

void lcd_draw(lcd_t * lcd, CDraw *draw,int x1,int y1,int w1,int h1,int picpwr)
{
  int l,c,x,y;
  
         draw->Canvas.Rectangle (1, x1, y1, w1,h1);
         lcd->update=0;

         
         for(l=0;l<2;l++)
         {
           for(c=0;c<16;c++)
           {
             for(x=0;x<5;x++)
             { 
               for(y=0;y<8;y++)
               {
                 int cs= c-lcd->shift;
                 if(cs < 0) cs= 40+(cs%40);
                 if(cs >= 40 )cs=cs%40;
            
                 if((lcd->ddram[cs+(40*l)][x]  & (0x01<<y))&& (lcd->flags & L_DON))
                 {  
                    draw->Canvas.SetFgColor (0, 35 , 0);
                    draw->Canvas.SetColor (0, 35 , 0);
                 }
                 else
                 {
                    draw->Canvas.SetFgColor (0, 90*picpwr+35 , 0);
                    draw->Canvas.SetColor (0, 90*picpwr+35 , 0);
                 }
       //          draw->Canvas.Rectangle (1, output[i].x1+12+(x*4)+(c*22), output[i].y1+8+(y*4)+(l*38), 4,4 );
                 draw->Canvas.Rectangle (1, x1+2+(x*4)+(c*23), y1+10+(y*4)+(l*35), 4,4 );
               }
             }
           }
         } 
         
//cursor  
         if((lcd->flags & L_DON)&&(lcd->flags & L_CON))
         {
           if(lcd->ddram_ad < 40)
           {
             l=0;
             c=(lcd->ddram_ad+lcd->shift);
           }
           else
           {
             l=1;
             c=lcd->ddram_ad-40+lcd->shift;  
           }
           
           if(c < 0) c= 40+(c%40);
           if(c >= 40 )c=c%40;      
           
         
           if((c >= 0)&& (c< 16)) //draw only visible columns
           {    
             draw->Canvas.SetFgColor (0, 35 , 0);
             draw->Canvas.SetColor (0, 35 , 0);
            
             if(lcd->blink)
               draw->Canvas.Rectangle (1, x1+2+(c*23), y1+10 +(l*35), 20,32 );  
             else    
               draw->Canvas.Rectangle (1, x1+2+(c*23), y1+38+(l*35), 20,4 );
           }
         }
      

}