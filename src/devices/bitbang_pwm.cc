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
    for (int i = 0; i < pwm->channels_count; i++) {
        pwm->channels[i].counter = 0;
        pwm->channels[i].duty = 0;
        pwm->channels[i].out = 0;
        pwm->channels[i].freq = 0;
        pwm->channels[i].enabled = 0;
    }
}

static void bitbang_pwm_ctrl_callback(void* arg) {
    channel_pwm_t* channel = (channel_pwm_t*)arg;
    unsigned char out;

    if (channel->pin) {
        if (channel->counter < channel->duty) {
            out = 1;
        } else {
            out = 0;
        }
        if (channel->out != out) {
            channel->out = out;
            ioupdated = 1;
        }

        channel->counter++;

        if (channel->counter > ((100 * channel->res) - 1)) {
            channel->counter = 0;
        }
    }
}

void bitbang_pwm_init(bitbang_pwm_t* pwm, board* pboard, const unsigned char channels) {
    dprintf("bitbang_pwm init\n");
    pwm->channels_count = channels;
    pwm->pboard = pboard;
    memset(pwm->channels, 0, sizeof(channel_pwm_t) * PWM_CHANNEL_MAX);
}

void bitbang_pwm_end(bitbang_pwm_t* pwm) {
    dprintf("bitbang_pwm end\n");
    for (int i = 0; i < pwm->channels_count; i++) {
        if (pwm->channels[i].pin) {
            pwm->pboard->TimerUnregister(pwm->channels[i].TimerID);
        }
    }
}

void bitbang_pwm_set_pin(bitbang_pwm_t* pwm, const unsigned char channel, const char pin) {
    pwm->channels[channel].pin = pin;
    if (!pwm->channels[channel].TimerID) {
        pwm->channels[channel].TimerID =
            pwm->pboard->TimerRegister_us(2, bitbang_pwm_ctrl_callback, &pwm->channels[channel]);
    }

    if (!pwm->channels[channel].freq) {
        bitbang_pwm_set_freq(pwm, channel, 5000);
    } else {
        bitbang_pwm_set_freq(pwm, channel, pwm->channels[channel].freq);
    }
    pwm->pboard->TimerSetState(pwm->channels[channel].TimerID, 1);
}

void bitbang_pwm_set_duty(bitbang_pwm_t* pwm, const unsigned char channel, const char duty) {
    dprintf("bitbang_pwm channel %i (pin %i) set duty to %i%% \n", channel, pwm->channels[channel].pin, duty);
    pwm->channels[channel].duty = duty * pwm->channels[channel].res;
}

void bitbang_pwm_set_duty_f(bitbang_pwm_t* pwm, const unsigned char channel, const float duty) {
    dprintf("bitbang_pwm channel %i (pin %i) set duty to %f%% \n", channel, pwm->channels[channel].pin, duty);
    pwm->channels[channel].duty = duty * pwm->channels[channel].res;
}

void bitbang_pwm_set_freq(bitbang_pwm_t* pwm, const unsigned char channel, unsigned int freq) {
    if (freq > 10000) {
        freq = 10000;
    }
    if (freq <= 0) {
        freq = 1;
    }
    if ((pwm->channels[channel].freq != freq) && (pwm->channels[channel].TimerID)) {
        dprintf("bitbang_pwm channel %i (pin %i) set freq to %i%% \n", channel, pwm->channels[channel].pin, freq);
        if (freq > 1000) {  // 0-100
            pwm->channels[channel].res = 1;
        } else if (freq > 200) {  // 0-200
            pwm->channels[channel].res = 2;
        } else {  // 0-400
            pwm->channels[channel].res = 4;
        }
        pwm->pboard->TimerChange_us(pwm->channels[channel].TimerID, 10000.0 / (freq * pwm->channels[channel].res));
        pwm->channels[channel].freq = freq;
    }
}

void bitbang_pwm_set_enable(bitbang_pwm_t* pwm, const unsigned char channel, unsigned char enable) {
    pwm->channels[channel].enabled = enable;
}