/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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

#ifndef PART_SERVO_H
#define	PART_SERVO_H

#include<lxrad.h>
#include"part.h"

    /**
     * @brief servo motor part class
     *
     * class definition of servo motor class.
     */
class cpart_servo:public part
{
   public:
      lxString GetName(void){return lxT("Servo motor");};
      lxString GetHelpURL(void){return lxT("Servo_Motor.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};      
      
     /**
     * @brief constructor called once on part creation
     *
     */
      cpart_servo(unsigned x, unsigned y);
     
     /**
     * @brief destructor called once on part destruction
     *
     */
      ~cpart_servo(void);
      
      void Draw(void);
      void Process(void);
      void PostProcess(void);
      lxString GetPictureFileName(void){return lxT("servo/servo_motor.svg");};
      lxString GetInputMapFile(void){return lxT("servo/servo_motor_i.map");};
      lxString GetOutputMapFile(void){return lxT("servo/servo_motor_o.map");};
      lxString GetPropertiesWindowFile(void){return lxT("servo/servo_motor.lxrad");};
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      void SetOrientation(int orientation);
      void SetScale(double scale);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
    private: 
      void RegisterRemoteControl(void);          
      unsigned char input_pin; ///< pulse input pin  
      lxBitmap * BackGround;   ///< Background image
      float angle;             ///< angle of shaft
      float angle_;            ///< old angle of shaft
      unsigned char in_[2];    ///< input pin memory
      int time;                ///< pulse time
      lxFont font;
};

#endif	/* PART_SERVO */

