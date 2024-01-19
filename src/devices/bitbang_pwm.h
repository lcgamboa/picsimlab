/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2023-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BITBANG_PWM
#define BITBANG_PWM

class board;

#define LEDC_MAX 32

typedef struct {
    unsigned char chanels;
    board* pboard;
    int TimerID;
    unsigned int counter;
    unsigned char pins[LEDC_MAX];
    unsigned char out[LEDC_MAX];
    unsigned char duty[LEDC_MAX];
} bitbang_pwm_t;

void bitbang_pwm_init(bitbang_pwm_t* pwm, board* pboard, const unsigned char channels = 8);
void bitbang_pwm_rst(bitbang_pwm_t* pwm);
void bitbang_pwm_end(bitbang_pwm_t* pwm);
void bitbang_pwm_set_duty(bitbang_pwm_t* pwm, const unsigned char channel, const char duty);

#endif  // BITBANG_PWM