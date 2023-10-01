/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2023-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "bitbang_pwm.h"
#include "../lib/board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void bitbang_pwm_rst(bitbang_pwm_t* pwm) {
    dprintf("bitbang_pwm rst\n");
    pwm->counter = 0;
    for (int i = 0; i < pwm->chanels; i++) {
        pwm->duty[i] = 0;
        pwm->out[i] = 0;
    }
}

static void bitbang_pwm_ctrl_callback(void* arg) {
    bitbang_pwm_t* pwm = (bitbang_pwm_t*)arg;
    unsigned char out;

    for (int i = 0; i < pwm->chanels; i++) {
        if (pwm->pins[i]) {
            if (pwm->counter < pwm->duty[i]) {
                out = 1;
            } else {
                out = 0;
            }
            if (pwm->out[i] != out) {
                pwm->out[i] = out;
                ioupdated = 1;
            }
        }
    }

    pwm->counter++;
    if (pwm->counter > 99) {
        pwm->counter = 0;
    }
}

void bitbang_pwm_init(bitbang_pwm_t* pwm, board* pboard, const unsigned char channels) {
    dprintf("bitbang_pwm init\n");
    pwm->chanels = channels;
    bitbang_pwm_rst(pwm);
    pwm->pboard = pboard;
    pwm->TimerID = pwm->pboard->TimerRegister_us(2, bitbang_pwm_ctrl_callback, pwm);
    pwm->pboard->TimerChange_us(pwm->TimerID, 2);  // FIXME only 5kHz frequency
    pwm->pboard->TimerSetState(pwm->TimerID, 0);   // disabled
}

void bitbang_pwm_end(bitbang_pwm_t* pwm) {
    dprintf("bitbang_pwm end\n");
    pwm->pboard->TimerUnregister(pwm->TimerID);
}

void bitbang_pwm_set_duty(bitbang_pwm_t* pwm, const unsigned char channel, const char duty) {
    dprintf("bitbang_pwm chanell %i (pin %i) set duty to %i%% \n", channel, pwm->pins[channel], duty);
    pwm->pboard->TimerSetState(pwm->TimerID, 1);
    pwm->duty[channel] = duty;
}
