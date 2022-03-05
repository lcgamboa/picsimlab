/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2020  Luis Claudio Gambôa Lopes

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

// include files
#include "exp_board_Xpress.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* ids of inputs of input map*/
enum {
  I_POT1, // potentiometer
  I_ICSP, // ICSP connector
  I_PWR,  // Power button
  I_RST,  // Reset button
  I_S1    // S1 push button
};

/* ids of outputs of output map*/
enum {
  O_POT1,
  O_D1,  // LED D1
  O_D2,  // LED D2
  O_D3,  // LED D3
  O_D4,  // LED D4
  O_D5,  // LED D5
  O_RST, // Reset button
  O_S1   // S1 push button
};

// return the input ids numbers of names used in input map

unsigned short cboard_Xpress::get_in_id(char *name) {
  if (strcmp(name, "PG_ICSP") == 0)
    return I_ICSP;
  if (strcmp(name, "SW_PWR") == 0)
    return I_PWR;
  if (strcmp(name, "PB_RST") == 0)
    return I_RST;
  if (strcmp(name, "PB_S1") == 0)
    return I_S1;
  if (strcmp(name, "PO_1") == 0)
    return I_POT1;

  printf("Erro input '%s' don't have a valid id! \n", name);
  return -1;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Xpress::get_out_id(char *name) {
  if (strcmp(name, "LD_D1") == 0)
    return O_D1;
  if (strcmp(name, "LD_D2") == 0)
    return O_D2;
  if (strcmp(name, "LD_D3") == 0)
    return O_D3;
  if (strcmp(name, "LD_D4") == 0)
    return O_D4;
  if (strcmp(name, "LD_D5") == 0)
    return O_D5;

  if (strcmp(name, "PB_S1") == 0)
    return O_S1;
  if (strcmp(name, "PO_1") == 0)
    return O_POT1;
  if (strcmp(name, "PB_RST") == 0)
    return O_RST;

  printf("Erro output '%s' don't have a valid id! \n", name);
  return 1;
}

// Constructor called once on board creation

cboard_Xpress::cboard_Xpress(void) {
  Proc =
      "PIC16F18855"; // default microcontroller if none defined in preferences
  ReadMaps();        // Read input and output board maps

  pot1 = 100;

  active = 0;

  // controls properties and creation
  // gauge1
  gauge1 = new CGauge();
  gauge1->SetFOwner(&Window1);
  gauge1->SetName(lxT("gauge1_p6"));
  gauge1->SetX(48);
  gauge1->SetY(230 - 110);
  gauge1->SetWidth(110);
  gauge1->SetHeight(20);
  gauge1->SetEnable(1);
  gauge1->SetVisible(1);
  gauge1->SetRange(100);
  gauge1->SetValue(0);
  gauge1->SetType(4);
  Window1.CreateChild(gauge1);
  // gauge2
  gauge2 = new CGauge();
  gauge2->SetFOwner(&Window1);
  gauge2->SetName(lxT("gauge2_p6"));
  gauge2->SetX(48);
  gauge2->SetY(255 - 110);
  gauge2->SetWidth(110);
  gauge2->SetHeight(20);
  gauge2->SetEnable(1);
  gauge2->SetVisible(1);
  gauge2->SetRange(100);
  gauge2->SetValue(0);
  gauge2->SetType(4);
  Window1.CreateChild(gauge2);
  // gauge3
  gauge3 = new CGauge();
  gauge3->SetFOwner(&Window1);
  gauge3->SetName(lxT("gauge3_p6"));
  gauge3->SetX(48);
  gauge3->SetY(280 - 110);
  gauge3->SetWidth(110);
  gauge3->SetHeight(20);
  gauge3->SetEnable(1);
  gauge3->SetVisible(1);
  gauge3->SetRange(100);
  gauge3->SetValue(0);
  gauge3->SetType(4);
  Window1.CreateChild(gauge3);
  // gauge4
  gauge4 = new CGauge();
  gauge4->SetFOwner(&Window1);
  gauge4->SetName(lxT("gauge4_p6"));
  gauge4->SetX(48);
  gauge4->SetY(305 - 110);
  gauge4->SetWidth(110);
  gauge4->SetHeight(20);
  gauge4->SetEnable(1);
  gauge4->SetVisible(1);
  gauge4->SetRange(100);
  gauge4->SetValue(0);
  gauge4->SetType(4);
  Window1.CreateChild(gauge4);
  // label2
  label2 = new CLabel();
  label2->SetFOwner(&Window1);
  label2->SetName(lxT("label2_p6"));
  label2->SetX(12);
  label2->SetY(230 - 110);
  label2->SetWidth(60);
  label2->SetHeight(20);
  label2->SetEnable(1);
  label2->SetVisible(1);
  label2->SetText(lxT("RA0"));
  label2->SetAlign(1);
  Window1.CreateChild(label2);
  // label3
  label3 = new CLabel();
  label3->SetFOwner(&Window1);
  label3->SetName(lxT("label3_p6"));
  label3->SetX(13);
  label3->SetY(255 - 110);
  label3->SetWidth(60);
  label3->SetHeight(20);
  label3->SetEnable(1);
  label3->SetVisible(1);
  label3->SetText(lxT("RA1"));
  label3->SetAlign(1);
  Window1.CreateChild(label3);
  // label4
  label4 = new CLabel();
  label4->SetFOwner(&Window1);
  label4->SetName(lxT("label4_p6"));
  label4->SetX(13);
  label4->SetY(280 - 110);
  label4->SetWidth(60);
  label4->SetHeight(20);
  label4->SetEnable(1);
  label4->SetVisible(1);
  label4->SetText(lxT("RA2"));
  label4->SetAlign(1);
  Window1.CreateChild(label4);
  // label5
  label5 = new CLabel();
  label5->SetFOwner(&Window1);
  label5->SetName(lxT("label5_p6"));
  label5->SetX(13);
  label5->SetY(305 - 110);
  label5->SetWidth(60);
  label5->SetHeight(20);
  label5->SetEnable(1);
  label5->SetVisible(1);
  label5->SetText(lxT("RA3"));
  label5->SetAlign(1);
  Window1.CreateChild(label5);
}

// Destructor called once on board destruction

cboard_Xpress::~cboard_Xpress(void) {
  // controls destruction
  Window1.DestroyChild(gauge1);
  Window1.DestroyChild(gauge2);
  Window1.DestroyChild(gauge3);
  Window1.DestroyChild(gauge4);
  Window1.DestroyChild(label2);
  Window1.DestroyChild(label3);
  Window1.DestroyChild(label4);
  Window1.DestroyChild(label5);
}

// Reset board status

void cboard_Xpress::Reset(void) {
  pic.pkg = QFN;
  pic.vcc = 3.3;

  pic_reset(&pic, 1);

  p_BT1 = 1; // set push button  in default state (high)

  // write button state to pic pin 6 (RC4)
  pic_set_pin(&pic, 6, p_BT1);

  // verify serial port state and refresh status bar
#ifndef _WIN_
  if (pic.serial[0].serialfd > 0)
#else
  if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField(
        2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE) + lxT(":") +
               itoa(pic.serial[0].serialbaud) + lxT("(") +
               lxString().Format("%4.1f",
                                 fabs((100.0 * pic.serial[0].serialexbaud -
                                       100.0 * pic.serial[0].serialbaud) /
                                      pic.serial[0].serialexbaud)) +
               lxT("%)"));
  else
    Window1.statusbar1.SetField(2, lxT("Serial: ") +
                                       lxString::FromAscii(SERIALDEVICE) +
                                       lxT(" (ERROR)"));

  if (use_spare)
    Window5.Reset();

  RegisterRemoteControl();
}

void cboard_Xpress::RegisterRemoteControl(void) {
  for (int i = 0; i < inputc; i++) {
    switch (input[i].id) {
    case I_S1:
      input[i].status = &p_BT1;
      break;
    case I_POT1:
      input[i].status = &pot1;
      break;
    }
  }

  for (int i = 0; i < outputc; i++) {
    switch (output[i].id) {
    case O_D2:
      output[i].status = &pic.pins[26].oavalue;
      break;
    case O_D3:
      output[i].status = &pic.pins[27].oavalue;
      break;
    case O_D4:
      output[i].status = &pic.pins[0].oavalue;
      break;
    case O_D5:
      output[i].status = &pic.pins[1].oavalue;
      break;
    }
  }
}

// Called ever 1s to refresh status

void cboard_Xpress::RefreshStatus(void) {
  // verify serial port state and refresh status bar
#ifndef _WIN_
  if (pic.serial[0].serialfd > 0)
#else
  if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField(
        2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE) + lxT(":") +
               itoa(pic.serial[0].serialbaud) + lxT("(") +
               lxString().Format("%4.1f",
                                 fabs((100.0 * pic.serial[0].serialexbaud -
                                       100.0 * pic.serial[0].serialbaud) /
                                      pic.serial[0].serialexbaud)) +
               lxT("%)"));
  else
    Window1.statusbar1.SetField(2, lxT("Serial: ") +
                                       lxString::FromAscii(SERIALDEVICE) +
                                       lxT(" (ERROR)"));
}

// Called to save board preferences in configuration file

void cboard_Xpress::WritePreferences(void) {
  // write selected microcontroller of board_6 to preferences
  Window1.saveprefs(lxT("Xpress_proc"), Proc);
  Window1.saveprefs(lxT("Xpress_clock"),
                    lxString().Format("%2.1f", Window1.GetClock()));
  Window1.saveprefs(lxT("Xpress_pot1"), lxString().Format("%i", pot1));
}

// Called whe configuration file load  preferences

void cboard_Xpress::ReadPreferences(char *name, char *value) {
  // read microcontroller of preferences
  if (!strcmp(name, "Xpress_proc")) {
    Proc = value;
  }
  if (!strcmp(name, "Xpress_clock")) {
    Window1.SetClock(atof(value));
  }
  if (!strcmp(name, "Xpress_pot1")) {
    pot1 = atoi(value);
  }
}

// Event on the board

void cboard_Xpress::EvKeyPress(uint key, uint mask) {
  // if keyboard key 1 is pressed then activate button (state=0)
  if (key == '1') {
    p_BT1 = 0;
  }
}

// Event on the board

void cboard_Xpress::EvKeyRelease(uint key, uint mask) {
  // if keyboard key 1 is pressed then deactivate button (state=1)
  if (key == '1') {
    p_BT1 = 1;
  }
}

// Event on the board

void cboard_Xpress::EvMouseButtonPress(uint button, uint x, uint y,
                                       uint state) {
  int i;

  // search for the input area which owner the event
  for (i = 0; i < inputc; i++) {
    if (((input[i].x1 <= x) && (input[i].x2 >= x)) &&
        ((input[i].y1 <= y) && (input[i].y2 >= y))) {
      switch (input[i].id) {
        // if event is over I_ISCP area then load hex file
      case I_ICSP:
        Window1.menu1_File_LoadHex_EvMenuActive(NULL);
        break;
        // if event is over I_PWR area then toggle board on/off
      case I_PWR:
        if (Window1.Get_mcupwr()) // if on turn off
        {
          Window1.Set_mcupwr(0);
          Reset();
          p_BT1 = 1;
        } else // if off turn on
        {
          Window1.Set_mcupwr(1);
          Reset();
        }
        break;
        // if event is over I_RST area then turn off and reset
      case I_RST:
        if (Window1.Get_mcupwr() && pic_reset(&pic, -1)) // if powered
        {
          Window1.Set_mcupwr(0);
          Window1.Set_mcurst(1);
        }
        p_RST = 0;
        break;
        // if event is over I_S1 area then activate button (state=0)
      case I_S1:
        p_BT1 = 0;
        break;
      case I_POT1: {
        active = 1;
        pot1 = CalcAngle(i, x, y);
      } break;
      }
    }
  }
}

void cboard_Xpress::EvMouseMove(uint button, uint x, uint y, uint state) {
  int i;

  for (i = 0; i < inputc; i++) {
    switch (input[i].id) {
    case I_POT1:
      if (((input[i].x1 <= x) && (input[i].x2 >= x)) &&
          ((input[i].y1 <= y) && (input[i].y2 >= y))) {
        if (active) {
          pot1 = CalcAngle(i, x, y);
        }
      } else {
        active = 0;
      }
      break;
    }
  }
}

// Event on the board

void cboard_Xpress::EvMouseButtonRelease(uint button, uint x, uint y,
                                         uint state) {
  int i;

  // search for the input area which owner the event
  for (i = 0; i < inputc; i++) {
    if (((input[i].x1 <= x) && (input[i].x2 >= x)) &&
        ((input[i].y1 <= y) && (input[i].y2 >= y))) {
      switch (input[i].id) {
        // if event is over I_RST area then turn on
      case I_RST:
        if (Window1.Get_mcurst()) // if powered
        {
          Window1.Set_mcupwr(1);
          Window1.Set_mcurst(0);

          if (pic_reset(&pic, -1)) {
            Reset();
          }
        }
        p_RST = 1;
        break;
        // if event is over I_S1 area then deactivate button (state=1)
      case I_S1:
        p_BT1 = 1;
        break;
      case I_POT1: {
        active = 0;
      } break;
      }
    }
  }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_Xpress::Draw(CDraw *draw) {
  int i;

  draw->Canvas.Init(Scale, Scale); // initialize draw context

  // board_6 draw
  for (i = 0; i < outputc; i++) // run over all outputs
  {
    if (!output[i].r) // if output shape is a rectangle
    {
      draw->Canvas.SetFgColor(0, 0, 0); // black

      switch (output[i].id) // search for color of output
      {
      case O_D1: // green using picpwr value
        draw->Canvas.SetColor(0, 200 * Window1.Get_mcupwr() + 55, 0);
        break;
      case O_D2: // Red using pin 27 mean  value (RA0)
        draw->Canvas.SetColor(pic.pins[26].oavalue, 0, 0);
        break;
      case O_D3: // Red using pin 28 mean  value (RA1)
        draw->Canvas.SetColor(pic.pins[27].oavalue, 0, 0);
        break;
      case O_D4: // Red using pin 1 mean value (RA2)
        draw->Canvas.SetColor(pic.pins[0].oavalue, 0, 0);
        break;
      case O_D5: // Red using pin 2 mean value (RA3)
        draw->Canvas.SetColor(pic.pins[1].oavalue, 0, 0);
        break;
      case O_S1:
      case O_RST:
        draw->Canvas.SetColor(100, 100, 100);
        break;
      case O_POT1:
        draw->Canvas.SetColor(66, 109, 246);
        break;
      }

      // draw a rectangle
      draw->Canvas.Rectangle(1, output[i].x1, output[i].y1,
                             output[i].x2 - output[i].x1,
                             output[i].y2 - output[i].y1);

      if (output[i].id == O_S1) {
        if (p_BT1) {
          draw->Canvas.SetColor(15, 15, 15);
        } else {
          draw->Canvas.SetColor(55, 55, 55);
        }
        draw->Canvas.Circle(1, output[i].cx, output[i].cy, 9);
      } else if (output[i].id == O_RST) {
        if (p_RST) {
          draw->Canvas.SetColor(15, 15, 15);
        } else {
          draw->Canvas.SetColor(55, 55, 55);
        }
        draw->Canvas.Circle(1, output[i].cx, output[i].cy, 9);
      } else if (output[i].id == O_POT1) {
        draw->Canvas.SetColor(26, 69, 206);
        draw->Canvas.Circle(1, output[i].cx, output[i].cy, 20);

        draw->Canvas.SetColor(150, 150, 150);
        int x = -16 * sin((5.585 * (pot1 / 200.0)) + 0.349);
        int y = 16 * cos((5.585 * (pot1 / 200.0)) + 0.349);
        draw->Canvas.Circle(1, output[i].cx + x, output[i].cy + y, 3);
      }
    }
  }

  // end draw
  draw->Canvas.End();
  draw->Update();

  // RA5 mean value to gauge1
  gauge1->SetValue((pic.pins[26].oavalue - 55) / 2);
  // RA1 mean value to gauge2
  gauge2->SetValue((pic.pins[27].oavalue - 55) / 2);
  // RA2 mean value to gauge3
  gauge3->SetValue((pic.pins[0].oavalue - 55) / 2);
  // RC5 mean value to gauge4
  gauge4->SetValue((pic.pins[1].oavalue - 55) / 2);
}

void cboard_Xpress::Run_CPU(void) {
  int i;
  int j;
  unsigned char pi;
  const picpin *pins;
  unsigned int alm[28];

  const int JUMPSTEPS = Window1.GetJUMPSTEPS(); // number of steps skipped
  const long int NSTEP = Window1.GetNSTEP();    // number of steps in 100ms
  const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

  // reset mean value
  memset(alm, 0, 28 * sizeof(unsigned int));

  // read pic.pins to a local variable to speed up
  pins = pic.pins;

  if (use_spare)
    Window5.PreProcess();

  j = JUMPSTEPS; // step counter
  pi = 0;
  if (Window1.Get_mcupwr())     // if powered
    for (i = 0; i < NSTEP; i++) // repeat for number of steps in 100ms
    {
      if (j >= JUMPSTEPS) // if number of step is bigger than steps to skip
      {
        pic_set_pin(&pic, pic.mclr, p_RST);
        pic_set_pin(&pic, 4, p_BT1); // Set pin 4 (RA5) with button state
      }

      // verify if a breakpoint is reached if not run one instruction
      if (!mplabxd_testbp())
        pic_step(&pic);
      ioupdated = pic.ioupdated;
      if (use_oscope)
        Window4.SetSample();
      if (use_spare)
        Window5.Process();

      // increment mean value counter if pin is high
      alm[pi] += pins[pi].value;
      pi++;
      if (pi == pic.PINCOUNT)
        pi = 0;

      if (j >= JUMPSTEPS) // if number of step is bigger than steps to skip
      {
        // set analog pin 3 (RA4 ANA4) with value from scroll
        pic_set_apin(&pic, 3, (3.3 * pot1 / 199));

        j = -1; // reset counter
      }

      j++; // counter increment
      pic.ioupdated = 0;
    }

  // calculate mean value
  for (pi = 0; pi < pic.PINCOUNT; pi++) {
    pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
  }

  if (use_spare)
    Window5.PostProcess();
}

board_init(BOARD_Xpress_Name, cboard_Xpress);
