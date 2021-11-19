/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020       El-khadraouy Mohammed <mohammed.el-khadraouy@ecole.ensicaen.fr>
   Copyright (c) : 2015-2018  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_Curiosity_HPC_H
#define BOARD_Curiosity_HPC_H

#include<lxrad.h>

#include "bsim_picsim.h"

#define BOARD_Curiosity_HPC_Name "Curiosity HPC"

//New board Class derived from board class defined in board.h
class cboard_Curiosity_HPC:public bsim_picsim
{
    private:
        unsigned char p_BT[2]; // Button S1 and S2
        
        unsigned char pot1;
        unsigned char active;
        
        unsigned char jmp[1];  //jumper

         //controls to be added in simulator window
        CGauge *gauge1;   //gauge to show mean value of RA4
        CGauge *gauge2;   //gauge to show mean value of RA5
        CGauge *gauge3;   //gauge to show mean value of RA6
        CGauge *gauge4;   //gauge to show mean value of RA7
        CLabel *label2;   //label of gauge RA4
        CLabel *label3;   //label of gauge RA5
        CLabel *label4;   //label of gauge RA6
        CLabel *label5;   //label of gauge RA7
        CLabel *label6;   //label of second serial
        CCombo *combo1;  //combo of second serial
        void RegisterRemoteControl(void) override; 
    public:
      //Return the board name
      lxString GetName(void) override {return lxT(BOARD_Curiosity_HPC_Name); };  
      lxString GetAboutInfo(void) override {return lxT("El-khadraouy Mohammed\n<mohammed.el-khadraouy@ecole.ensicaen.fr>\nand L.C. Gamboa \n <lcgamboa@yahoo.com>");};  
      //Constructor called once on board creation 
      cboard_Curiosity_HPC(void);
      //Destructor called once on board destruction 
      ~cboard_Curiosity_HPC(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw) override;
      void Run_CPU(void) override;
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void) override {return lxT("PIC16F1619,PIC18F27K40,");};
      //Return the filename of board picture 
      lxString GetPictureFileName(void) override {return lxT("Curiosity HPC/board.png");};
      //Reset board status
      void Reset(void) override;
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state) override;
      //Event on the board
      void EvMouseMove(uint button, uint x, uint y, uint state) override;
      //Event on the board
      void EvKeyPress(uint key, uint mask) override;
      //Event on the board
      void EvKeyRelease(uint key, uint mask) override;
      //Called ever 1s to refresh status
      void RefreshStatus(void) override;
      //Called to save board preferences in configuration file
      void WritePreferences(void) override;
      //Called whe configuration file load  preferences 
      void ReadPreferences(char *name,char *value) override;
      //return the input ids numbers of names used in input map
      unsigned short get_in_id(char * name) override;
      //return the output ids numbers of names used in output map
      unsigned short get_out_id(char * name) override;
      //Set Serial port name
      void MSetSerial(const char * port) override;
};

#endif  /* BOARD_Curiosity_HPC_H */