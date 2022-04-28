/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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

#include <stdint.h>

extern "C" {
void qemu_init(int, char**, char**);
void qemu_main_loop(void);
void qemu_cleanup(void);
void qemu_clear_opts(void);
#define Error char
void qmp_quit(Error** errp);
void qmp_stop(Error** errp);
void qmp_system_reset(Error** errp);
void qmp_pmemsave(int64_t val, int64_t size, const char* filename, Error** errp);
void qmp_cont(Error** errp);
#define qemu_mutex_lock_iothread() qemu_mutex_lock_iothread_impl(__FILE__, __LINE__)
void qemu_mutex_lock_iothread_impl(const char* file, int line);
void qemu_mutex_unlock_iothread(void);
void qemu_picsimlab_register(void (*picsimlab_write_pin)(int pin, int value));
void qemu_picsimlab_set_pin(int pin, int value);
void qemu_picsimlab_set_apin(int chn, int value);
typedef uint64_t hwaddr;
int cpu_physical_memory_rw(hwaddr addr, void* buf, hwaddr len, bool is_write);
bool main_loop_should_exit(void);
void main_loop_wait(int64_t timeout);
typedef enum {
    QEMU_CLOCK_REALTIME = 0,
    QEMU_CLOCK_VIRTUAL = 1,
    QEMU_CLOCK_HOST = 2,
    QEMU_CLOCK_VIRTUAL_RT = 3,
    QEMU_CLOCK_MAX
} QEMUClockType;
int64_t qemu_clock_get_ns(QEMUClockType type);

typedef struct QEMUTimer QEMUTimer;
typedef void QEMUTimerCB(void* opaque);
typedef struct QEMUTimerList QEMUTimerList;
typedef struct QEMUTimerListGroup QEMUTimerListGroup;

void timer_init_full(QEMUTimer* ts, QEMUTimerListGroup* timer_list_group, QEMUClockType type, int scale, int attributes,
                     QEMUTimerCB* cb, void* opaque);

void timer_mod_ns(QEMUTimer* ts, int64_t expire_time);

struct QEMUTimer {
    int64_t expire_time;  // in nanoseconds
    QEMUTimerList* timer_list;
    QEMUTimerCB* cb;
    void* opaque;
    QEMUTimer* next;
    int attributes;
    int scale;
};

typedef struct {
    QEMUTimer* qtimer;
    int64_t timeout;
    int64_t last;
} user_timer_t;
}