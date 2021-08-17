/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gamboa Lopes

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

#ifndef TESTS_H
#define TESTS_H

#define MAX_TESTS 20

//extern int NUM_TESTS;

#define register_test(description,function, arg)  \
    static void __attribute__((constructor)) function## _register(void);\
    static void function## _register(void){\
    test_register(description , function , arg);}


typedef int (* test_run_func)(void * arg);

void test_register(const char * name, test_run_func trun, void* arg);

//control
int test_load(const char * fname);
int test_send_rcmd(const char * message);
char * test_get_cmd_resp(void);
int test_end();

//serial
int test_serial_send(const char data);
int test_serial_recv(char *data);
int test_serial_recv_wait(char * data, const int timeout);
int test_serial_recv_str(char * data, const int size, const int timeout=0);

int test_file_exist(const char * fname);

#endif /* TESTS_H */

