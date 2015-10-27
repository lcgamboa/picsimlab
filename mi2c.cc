


#include"mi2c.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void
mi2c_rst(mi2c_t *mem)
{
//int i;

mem->sclo=1;
mem->bit=0xFF;
mem->byte=0xFF;
mem->datab=0;
mem->ctrl=0;
mem->ret=0;
  //printf("mi2c rst\n");

//for(i=0;i<100;i++)
   //printf("%02X ",mem->data[i]);
//printf("\n");
}

void 
mi2c_init_null(mi2c_t *mem)
{
  mem->SIZE= 0;
  mem->data=NULL;
}

void 
mi2c_init(mi2c_t *mem,int sizekbits)
{
  mem->SIZE= sizekbits*128;  //*1024/8
  
  if(mem->SIZE < 4096)
     mem->ADDRB=1;
  else
     mem->ADDRB=2;

  //printf("mi2c init size=(%i) ADDRB=(%i)\n", sizekbits,mem->ADDRB);

  mem->data=(unsigned char *) calloc(mem->SIZE,sizeof(unsigned char));

  memset(mem->data,0xFF,mem->SIZE);
  mi2c_rst(mem);
}

void
mi2c_end(mi2c_t *mem)
{
  //printf("mi2c end\n");
  if(mem->data)
    free(mem->data);
  mem->data=NULL;
}



unsigned char
mi2c_io(mi2c_t *mem, unsigned char scl, unsigned char sda)
{
  
  if((mem->sdao == 1)&&(sda ==0)&&(scl==1)&&(mem->sclo==1 )) //start
  {
    mem->bit=0;  
    mem->byte=0;
    mem->datab=0;
    mem->ctrl=0;
    mem->ret=0;
    //printf("---->mem start!\n");	 
  }
  
  if((mem->sdao == 0)&&(sda ==1)&&(scl==1)&&(mem->sclo==1)) //stop
  {
    mem->bit=0xFF;  
    mem->byte=0xFF;  
    mem->ctrl=0;
    mem->ret=0;
    //printf("---> mem stop!\n");	 
  }


  if((mem->bit < 9)&&(mem->sclo == 0)&&(scl==1))  //data 
  {

    if(mem->bit < 8)
    {
     mem->datab|=(sda<<(7-mem->bit));
    }

    mem->bit++; 
  }
  
  if((mem->bit < 9)&&(mem->sclo == 1)&&(scl==0)&&(mem->ctrl == 0x0A1))  //data 
  {
    if(mem->bit < 8)
    {
     mem->ret=((mem->datas & (1<<(7-mem->bit)))>0);
     //printf("send %i %i (%02X)\n",mem->bit,mem->ret,mem->datas);  
    }
    else
    {
     mem->ret=0;
    }
  }


  if(mem->bit == 9)
  {
     //printf("mi2c data %02X\n",mem->datab);

     if(mem->byte == 0)
     {
       if( mem->ADDRB == 2)
         mem->ctrl=mem->datab;
       else 
         mem->ctrl=mem->datab & 0xF1;

       //printf("----> mem ctrl = %02X\n",mem->ctrl);		
       mem->ret=0;
       
       if( (mem->ctrl& 0x01) == 0x00)
       {     
         if( mem->ADDRB == 2)
           mem->addr=0;
         else
         { 
           mem->addr=((mem->datab&0x0E)<<7);	
         }
       }
       
     }

     if((mem->ctrl) == 0xA0)
     {
         if( mem->ADDRB == 2)
         {
           if(mem->byte == 1)
           {
             mem->addr|=(mem->datab<<8);	
             //printf("----> mem add = %02X\n",mem->addr);		
           }
           if(mem->byte == 2)
           {
             mem->addr|=mem->datab;
             //printf("----> mem add = %02X\n",mem->addr);		
           } 	
         }
         else
         {
            if(mem->byte == 1)
            {
              mem->addr|=mem->datab;
              //printf("----> mem add = %02X\n",mem->addr);		
            };	
         }
  
         if((mem->byte > mem->ADDRB)&&((mem->ctrl &0x01) == 0))
         {
            mem->data[mem->addr]=mem->datab;
            //printf("write mem[%04X]=%02X\n",mem->addr,mem->datab); 
            mem->addr++;
         }
         mem->ret=0;
     }  
     else if((mem->ctrl ) == 0xA1)  //read
     {
       if(mem->byte < mem->SIZE)
       {  
         mem->datas=mem->data[mem->addr];
         //printf("read mem[%04X]=%02X\n",mem->addr,mem->datas); 
         mem->addr++;
       }
       else
       {
         mem->ctrl=0xFF;
       }
     }
     


     mem->bit=0; 
     mem->datab=0;
     mem->byte++;
  }	


  mem->sdao=sda;
  mem->sclo=scl;
return mem->ret;
}
