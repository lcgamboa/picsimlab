


#include"rtc2.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


time_t utime2;
struct tm *dtime2;

void
rtc2_rst(rtc2_t *rtc)
{
//int i;

rtc->sclo=1;
rtc->bit=0xFF;
rtc->byte=0xFF;
rtc->datab=0;
rtc->ctrl=0;
rtc->ret=0;
  //printf("rtc rst\n");

//for(i=0;i<16;i++)
   //printf("%02X ",rtc->data[i]);
//printf("\n");
}

void 
rtc2_init(rtc2_t *rtc)
{
  //printf("rtc init\n");
  
  rtc->data=(unsigned char *) calloc(64,sizeof(unsigned char));

  memset(rtc->data,0xFF,16);
  rtc2_rst(rtc);
         

  utime2=time(NULL);
  dtime2=localtime(&utime2);
  rtc->data[0]=((dtime2->tm_sec/10)<<4)|(dtime2->tm_sec%10);
  rtc->data[1]=((dtime2->tm_min/10)<<4)|(dtime2->tm_min%10);
  rtc->data[2]=((dtime2->tm_hour/10)<<4)|(dtime2->tm_hour%10);
  rtc->data[3]=dtime2->tm_wday;
  rtc->data[4]=((dtime2->tm_mday/10)<<4)|(dtime2->tm_mday%10);
  rtc->data[5]=(((dtime2->tm_mon+1)/10)<<4)|((dtime2->tm_mon+1)%10);
  rtc->data[6]=(((dtime2->tm_year%100)/10)<<4)|(dtime2->tm_year%10);


}

int rtcc2=0;
int alarm2;

void rtc2_update(rtc2_t *rtc)
{
  rtcc2++;

  if(rtcc2 >= 10)
  {
    rtcc2=0;
    if((rtc->data[0] & 0x80) == 0 )
    {
       
      dtime2->tm_sec++;
      if(dtime2->tm_sec == 60)
      {
        dtime2->tm_sec =0;
        dtime2->tm_min++;
      } 
      if(dtime2->tm_min == 60)
      {
        dtime2->tm_min=0;
        dtime2->tm_hour++;  
      }
      if(dtime2->tm_hour == 24)
      {
        dtime2->tm_hour=0;
        dtime2->tm_mday++;
        dtime2->tm_wday++;
      } 
      if(dtime2->tm_wday == 7)dtime2->tm_wday=0;
 
      if(dtime2->tm_mday == 31)
      {
        dtime2->tm_mday=0;
        dtime2->tm_mon++;
      } 
      if(dtime2->tm_mon == 13)
      {
        dtime2->tm_mon=1;
        dtime2->tm_year++;
      }  

      rtc->data[0]=((dtime2->tm_sec/10)<<4)|(dtime2->tm_sec%10);
      rtc->data[1]=((dtime2->tm_min/10)<<4)|(dtime2->tm_min%10);
      rtc->data[2]=((dtime2->tm_hour/10)<<4)|(dtime2->tm_hour%10);
      rtc->data[3]=dtime2->tm_wday;
      rtc->data[4]=((dtime2->tm_mday/10)<<4)|(dtime2->tm_mday%10);
      rtc->data[5]=(((dtime2->tm_mon+1)/10)<<4)|((dtime2->tm_mon+1)%10);
      rtc->data[6]=(((dtime2->tm_year%100)/10)<<4)|(dtime2->tm_year%10);
    };

//alarm
  alarm2=0;
/*
//minute
  if((rtc->data[0x9]&0x80) == 0)
  {
    if((rtc->data[0x9]&0x7F)==(rtc->data[0x3]&0x7F))
    {
      alarm2=1;
    }
    else
    {
      alarm2=0; 
    }
  }

//hour
  if((rtc->data[0xA]&0x80) == 0)
  {
    if((rtc->data[0xA]&0x3F)==(rtc->data[0x4]&0x3F))
    {
      alarm2=1;
    }
    else
    {
      alarm2=0; 
    }
  }

//day
  if((rtc->data[0xB]&0x80) == 0)
  {
    if((rtc->data[0xB]&0x3F)==(rtc->data[0x5]&0x3F))
    {
      alarm2=1;
    }
    else
    {
      alarm2=0; 
    }
  }

//week day
  if((rtc->data[0xC]&0x80) == 0)
  {
    if((rtc->data[0xC]&0x07)==(rtc->data[0x6]&0x07))
    {
      alarm2=1;
    }
    else
    {
      alarm2=0; 
    }
  }

  if(alarm2)
  {

   rtc->data[0x2]|=0x08; //AF
   //printf("RTC Alarm !\n");

   
   //interrupt
   if(rtc->data[0x2] & 0x02) 
   {
      //printf("RTC Alarm Interrupt!\n");
   }

  }

*/
 }

};

void
rtc2_end(rtc2_t *rtc)
{
  //printf("rtc end\n");
  if(rtc->data)
    free(rtc->data);
  rtc->data=NULL;
}



unsigned char
rtc2_io(rtc2_t *rtc, unsigned char scl, unsigned char sda)
{

  if((rtc->sdao == 1)&&(sda ==0)&&(scl==1)&&(rtc->sclo == 1)) //start
  {
    rtc->bit=0;  
    rtc->byte=0;
    rtc->datab=0;
    rtc->ctrl=0;
    rtc->ret=0;
    //printf("rtc start!\n");	 
  }
  
  if((rtc->sdao == 0)&&(sda ==1)&&(scl==1)&&(rtc->sclo == 1)) //stop
  {
    rtc->bit=0xFF;  
    rtc->byte=0xFF;  
    rtc->ctrl=0;
    rtc->ret=0;
    //printf("rtc stop!\n");	 
  }


  if((rtc->bit < 9)&&(rtc->sclo == 0)&&(scl==1))  //data 
  {

    if(rtc->bit < 8)
    {
     rtc->datab|=(sda<<(7-rtc->bit));
    }

    rtc->bit++; 
  }
  
  if((rtc->bit < 9)&&(rtc->sclo == 1)&&(scl==0)&&(rtc->ctrl == 0x0D1))  //data 
  {
    if(rtc->bit < 8)
    {
     rtc->ret=((rtc->datas & (1<<(7-rtc->bit)))>0);
    ////printf("send %i %i (%02X)\n",rtc->bit,rtc->ret,rtc->datas);  
    }
    else
    {
     rtc->ret=0;
    }
  }


  if(rtc->bit == 9)
  {
     //printf("rtc data %02X\n",rtc->datab);

     if(rtc->byte == 0)
     {
       rtc->ctrl=rtc->datab;
       //printf("rtc ctrl = %02X\n",rtc->ctrl);		
       rtc->ret=0;
       
       if( (rtc->ctrl& 0x01) == 0x00)
       {     
           rtc->addr=((rtc->ctrl&0x0E)<<7);	
       };
       
     }

     if((rtc->ctrl) == 0xD0)
     {
         if(rtc->byte == 1)rtc->addr=rtc->datab;	
  
         if(((rtc->byte > 1)&&(rtc->ctrl &0x01) == 0))
         {
            //printf("write rtc[%04X]=%02X\n",rtc->addr,rtc->datab); 
            rtc->data[rtc->addr]=rtc->datab;
 
            switch(rtc->addr)
            {
              case 0:
                 dtime2->tm_sec=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 1:
                 dtime2->tm_min=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 2:
                 dtime2->tm_hour=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 4:
                 dtime2->tm_mday=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 3:
                 dtime2->tm_wday=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 5:
                 dtime2->tm_mon=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 6:
                 dtime2->tm_year= (dtime2->tm_year&0xFF00)|((((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F)); 
                 break; 
             default:
                break;   
            } 
 
            rtc->addr++;
            rtc->ret=0;
          }
     }  
     else if((rtc->ctrl ) == 0xD1)  //read
     {
       if(rtc->byte < 16)
       {  
         rtc->datas=rtc->data[rtc->addr];
         //printf("read rtc[%04X]=%02X\n",rtc->addr,rtc->datas); 
         rtc->addr++;
       }
       else
       {
         rtc->ctrl=0xFF;
       }
     }
 
     


     rtc->bit=0; 
     rtc->datab=0;
     rtc->byte++;
  }	


  rtc->sdao=sda;
  rtc->sclo=scl;
return rtc->ret;
}
