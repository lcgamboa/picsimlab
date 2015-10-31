
#include"picsimlab1.h"
#include"board_4.h"

/* outputs */

#define O_RB0	1
#define O_RB1	2
#define O_RB2	3
#define O_RB3	4
#define O_RB4	5
#define O_RB5	6
#define O_RB6	7
#define O_RB7	8
#define O_LPWR	9
#define O_LCD	10
#define O_A1	11
#define O_B1	12
#define O_C1	13
#define O_D1	14
#define O_E1	15
#define O_F1	16
#define O_G1	17
#define O_P1	18
#define O_A2	19
#define O_B2	20
#define O_C2	21
#define O_D2	22
#define O_E2	23
#define O_F2	24
#define O_G2	25
#define O_P2	26
#define O_A3	27
#define O_B3	28
#define O_C3	29
#define O_D3	30
#define O_E3	31
#define O_F3	32
#define O_G3	33
#define O_P3	34
#define O_A4	35
#define O_B4	36
#define O_C4	37
#define O_D4	38
#define O_E4	39
#define O_F4	40
#define O_G4	41
#define O_P4	42
#define O_RD0	43
#define O_RD1	44
#define O_RD2	45
#define O_RD3	46
#define O_RD4	47
#define O_RD5	48
#define O_RD6	49
#define O_RD7	50
#define O_RL1	51
#define O_RL2	52
#define O_D01	53
#define O_D02	54
#define O_D03	55
#define O_D04	56
#define O_D05	57
#define O_D06	58
#define O_D07	59
#define O_D08	60
#define O_D09	61
#define O_D10	62
#define O_D11	63
#define O_D12	64
#define O_D13	65
#define O_D14	66
#define O_D15	67
#define O_D16	68
#define O_D17	69
#define O_D18	70
#define O_D19	71
#define O_D20	72
#define O_VT	73
#define O_PRG   74
#define O_RUN   75



/*inputsa*/

#define I_RST	1
#define I_RA1	2
#define I_RA2	3
#define I_RA3	4
#define I_RA4	5
#define I_RA5	6
#define I_PWR	7
#define I_ICSP	8
#define I_RB0	9
#define I_RB1	10
#define I_RB2	11
#define I_RB3	12
#define I_RB4	13
#define I_RB5	14
#define I_TC1   15
#define I_TC2   16
#define I_TC3   17
#define I_TC4   18
#define I_TC5   19
#define I_TC6   20
#define I_TC7   21
#define I_TC8   22
#define I_TC9   23
#define I_TCA   24
#define I_TC0   25
#define I_TCT   26
#define I_D01	27
#define I_D02	28
#define I_D03	29
#define I_D04	30
#define I_D05	31
#define I_D06	32
#define I_D07	33
#define I_D08	34
#define I_D09	35
#define I_D10	36
#define I_D11	37
#define I_D12	38
#define I_D13	39
#define I_D14	40
#define I_D15	41
#define I_D16	42
#define I_D17	43
#define I_D18	44
#define I_D19	45
#define I_D20	46


cboard_4::cboard_4(void)
{
      proc=P18F452;
      
      vtc =0;
      vt=0;
      
      vp1in=2.5;
      vp2in=2.5;
      vp2[0]=2.5;
      vp2[1]=2.5;
      temp[0]=27.5;
      temp[1]=27.5;
      ref=0;
      
      rpmstp=0;//(NSTEP*2)/100;
      rpmc=0;


      d=0;
      lcde=0;
      sound_on=0;
      
      buzz.Create(Window1.GetSharePath()+wxT("buzzer.wav"));
      
     wxImage  image;
     image.LoadFile(Window1.GetSharePath()+wxT("VT1.png"));
     vent[0] = new wxBitmap(image);
     image.LoadFile(Window1.GetSharePath()+wxT("VT2.png"));
     vent[1] = new wxBitmap(image);
     
      
     mi2c_init(&mi2c,4);
     rtc2_init(&rtc2);
      
     ReadMaps();
     
     for(int i=0;i<20;i++)
        dip[i]=1;
  
  //scroll1
  scroll1=new CScroll();   
  scroll1->SetFOwner(&Window1);
  scroll1->SetName(wxT("scroll1_p4"));
  scroll1->SetX(12);
  scroll1->SetY(273);
  scroll1->SetWidth(140);
  scroll1->SetHeight(22);
  scroll1->SetEnable(1);
  scroll1->SetVisible(1);
  scroll1->SetRange(100);
  scroll1->SetPosition(50);
  scroll1->SetType(4);
  Window1.CreateChild(scroll1);
  //scroll2
  scroll2=new CScroll();
  scroll2->SetFOwner(&Window1);
  scroll2->SetName(wxT("scroll2_p4"));
  scroll2->SetX(12);
  scroll2->SetY(221);
  scroll2->SetWidth(140);
  scroll2->SetHeight(22);
  scroll2->SetEnable(1);
  scroll2->SetVisible(1);
  scroll2->SetRange(100);
  scroll2->SetPosition(50);
  scroll2->SetType(4);
  Window1.CreateChild(scroll2);
  //gauge1
  gauge1=new CGauge();
  gauge1->SetFOwner(&Window1);
  gauge1->SetName(wxT("gauge1_p4"));
  gauge1->SetX(13);
  gauge1->SetY(382);
  gauge1->SetWidth(140);
  gauge1->SetHeight(20);
  gauge1->SetEnable(1);
  gauge1->SetVisible(1);
  gauge1->SetRange(100);
  gauge1->SetValue(0);
  gauge1->SetType(4);
  Window1.CreateChild(gauge1);
  //gauge2
  gauge2=new CGauge();
  gauge2->SetFOwner(&Window1);
  gauge2->SetName(wxT("gauge2_p4"));
  gauge2->SetX(12);
  gauge2->SetY(330);
  gauge2->SetWidth(140);
  gauge2->SetHeight(20);
  gauge2->SetEnable(1);
  gauge2->SetVisible(1);
  gauge2->SetRange(100);
  gauge2->SetValue(0);
  gauge2->SetType(4);
  Window1.CreateChild(gauge2);   
  //label1
  label1=new CLabel();
  label1->SetFOwner(&Window1);
  label1->SetName(wxT("label1_p4"));
  label1->SetX(12);
  label1->SetY(197);
  label1->SetWidth(60);
  label1->SetHeight(20);
  label1->SetEnable(1);
  label1->SetVisible(1);
  label1->SetText(wxT("Pot. P1"));
  label1->SetAlign(1);
  Window1.CreateChild(label1); 
  //label2
  label2=new CLabel();
  label2->SetFOwner(&Window1);
  label2->SetName(wxT("label2_p4"));
  label2->SetX(12);
  label2->SetY(249);
  label2->SetWidth(60);
  label2->SetHeight(20);
  label2->SetEnable(1);
  label2->SetVisible(1);
  label2->SetText(wxT("Pot. P2"));
  label2->SetAlign(1);
  Window1.CreateChild(label2);    
  //label3
  label3=new CLabel();
  label3->SetFOwner(&Window1);
  label3->SetName(wxT("label3_p4"));
  label3->SetX(12);
  label3->SetY(306);
  label3->SetWidth(60);
  label3->SetHeight(20);
  label3->SetEnable(1);
  label3->SetVisible(1);
  label3->SetText(wxT("Heater"));
  label3->SetAlign(1);
  Window1.CreateChild(label3);    
  //label4
  label4=new CLabel();
  label4->SetFOwner(&Window1);
  label4->SetName(wxT("label4_p4"));
  label4->SetX(13);
  label4->SetY(357);
  label4->SetWidth(60);
  label4->SetHeight(20);
  label4->SetEnable(1);
  label4->SetVisible(1);
  label4->SetText(wxT("Cooler"));
  label4->SetAlign(1);
  Window1.CreateChild(label4);
  //label5
  label5=new CLabel();
  label5->SetFOwner(&Window1);
  label5->SetName(wxT("label5_p4"));
  label5->SetX(13);
  label5->SetY(412);
  label5->SetWidth(120);
  label5->SetHeight(24);
  label5->SetEnable(1);
  label5->SetVisible(1);
  label5->SetText(wxT("Temp: 00.0°C"));
  label5->SetAlign(1);
  Window1.CreateChild(label5);
     
};

cboard_4::~cboard_4(void)
{
      buzz.Stop();
      
      delete vent[0];
      delete vent[1];
      vent[0]=NULL; 
      vent[1]=NULL; 
      
      mi2c_end(&mi2c);
      rtc2_end(&rtc2);
      
      Window1.DestroyChild(scroll1);
      Window1.DestroyChild(scroll2);
      Window1.DestroyChild(gauge1);
      Window1.DestroyChild(gauge2);
      Window1.DestroyChild(label1);
      Window1.DestroyChild(label2);
      Window1.DestroyChild(label3);
      Window1.DestroyChild(label4);
      Window1.DestroyChild(label5);
}

void cboard_4::Draw(_pic *pic, CDraw *draw,double scale)
{
  int i;
  int j;
  unsigned char pi;
  unsigned char pinv;
  const picpin * pins;
 
  unsigned char L[4];
  
  int JUMPSTEPS = Window1.GetJUMPSTEPS();
  long int NSTEPJ=Window1.GetNSTEPJ();

 
          pic_set_pin(pic,39,1);
          pic_set_pin(pic,40,1);
          
          pic_set_pin(pic,19,1);
          pic_set_pin(pic,20,1);
          pic_set_pin(pic,21,1);
          pic_set_pin(pic,22,1);
          pic_set_pin(pic,27,1);
          pic_set_pin(pic,28,1);
          pic_set_pin(pic,29,1);
          pic_set_pin(pic,30,1);
 

  
  draw->Canvas.Init(scale,scale);
          

  lcd_blink(&lcd);


//lab4 draw 
  for(i=0;i<outputc;i++)
  {
    if(!output[i].r)
    {
       
      draw->Canvas.SetFgColor (30, 0, 0);
      
      switch(output[i].id) 
      {
        case O_A1: draw->Canvas.SetColor (0, lm1[18], 0);break;
        case O_B1: draw->Canvas.SetColor (0, lm1[19], 0);break;
        case O_C1: draw->Canvas.SetColor (0, lm1[20], 0);break;
        case O_D1: draw->Canvas.SetColor (0, lm1[21], 0);break;
        case O_E1: draw->Canvas.SetColor (0, lm1[26], 0);break;
        case O_F1: draw->Canvas.SetColor (0, lm1[27], 0);break;
        case O_G1: draw->Canvas.SetColor (0, lm1[28], 0);break;
        case O_P1: draw->Canvas.SetColor (0, lm1[29], 0);break;
          
        case O_A2: draw->Canvas.SetColor (0, lm2[18], 0);break;
        case O_B2: draw->Canvas.SetColor (0, lm2[19], 0);break;
        case O_C2: draw->Canvas.SetColor (0, lm2[20], 0);break;
        case O_D2: draw->Canvas.SetColor (0, lm2[21], 0);break;
        case O_E2: draw->Canvas.SetColor (0, lm2[26], 0);break;
        case O_F2: draw->Canvas.SetColor (0, lm2[27], 0);break;
        case O_G2: draw->Canvas.SetColor (0, lm2[28], 0);break;
        case O_P2: draw->Canvas.SetColor (0, lm2[29], 0);break;
      
        case O_A3: draw->Canvas.SetColor (0, lm3[18], 0);break;
        case O_B3: draw->Canvas.SetColor (0, lm3[19], 0);break;
        case O_C3: draw->Canvas.SetColor (0, lm3[20], 0);break;
        case O_D3: draw->Canvas.SetColor (0, lm3[21], 0);break;
        case O_E3: draw->Canvas.SetColor (0, lm3[26], 0);break;
        case O_F3: draw->Canvas.SetColor (0, lm3[27], 0);break;
        case O_G3: draw->Canvas.SetColor (0, lm3[28], 0);break;
        case O_P3: draw->Canvas.SetColor (0, lm3[29], 0);break;
      
        case O_A4: draw->Canvas.SetColor (0, lm4[18], 0);break;
        case O_B4: draw->Canvas.SetColor (0, lm4[19], 0);break;
        case O_C4: draw->Canvas.SetColor (0, lm4[20], 0);break;
        case O_D4: draw->Canvas.SetColor (0, lm4[21], 0);break;
        case O_E4: draw->Canvas.SetColor (0, lm4[26], 0);break;
        case O_F4: draw->Canvas.SetColor (0, lm4[27], 0);break;
        case O_G4: draw->Canvas.SetColor (0, lm4[28], 0);break;
        case O_P4: draw->Canvas.SetColor (0, lm4[29], 0);break;
     
        case O_RL1:
                 if(dip[3])
                    draw->Canvas.SetColor (0,lm[14], 0);
                 else
                    draw->Canvas.SetColor (0,15, 0);
                 break;
        case O_RL2: 
                 if(dip[4])
                    draw->Canvas.SetColor (0,lm[7] , 0);
                 else                  
                    draw->Canvas.SetColor (0,15, 0);
                 break;
 

        case O_LCD: draw->Canvas.SetColor (0, 90*Window1.Get_picpwr()+40, 0);break;

        default : 
           if((output[i].name[0]=='D')&&(output[i].name[1]=='P') )
           {
             draw->Canvas.SetBgColor (250, 250, 250);break;
           };
      }
     
      if((output[i].id != O_LCD)&&(output[i].id != O_VT))  
        draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );


//draw cooler 
      if(output[i].id == O_VT)
      { 
        if(gauge1->GetValue() > 20) vtc++;
        
        if(vtc > (4  - 0.04*gauge1->GetValue()) )
        {
          vtc=0;
          draw->Canvas.PutBitmap(vent[vt],output[i].x1, output[i].y1);
          vt^=1; 
        }    
        
      }
      
//draw lcd text 
      if(dip[0]) 
      {
        if((output[i].id == O_LCD)&&(lcd.update))
        {  
           lcd_draw(&lcd,draw,output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1,Window1.Get_picpwr());          
        } 
      }
      else if(output[i].id == O_LCD)  
      {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
      } 
       
      if((output[i].name[0]=='D')&&(output[i].name[1]=='P'))
      {
       if(!dip[(((output[i].name[2]-0x30)*10)+(output[i].name[3]-0x30))-1 ])
       {
         draw->Canvas.SetBgColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1,output[i].y1+((int)((output[i].y2-output[i].y1)*0.35)),output[i].x2-output[i].x1 , (int)((output[i].y2-output[i].y1)*0.65) );
         //draw->Canvas.SetColor (220, 220, 0);
         //draw->Canvas.Circle (1,output[i].x1+(int)((output[i].x2-output[i].x1)*0.20), output[i].y1+((output[i].y2-output[i].y1)/2),3);
       }
       else
       {
         draw->Canvas.SetBgColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1,output[i].x2-output[i].x1 , (int)((output[i].y2-output[i].y1)*0.65));
         //draw->Canvas.SetColor (220, 220, 0);
         //draw->Canvas.Circle (1,output[i].x1+(int)((output[i].x2-output[i].x1)*0.80), output[i].y1+((output[i].y2-output[i].y1)/2),3);
       };
      }       
        
    }
    else
    {
      draw->Canvas.SetFgColor (0, 0, 0);
      
          
        draw->Canvas.SetColor (25, 15, 15);
 
        if(dip[9])
        switch(output[i].id)
        {
          case O_RB0: draw->Canvas.SetColor (lm[32], 0, 0);break;
          case O_RB1: draw->Canvas.SetColor (lm[33], 0, 0);break;
          case O_RB2: draw->Canvas.SetColor (lm[34], 0, 0);break;
          case O_RB3: draw->Canvas.SetColor (lm[35], 0, 0);break;
          case O_RB4: draw->Canvas.SetColor (lm[36], 0, 0);break;
          case O_RB5: draw->Canvas.SetColor (lm[37], 0, 0);break;
          case O_RB6: draw->Canvas.SetColor (lm[38], 0, 0);break;
          case O_RB7: draw->Canvas.SetColor (lm[39], 0, 0);break;
        }
         

        if(dip[8])
        switch(output[i].id)
        {
          case O_RD0: draw->Canvas.SetColor (lm[18], 0, 0);break;
          case O_RD1: draw->Canvas.SetColor (lm[19], 0, 0);break;
          case O_RD2: draw->Canvas.SetColor (lm[20], 0, 0);break;
          case O_RD3: draw->Canvas.SetColor (lm[21], 0, 0);break;
          case O_RD4: draw->Canvas.SetColor (lm[26], 0, 0);break;
          case O_RD5: draw->Canvas.SetColor (lm[27], 0, 0);break;
          case O_RD6: draw->Canvas.SetColor (lm[28], 0, 0);break;
          case O_RD7: draw->Canvas.SetColor (lm[29], 0, 0);break;
        }
          
      if(output[i].id == O_LPWR)draw->Canvas.SetColor (0,255*Window1.Get_picpwr(), 0);
      
      draw->Canvas.Circle (1,output[i].x1, output[i].y1,output[i].r );
    };    

  };
  rtc2_update(&rtc2);
//end draw

  draw->Canvas.End();
  draw->Update ();
  
     for(pi=0;pi < pic->PINCOUNT;pi++)
     {
       lm[pi]=0;
       lm1[pi]=0;
       lm2[pi]=0;
       lm3[pi]=0;
       lm4[pi]=0;
     };

 pins = pic->pins;


  j=JUMPSTEPS+1;

 if(Window1.Get_picpwr())
   for(i=0;i<Window1.GetNSTEP();i++)
      {

        if(j > JUMPSTEPS)
          {     
          pic_set_pin(pic,33,p_BT1); 
          pic_set_pin(pic,34,p_BT2); 
          pic_set_pin(pic,35,p_BT3); 
          pic_set_pin(pic,36,p_BT4);
          pic_set_pin(pic,37,p_BT5);
          pic_set_pin(pic,38,p_BT6);
          pic_set_pin(pic,7,p_BT7);
          
          pic_set_pin(pic,39,1);
          pic_set_pin(pic,40,1);
          
          /*
          pic_set_pin(pic,19,1);
          pic_set_pin(pic,20,1);
          pic_set_pin(pic,21,1);
          pic_set_pin(pic,22,1);
          pic_set_pin(pic,27,1);
          pic_set_pin(pic,28,1);
          pic_set_pin(pic,29,1);
          pic_set_pin(pic,30,1);
          */
          L[0]=!pic_get_pin(pic,33);
          L[1]=!pic_get_pin(pic,34);
          L[2]=!pic_get_pin(pic,35);



          if(((L[0])&&(p_CL1 == 1))||((L[1])&&(p_CL2 == 1))||((L[2])&&(p_CL3 == 1)))
          {
             pic_set_pin(pic,22,0); 
          }
          else
          {
             pic_set_pin(pic,22,1);
          }

          if(((L[0])&&(p_CL1 == 2))||((L[1])&&(p_CL2 == 2))||((L[2])&&(p_CL3 == 2)))
          {
             pic_set_pin(pic,21,0); 
          }
          else
          {
             pic_set_pin(pic,21,1);
          }

          if(((L[0])&&(p_CL1 == 3))||((L[1])&&(p_CL2 == 3))||((L[2])&&(p_CL3 == 3)))
          {
             pic_set_pin(pic,20,0); 
          }
          else
          {
             pic_set_pin(pic,20,1);
          }

          if(((L[0])&&(p_CL1 == 4))||((L[1])&&(p_CL2 == 4))||((L[2])&&(p_CL3 == 4)))
          {
             pic_set_pin(pic,19,0); 
          }
          else
          {
             pic_set_pin(pic,19,1);
          }

                          


          rpmc++;
          if(rpmc > rpmstp) 
          {
             rpmc=0;
             if(dip[14]) 
                 pic_set_pin(pic,15, !pic_get_pin(pic,15));
          }

        }    

        if(!mplabxd_testbp(pic))pic_step(pic,0);
/*
        for(pi=0;pi < pic.PINCOUNT;pi++)
        {
           lm[pi]+=pins[pi].value;
           //if((!pins[pi].dir)&&(pins[pi].value)) lm[pi]++;
        }
*/

        if(j > JUMPSTEPS)
        {       
//outputs     
          lm[32]+=pins[32].value;
          lm[33]+=pins[33].value;
          lm[34]+=pins[34].value;
          lm[35]+=pins[35].value;
          lm[36]+=pins[36].value;
          lm[37]+=pins[37].value;
          lm[38]+=pins[38].value;
          lm[39]+=pins[39].value;

          lm[18]+=pins[18].value;
          lm[19]+=pins[19].value;
          lm[20]+=pins[20].value;
          lm[21]+=pins[21].value;
          lm[26]+=pins[26].value;
          lm[27]+=pins[27].value;
          lm[28]+=pins[28].value;
          lm[29]+=pins[29].value;
        
          lm[23]+=pins[23].value;
          lm[16]+=pins[16].value;
          lm[14]+=pins[14].value;
          lm[7]+=pins[7].value;
          lm[15]+=pins[15].value;

//inputs
          if((pins[32].dir)&&(p_BT1))lm[32]++; 
          if((pins[33].dir)&&(p_BT2))lm[33]++; 
          if((pins[34].dir)&&(p_BT3))lm[34]++; 
          if((pins[35].dir)&&(p_BT4))lm[35]++; 

          if((pins[36].dir)&&(p_BT5))lm[36]++; 
          if((pins[37].dir)&&(p_BT6))lm[37]++; 
          
          if((pins[38].dir))lm[38]++; 
          if((pins[39].dir))lm[39]++; 

          if((pins[18].dir))lm[18]++; 
          if((pins[19].dir))lm[19]++; 
          if((pins[20].dir))lm[20]++; 
          if((pins[21].dir))lm[21]++; 
          if((pins[26].dir))lm[26]++; 
          if((pins[27].dir))lm[27]++; 
          if((pins[28].dir))lm[28]++; 
          if((pins[29].dir))lm[29]++; 

            for(pi=18;pi<30;pi++)
            {
              pinv=pins[pi].value;
              if((pinv)&&(pins[3].value)&&(dip[10])) lm1[pi]++;
              if((pinv)&&(pins[4].value)&&(dip[11])) lm2[pi]++;
              if((pinv)&&(pins[5].value)&&(dip[12])) lm3[pi]++;
              if((pinv)&&(pins[6].value)&&(dip[13])) lm4[pi]++;
            }

	if(dip[7])lm[32]=0;

    
// potenciometro p1 e p2
      if(dip[18])pic_set_apin(pic,2,vp1in);
      if(dip[19])pic_set_apin(pic,3,vp2in);


    j=0;
    }
    j++;

//lcd dipins[2].display code

    if((!pins[8].dir)&&(!pins[8].value))
    {
      if(!lcde)
      {
      d=0; 
      if(pins[29].value) d|= 0x80;
      if(pins[28].value) d|= 0x40;
      if(pins[27].value) d|= 0x20;
      if(pins[26].value) d|= 0x10;
      if(pins[21].value) d|= 0x08;
      if(pins[20].value) d|= 0x04;
      if(pins[19].value) d|= 0x02;
      if(pins[18].value) d|= 0x01;

      if((!pins[9].dir)&&(!pins[9].value))
      {
        lcd_cmd(&lcd,d);
      }
      else if((!pins[9].dir)&&(pins[9].value))
      {
        lcd_data(&lcd,d);
      }
      lcde=1;
      }  
    }
    else
    {
      lcde=0;
    }
//end display code     
 
//i2c code
        if(pins[22].dir)
        {
          sda=1;
        }
        else
        {
          sda=pins[22].value;
        }
        
        if(pins[17].dir)
        {
          sck=1;
	  pic_set_pin(pic,18,1);
        }
        else
        {
          sck=pins[17].value;
        }
	pic_set_pin(pic,23,mi2c_io(&mi2c,sck,sda) | rtc2_io(&rtc2,sck,sda));
     
     }

   //fim STEP


     if( ((100.0*lm[15])/NSTEPJ) > 40)
     {
       if(!sound_on)
       {
         buzz.Play(wxSOUND_ASYNC|wxSOUND_LOOP); 
         sound_on=1;
       }
     }
     else
     {
       buzz.Stop(); 
       sound_on=0;
     }

     if(!dip[15])lm[23]=0;//aquecedor
     if(!dip[17])lm[16]=0;//ventilador


     //Ventilador
     gauge1->SetValue((100.0*lm[16])/NSTEPJ); 
     //Aquecedor
     gauge2->SetValue((100.0*lm[23])/NSTEPJ); 

     //sensor ventilador
     rpmstp=((float)NSTEPJ*NSTEPJ)/(144.0*(lm[16]+1));
   
     //tensão p2
     vp2in=((5.0*(scroll1->GetPosition()))/(scroll1->GetRange()-1));
     vp1in=((5.0*(scroll2->GetPosition()))/(scroll2->GetRange()-1));

     //temperatura 
     ref=((50.0*lm[23])/NSTEPJ)-((50.0*lm[16])/NSTEPJ); 

     if(ref < 0)
       ref=0; 
 
       temp[1]=temp[0];    
       temp[0]=((27.5+ref)*0.003)+temp[1]*(0.997);

     if(dip[16])
       pic_set_apin(pic,4,temp[0]/100.0);
     else
       pic_set_apin(pic,4,0);

     //referencia
     //pic_set_apin(pic,5,2.5);

     for(pi=0;pi < pic->PINCOUNT;pi++)
     { 
      lm[pi]= (int)(((225.0*lm[pi])/NSTEPJ)+30);

      lm1[pi]= (int)(((600.0*lm1[pi])/NSTEPJ)+30);
      lm2[pi]= (int)(((600.0*lm2[pi])/NSTEPJ)+30);
      lm3[pi]= (int)(((600.0*lm3[pi])/NSTEPJ)+30);
      lm4[pi]= (int)(((600.0*lm4[pi])/NSTEPJ)+30);
      if(lm1[pi] > 255)lm1[pi]=255;
      if(lm2[pi] > 255)lm2[pi]=255;
      if(lm3[pi] > 255)lm3[pi]=255;
      if(lm4[pi] > 255)lm4[pi]=255;
     }
  
 
};

void 
cboard_4::Reset(_pic *pic)
{
          
   lcd_rst(&lcd);   
   mi2c_rst(&mi2c);
   rtc2_rst(&rtc2);
  
    p_BT1=1; 
    p_BT2=1; 
    p_BT3=1; 
    p_BT4=1; 
    p_BT5=1; 
    p_BT6=1; 
    p_BT7=1; 
  
    pic_set_pin(pic,33,p_BT1); 
    pic_set_pin(pic,34,p_BT2); 
    pic_set_pin(pic,35,p_BT3); 
    pic_set_pin(pic,36,p_BT4); 
    pic_set_pin(pic,37,p_BT5); 
    pic_set_pin(pic,38,p_BT6); 
    pic_set_pin(pic,7,p_BT7); 
    p_CL1=0; 
    p_CL2=0; 
    p_CL3=0; 
    pic_set_pin(pic,33,p_CL1); 
    pic_set_pin(pic,34,p_CL2); 
    pic_set_pin(pic,35,p_CL3); 
    
#ifndef _WIN_
    if(pic->serialfd > 0)
#else
    if(pic->serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(":")+itoa(pic->serialbaud)+wxT("(")+String().Format("%4.1f",fabs((100.0*pic->serialexbaud-100.0*pic->serialbaud)/pic->serialexbaud))+wxT("%)"));
    else  
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(" (ERROR)"));
        
};

void 
cboard_4::MouseButtonPress(_pic *pic, uint button, uint x, uint y,uint state)
{
 
  int i;
    
  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
   
      switch(input[i].id) 
      { 
      case I_ICSP:
      {
         Window1.menu1_File_LoadHex_EvMenuActive(NULL);
      };break;  
      

      case I_D01:
      {
        dip[0]^=0x01; 
      };break;
      case I_D02:
      {
        dip[1]^=0x01; 
      };break;
      case I_D03:
      {
        dip[2]^=0x01; 
      };break;
      case I_D04:
      {
        dip[3]^=0x01; 
      };break;
      case I_D05:
      {
        dip[4]^=0x01; 
      };break;
      case I_D06:
      {
        dip[5]^=0x01; 
      };break;
      case I_D07:
      {
        dip[6]^=0x01; 
      };break;
      case I_D08:
      {
        dip[7]^=0x01; 
      };break;
      case I_D09:
      {
        dip[8]^=0x01; 
      };break;
      case I_D10:
      {
        dip[9]^=0x01; 
      };break;
      case I_D11:
      {
        dip[10]^=0x01; 
      };break;
      case I_D12:
      {
        dip[11]^=0x01; 
      };break;
      case I_D13:
      {
        dip[12]^=0x01; 
      };break;
      case I_D14:
      {
        dip[13]^=0x01; 
      };break;
      case I_D15:
      {
        dip[14]^=0x01; 
      };break;
      case I_D16:
      {
        dip[15]^=0x01; 
      };break;
      case I_D17:
      {
        dip[16]^=0x01; 
      };break;
      case I_D18:
      {
        dip[17]^=0x01; 
      };break;
      case I_D19:
      {
        dip[18]^=0x01; 
      };break;
      case I_D20:
      {
        dip[19]^=0x01; 
      };break;
      
      case I_PWR:
      {
        if(Window1.Get_picpwr()) 
        { 
          Window1.Set_picrun(0); 
          Window1.Set_picpwr(0); 
          pic_reset(pic,1);
          Reset(pic);

          p_BT1=0; 
          p_BT2=0; 
          p_BT3=0; 
          p_BT4=0; 
          Window1.statusbar1.SetField(0,wxT("Stoped"));
        }
        else
        {
          Window1.Set_picpwr(1);
          Window1.Set_picrun(1);
          pic_reset(pic,1);  
	
          Reset(pic);
     
          
          Window1.statusbar1.SetField(0,wxT("Running..."));
        } 
      };break;
      
      case I_RST:
      {
          if(Window1.Get_picpwr())
          {
             Window1.Set_picpwr(0);
             Window1.Set_picrst(1);
          } 
      };break;
      
      case I_RA1:
        {
          p_BT1=0;
        };break;
      case I_RA2:
        {
          p_BT2=0; 
        };break;
      case I_RA3:
        {
          p_BT3=0; 
        };break;
      case I_RA4:
        {
          p_BT4=0; 
        };break;
      case I_RA5:
        {
          p_BT7=0; 
        };break;
      case I_RB0:
        {
          p_BT1=0;
        };break;
      case I_RB1:
        {
          p_BT2=0; 
        };break;
      case I_RB2:
        {
          p_BT3=0; 
        };break;
      case I_RB3:
        {
          p_BT4=0; 
        };break;
      case I_RB4:
        {
          p_BT5=0; 
        };break;
      case I_RB5:
        {
          p_BT6=0; 
        };break;

      case I_TC1:
        {
          p_CL1=1;
        };break;
      case I_TC2:
        {
          p_CL2=1; 
        };break;
      case I_TC3:
        {
          p_CL3=1; 
        };break;

      case I_TC4:
        {
          p_CL1=2; 
        };break;
      case I_TC5:
        {
          p_CL2=2; 
        };break;
      case I_TC6:
        {
          p_CL3=2; 
        };break;

      case I_TC7:
        {
          p_CL1=3; 
        };break;
      case I_TC8:
        {
          p_CL2=3; 
        };break;
      case I_TC9:
        {
          p_CL3=3; 
        };break;
      
      case I_TCA:
        {
          p_CL1=4; 
        };break;
      case I_TC0:
        {
          p_CL2=4; 
        };break;
      case I_TCT:
        {
          p_CL3=4; 
        };break;

      }
    }
  }
    
    
};


void 
cboard_4::MouseButtonRelease(_pic *pic, uint button, uint x, uint y,uint state)
{    
  int i;

  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
      switch(input[i].id)
      {
      case I_RST:
      {
          if(Window1.Get_picrst())
          {
            Window1.Set_picpwr(1);
            Window1.Set_picrst(0);

            if(pic_reset(pic,-1))
            { 
              lcd_rst(&lcd);
	      mi2c_rst(&mi2c);
	      rtc2_rst(&rtc2);
              
              Reset(pic);
        
            }
          } 
      };break;
      
        case I_RA1:
        {
          p_BT1=1; 
        };break;
        case I_RA2:
        {
          p_BT2=1; 
        };break;
        case I_RA3:
        {
          p_BT3=1; 
        };break;
        case I_RA4:
        {
          p_BT4=1; 
        };break;
        case I_RA5:
        {
          p_BT7=1; 
        };break;
        case I_RB0:
        {
          p_BT1=1; 
        };break;
        case I_RB1:
        {
          p_BT2=1; 
        };break;
        case I_RB2:
        {
          p_BT3=1; 
        };break;
        case I_RB3:
        {
          p_BT4=1; 
        };break;
        case I_RB4:
        {
          p_BT5=1; 
        };break;
        case I_RB5:
        {
          p_BT6=1; 
        };break;

        case I_TC1:
        case I_TC4:
        case I_TC7:
        case I_TCA:
        {
          p_CL1=0; 
        };break;

        case I_TC2:
        case I_TC5:
        case I_TC8:
        case I_TC0:
        {
          p_CL2=0; 
        };break;

        case I_TC3:
        case I_TC6:
        case I_TC9:
        case I_TCT:
        {
          p_CL3=0; 
        };break;
      }
    } 
  }

};



void 
cboard_4::KeyPress(_pic *pic, uint key, uint x, uint y,uint mask)
{
  if(key == '1')
  {
    p_CL1=1; 
  }
  if(key == '2')
  {
    p_CL2=1; 
  }
  if(key == '3')
  {
    p_CL3=1; 
  }

  if(key == '4')
  {
    p_CL1=2; 
  }
  if(key == '5')
  {
    p_CL2=2; 
  }
  if(key == '6')
  {
    p_CL3=2; 
  }

  if(key == '7')
  {
    p_CL1=3; 
  }
  if(key == '8')
  {
    p_CL2=3; 
  }
  if(key == '9')
  {
    p_CL3=3; 
  }

  if(key == '*')
  {
    p_CL1=4; 
  }
  if(key == '0')
  {
    p_CL2=4; 
  }
  if(key == '#')
  {
    p_CL3=4; 
  }

};

void
cboard_4::KeyRelease(_pic *pic, uint key, uint x, uint y,uint mask)
{
  if((key == '1')||(key == '4')||(key == '7')||(key == '*'))
  {
    p_CL1=0; 
  }
  if((key == '2')||(key == '5')||(key == '8')||(key == '0'))
  {
    p_CL2=0; 
  }
  if((key == '3')||(key == '6')||(key == '9')||(key == '#'))
  {
    p_CL3=0; 
  }
};

void
cboard_4::OnShow(void)
{
    lcd.update=1;
}

unsigned short 
cboard_4::get_in_id(char * name)
{
  if(strcmp(name,"RST")==0)return I_RST; 
  if(strcmp(name,"RA1")==0)return I_RA1;
  if(strcmp(name,"RA2")==0)return I_RA2;
  if(strcmp(name,"RA3")==0)return I_RA3;
  if(strcmp(name,"RA4")==0)return I_RA4;
  if(strcmp(name,"RA5")==0)return I_RA5;
  if(strcmp(name,"PWR")==0)return I_PWR;
  if(strcmp(name,"ICSP")==0)return I_ICSP;

  if(strcmp(name,"RB0")==0)return I_RB0;
  if(strcmp(name,"RB1")==0)return I_RB1;
  if(strcmp(name,"RB2")==0)return I_RB2;
  if(strcmp(name,"RB3")==0)return I_RB3;
  if(strcmp(name,"RB4")==0)return I_RB4;
  if(strcmp(name,"RB5")==0)return I_RB5;
  
  if(strcmp(name,"TC1")==0)return I_TC1;
  if(strcmp(name,"TC2")==0)return I_TC2;
  if(strcmp(name,"TC3")==0)return I_TC3;
  if(strcmp(name,"TC4")==0)return I_TC4;
  if(strcmp(name,"TC5")==0)return I_TC5;
  if(strcmp(name,"TC6")==0)return I_TC6;
  if(strcmp(name,"TC7")==0)return I_TC7;
  if(strcmp(name,"TC8")==0)return I_TC8;
  if(strcmp(name,"TC9")==0)return I_TC9;
  if(strcmp(name,"TC*")==0)return I_TCA;
  if(strcmp(name,"TC0")==0)return I_TC0;
  if(strcmp(name,"TC#")==0)return I_TCT;
  
  if(strcmp(name,"DP01")==0)return I_D01;
  if(strcmp(name,"DP02")==0)return I_D02;
  if(strcmp(name,"DP03")==0)return I_D03;
  if(strcmp(name,"DP04")==0)return I_D04;
  if(strcmp(name,"DP05")==0)return I_D05;
  if(strcmp(name,"DP06")==0)return I_D06;
  if(strcmp(name,"DP07")==0)return I_D07;
  if(strcmp(name,"DP08")==0)return I_D08;
  if(strcmp(name,"DP09")==0)return I_D09;
  if(strcmp(name,"DP10")==0)return I_D10;
  if(strcmp(name,"DP11")==0)return I_D11;
  if(strcmp(name,"DP12")==0)return I_D12;
  if(strcmp(name,"DP13")==0)return I_D13;
  if(strcmp(name,"DP14")==0)return I_D14;
  if(strcmp(name,"DP15")==0)return I_D15;
  if(strcmp(name,"DP16")==0)return I_D16;
  if(strcmp(name,"DP17")==0)return I_D17;
  if(strcmp(name,"DP18")==0)return I_D18;
  if(strcmp(name,"DP19")==0)return I_D19;
  if(strcmp(name,"DP20")==0)return I_D20;

  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cboard_4::get_out_id(char * name)
{

  if(strcmp(name,"RB0")==0)return O_RB0;
  if(strcmp(name,"RB1")==0)return O_RB1;
  if(strcmp(name,"RB2")==0)return O_RB2;
  if(strcmp(name,"RB3")==0)return O_RB3;
  if(strcmp(name,"RB4")==0)return O_RB4;
  if(strcmp(name,"RB5")==0)return O_RB5;
  if(strcmp(name,"RB6")==0)return O_RB6;
  if(strcmp(name,"RB7")==0)return O_RB7;
  
  if(strcmp(name,"RD0")==0)return O_RD0;
  if(strcmp(name,"RD1")==0)return O_RD1;
  if(strcmp(name,"RD2")==0)return O_RD2;
  if(strcmp(name,"RD3")==0)return O_RD3;
  if(strcmp(name,"RD4")==0)return O_RD4;
  if(strcmp(name,"RD5")==0)return O_RD5;
  if(strcmp(name,"RD6")==0)return O_RD6;
  if(strcmp(name,"RD7")==0)return O_RD7;
  
  if(strcmp(name,"RL1")==0)return O_RL1;
  if(strcmp(name,"RL2")==0)return O_RL2;
  
  if(strcmp(name,"PRG")==0)return O_PRG;
  if(strcmp(name,"RUN")==0)return O_RUN;

  if(strcmp(name,"A1")==0)return O_A1;	
  if(strcmp(name,"B1")==0)return O_B1;	
  if(strcmp(name,"C1")==0)return O_C1;	
  if(strcmp(name,"D1")==0)return O_D1;	
  if(strcmp(name,"E1")==0)return O_E1;	
  if(strcmp(name,"F1")==0)return O_F1;	
  if(strcmp(name,"G1")==0)return O_G1;	
  if(strcmp(name,"P1")==0)return O_P1;	

  if(strcmp(name,"A2")==0)return O_A2;	
  if(strcmp(name,"B2")==0)return O_B2;	
  if(strcmp(name,"C2")==0)return O_C2;	
  if(strcmp(name,"D2")==0)return O_D2;	
  if(strcmp(name,"E2")==0)return O_E2;	
  if(strcmp(name,"F2")==0)return O_F2;	
  if(strcmp(name,"G2")==0)return O_G2;	
  if(strcmp(name,"P2")==0)return O_P2;	

  if(strcmp(name,"LPWR")==0)return O_LPWR;	
  if(strcmp(name,"LCD")==0)return O_LCD;	

  if(strcmp(name,"A3")==0)return O_A3;	
  if(strcmp(name,"B3")==0)return O_B3;	
  if(strcmp(name,"C3")==0)return O_C3;	
  if(strcmp(name,"D3")==0)return O_D3;	
  if(strcmp(name,"E3")==0)return O_E3;	
  if(strcmp(name,"F3")==0)return O_F3;	
  if(strcmp(name,"G3")==0)return O_G3;	
  if(strcmp(name,"P3")==0)return O_P3;	
  
  if(strcmp(name,"A4")==0)return O_A4;	
  if(strcmp(name,"B4")==0)return O_B4;	
  if(strcmp(name,"C4")==0)return O_C4;	
  if(strcmp(name,"D4")==0)return O_D4;	
  if(strcmp(name,"E4")==0)return O_E4;	
  if(strcmp(name,"F4")==0)return O_F4;	
  if(strcmp(name,"G4")==0)return O_G4;	
  if(strcmp(name,"P4")==0)return O_P4;	
  
  
  if(strcmp(name,"DP01")==0)return O_D01;
  if(strcmp(name,"DP02")==0)return O_D02;
  if(strcmp(name,"DP03")==0)return O_D03;
  if(strcmp(name,"DP04")==0)return O_D04;
  if(strcmp(name,"DP05")==0)return O_D05;
  if(strcmp(name,"DP06")==0)return O_D06;
  if(strcmp(name,"DP07")==0)return O_D07;
  if(strcmp(name,"DP08")==0)return O_D08;
  if(strcmp(name,"DP09")==0)return O_D09;
  if(strcmp(name,"DP10")==0)return O_D10;
  if(strcmp(name,"DP11")==0)return O_D11;
  if(strcmp(name,"DP12")==0)return O_D12;
  if(strcmp(name,"DP13")==0)return O_D13;
  if(strcmp(name,"DP14")==0)return O_D14;
  if(strcmp(name,"DP15")==0)return O_D15;
  if(strcmp(name,"DP16")==0)return O_D16;
  if(strcmp(name,"DP17")==0)return O_D17;
  if(strcmp(name,"DP18")==0)return O_D18;
  if(strcmp(name,"DP19")==0)return O_D19;
  if(strcmp(name,"DP20")==0)return O_D20;
  if(strcmp(name,"VT")==0)  return O_VT;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


void 
cboard_4::RefreshStatus(_pic *pic)
{
    label5->SetText(wxT("Temp: ")+String().Format("%5.2f",temp[0])+wxT("°C"));
    
#ifndef _WIN_
    if(pic->serialfd > 0)
#else
    if(pic->serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(":")+itoa(pic->serialbaud)+wxT("(")+String().Format("%4.1f",fabs((100.0*pic->serialexbaud-100.0*pic->serialbaud)/pic->serialexbaud))+wxT("%)"));
    else  
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(" (ERROR)"));
    
};

void 
cboard_4::WritePreferences(void)
{
    char line[100];
    Window1.saveprefs(wxT("p4_proc"),getnamebyproc(proc,line));


    line[0]=0;
    for(int i=0;i<20;i++)
      sprintf(line+i,"%i",dip[i]);
    
    Window1.saveprefs(wxT("p4_dip"),line);
   
};

void 
cboard_4::ReadPreferences(char *name,char *value)
{
   if(!strcmp(name,"p4_proc"))
   {
      proc=getprocbyname(value); 
    }    
   
  int i;  
      if(!strcmp(name,"p4_dip"))
      {
        for(i=0;i<20;i++) 
        {   
          if(value[i] == '0')      
            dip[i]=0;
          else
            dip[i]=1;
        }
      }
};