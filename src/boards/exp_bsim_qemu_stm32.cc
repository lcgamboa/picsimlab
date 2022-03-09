/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#ifndef _WIN_
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define INVALID_HANDLE_VALUE -1;
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#define _TCP_
#define MSG_NOSIGNAL 0
#endif

#include "../picsimlab1.h"
#include "../serial_port.h"
#include "exp_bsim_qemu_stm32.h"

void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

static int listenfd = -1;
static int listenfd_mon = -1;

bsim_qemu_stm32::bsim_qemu_stm32(void) {
    connected = 0;
    sockfd = -1;
    sockmon = -1;
    fname_bak[0] = 0;
    fname_[0] = 0;

    memset(&ADCvalues, 0xFF, 32);
}

bsim_qemu_stm32::~bsim_qemu_stm32(void) {}

void bsim_qemu_stm32::MSetSerial(const char* port) {
 /*
 set_serial (&pic,0, port, 0, 0, 0);
 set_serial (&pic,1, "", 0, 0, 0);
  */ }

 int bsim_qemu_stm32::MInit(const char* processor, const char* fname, float freq) {
#ifdef _TCP_
     struct sockaddr_in serv, cli;
#else
     struct sockaddr_un serv, cli;
#endif
     sockaddr_in serv_mon, cli_mon;

     char buff[200];
     int n;
     char fname_[300];
     char cmd[600];

     lxString sproc = GetSupportedDevices();
     if (!sproc.Contains(processor)) {
         Proc = "stm32f103c8t6";
         printf("PICSimLab: Unknown processor %s, loading default !\n", processor);
     }

#ifdef _WIN_
     int clilen;
#else
     unsigned int clilen;
#endif

     sockfd = -1;
     sockmon = -1;

     pins_reset();

     serialfd[0] = INVALID_HANDLE_VALUE;
     serialfd[1] = INVALID_HANDLE_VALUE;
     serialfd[2] = INVALID_HANDLE_VALUE;
     serialfd[3] = INVALID_HANDLE_VALUE;

     if (listenfd < 0) {
         int reuse = 1;
#ifdef _TCP_
         if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
             printf("picsimlab: socket error : %s \n", strerror(errno));
             exit(1);
         }

         if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
             perror("setsockopt(SO_REUSEADDR) failed");

         memset(&serv, 0, sizeof(serv));
         serv.sin_family = AF_INET;
         serv.sin_addr.s_addr = htonl(INADDR_ANY);
         serv.sin_port = htons(2200);
#else
         if ((listenfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
             printf("picsimlab: socket error : %s \n", strerror(errno));
             exit(1);
         }

         memset(&serv, 0, sizeof(serv));
         serv.sun_family = AF_UNIX;
         serv.sun_path[0] = 0;
         strncpy(serv.sun_path + 1, "picsimlab_qemu", sizeof(serv.sun_path) - 2);
#endif

         if (bind(listenfd, (sockaddr*)&serv, sizeof(serv))) {
             printf("picsimlab: qemu_stm32 bind error : %s \n", strerror(errno));
             exit(1);
         }

         if (listen(listenfd, SOMAXCONN)) {
             printf("picsimlab: qemu_stm32 listen error : %s \n", strerror(errno));
             exit(1);
         }
         // monitor
         if ((listenfd_mon = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
             printf("picsimlab: qemu_stm32 socket error : %s \n", strerror(errno));
             exit(1);
         }

         if (setsockopt(listenfd_mon, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
             perror("setsockopt(SO_REUSEADDR) failed");

         memset(&serv_mon, 0, sizeof(serv_mon));
         serv_mon.sin_family = AF_INET;
         serv_mon.sin_addr.s_addr = htonl(INADDR_ANY);
         serv_mon.sin_port = htons(2500);

         if (bind(listenfd_mon, (sockaddr*)&serv_mon, sizeof(serv_mon))) {
             printf("picsimlab: qemu_stm32 monitor bind error : %s \n", strerror(errno));
             exit(1);
         }

         if (listen(listenfd_mon, SOMAXCONN)) {
             printf("picsimlab: qemu_stm32 monitor listen error : %s \n", strerror(errno));
             exit(1);
         }
     }

     // change .hex to .bin
     strncpy(fname_, fname, 299);
     fname_[strlen(fname_) - 3] = 0;
     strncat(fname_, "bin", 299);

     if (!lxFileExists(fname_)) {
         // create a empty memory
         FILE* fout;
         fout = fopen(fname_, "w");
         if (fout) {
             unsigned int val[4] = {0, 0, 0, 0};
             fwrite(&val, 4, sizeof(int), fout);
             fclose(fout);
         } else {
             printf("picsimlab: qemu_stm32 Erro creating file %s \n", fname_);
             exit(-1);
         }
     }

     // verify if qemu executable exists
#ifdef _WIN_
     if (!lxFileExists(Window1.GetSharePath() + lxT("/../qemu-stm32.exe")))
#else
     if (!lxFileExists(dirname(lxGetExecutablePath()) + lxT("/qemu-stm32")))
#endif
     {
         printf("picsimlab: qemu-stm32 not found! \n");
         Window1.RegisterError("qemu-stm32 not found!");
         return -1;
     }

     char* resp = serial_port_list();

     if (!Proc.compare("stm32f103c8t6")) {
         // verify if serial port exists
         if (strstr(resp, SERIALDEVICE)) {
             snprintf(
                 cmd, 599,
                 "qemu-stm32 -M stm32-f103c8-picsimlab -serial %s -qmp tcp:localhost:2500  -gdb tcp::%i -pflash \"%s\"",
                 SERIALDEVICE, Window1.Get_debug_port(), fname_);

         } else {
             snprintf(cmd, 599,
                      "qemu-stm32 -M stm32-f103c8-picsimlab -qmp tcp:localhost:2500 -gdb tcp::%i -pflash \"%s\"",
                      Window1.Get_debug_port(), fname_);
         }
     } else {
         // verify if serial port exists
         if (strstr(resp, SERIALDEVICE)) {
             snprintf(
                 cmd, 599,
                 "qemu-stm32 -M stm32-p103-picsimlab -serial %s -qmp tcp:localhost:2500  -gdb tcp::%i -pflash \"%s\"",
                 SERIALDEVICE, Window1.Get_debug_port(), fname_);

         } else {
             snprintf(cmd, 599,
                      "qemu-stm32 -M stm32-p103-picsimlab -qmp tcp:localhost:2500 -gdb tcp::%i -pflash \"%s\"",
                      Window1.Get_debug_port(), fname_);
         }
     }

     free(resp);

     printf("picsimlab: %s\n", (const char*)cmd);

#ifdef _WIN_
#define wxMSW_CONV_LPCTSTR(s) static_cast<const wxChar*>((s).t_str())
#define wxMSW_CONV_LPTSTR(s) const_cast<wxChar*>(wxMSW_CONV_LPCTSTR(s))
#define wxMSW_CONV_LPARAM(s) reinterpret_cast<LPARAM>(wxMSW_CONV_LPCTSTR(s))

     // wxExecute (Window1.GetSharePath () + lxT ("/../") + cmd, wxEXEC_MAKE_GROUP_LEADER | wxEXEC_SHOW_CONSOLE |
     // wxEXEC_ASYNC);
     wxExecute(Window1.GetSharePath() + lxT("/../") + cmd, wxEXEC_MAKE_GROUP_LEADER | wxEXEC_ASYNC);
     /*
     STARTUPINFO info = {sizeof (info)};
     PROCESS_INFORMATION processInfo;
     CreateProcess
         (
          NULL, // application name (use only cmd line)
          wxMSW_CONV_LPTSTR (Window1.GetSharePath () + lxT ("/../") + cmd), // full command line
          NULL, // security attributes: defaults for both
          NULL, //   the process and its main thread
          FALSE, // inherit handles if we use pipes
          CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS, // process creation flags
          NULL, // environment (may be NULL which is fine)
          NULL, // initial working directory
          &info, // startup info (unused here)
          &processInfo // process info
          );
      */
#else
     lxExecute(dirname(lxGetExecutablePath()) + lxT("/") + cmd, lxEXEC_MAKE_GROUP_LEADER);
#endif

     // monitor
     clilen = sizeof(cli_mon);
     if ((sockmon = accept(listenfd_mon, (sockaddr*)&cli_mon, &clilen)) < 0) {
         printf("picsimlab: accept error : %s \n", strerror(errno));
         exit(1);
     }
     printf("picsimlab: PICSimLab connected to Qemu qmp!\n");

     // read monitor qemu first mensage
     if ((n = recv(sockmon, buff, 199, 0)) < 0) {
         printf("picsimlab: recv error : %s \n", strerror(errno));
         exit(1);
     }
     buff[n] = 0;
     printf("picsimlab: %s", buff);

     qemu_cmd("qmp_capabilities");

     clilen = sizeof(cli);
     if ((sockfd = accept(listenfd, (sockaddr*)&cli, &clilen)) < 0) {
         printf("picsimlab: accept error : %s \n", strerror(errno));
         exit(1);
     }
     printf("picsimlab: Qemu connected to PICSimLab!\n");

     setnblock(sockfd);

     Window1.menu1_File_LoadHex.SetText("Load Bin");
     Window1.menu1_File_SaveHex.SetEnable(0);
     Window1.filedialog1.SetFileName(lxT("untitled.bin"));
     Window1.filedialog1.SetFilter(lxT("Bin Files (*.bin)|*.bin;*.BIN"));

     // qemu_cmd ("cont");
     connected = 1;

     return 0;  // ret;
 }

 void bsim_qemu_stm32::MEnd(void) {
     if (connected) {
         qemu_cmd("quit");
     }

     if (sockfd >= 0)
         close(sockfd);
     if (sockmon >= 0)
         close(sockmon);

     sockfd = -1;
     sockmon = -1;

     Window1.menu1_File_LoadHex.SetText("Load Hex");
     Window1.menu1_File_SaveHex.SetEnable(1);
     Window1.filedialog1.SetFileName(lxT("untitled.hex"));
     Window1.filedialog1.SetFilter(lxT("Hex Files (*.hex)|*.hex;*.HEX"));

#ifdef _WIN_
     Sleep(200);
#else
     usleep(200000);
#endif

     if (connected && fname_bak[0]) {
         lxRenameFile(fname_bak, fname_);
     }

     connected = 0;
 }

 int bsim_qemu_stm32::MGetArchitecture(void) {
     return ARCH_STM32;
 }

 void bsim_qemu_stm32::EndServers(void) {
     if (listenfd >= 0)
         close(listenfd);
     if (listenfd_mon >= 0)
         close(listenfd_mon);

     listenfd = -1;
     listenfd_mon = -1;
 }

 void bsim_qemu_stm32::MEraseFlash(void) {
     // erase_flash ();
 }

 void bsim_qemu_stm32::MSetFreq(float freq_) {
     freq = freq_;
 }

 float bsim_qemu_stm32::MGetFreq(void) {
     return freq;
 }

 void bsim_qemu_stm32::MSetVCC(float vcc) {
     printf("qemu_stm32: Incomplete!!!!\n");
 }

 float bsim_qemu_stm32::MGetVCC(void) {
     return 3.3;
 }

 float bsim_qemu_stm32::MGetInstClockFreq(void) {
     return freq;
 }

 int bsim_qemu_stm32::CpuInitialized(void) {
     return 1;
 }

 void bsim_qemu_stm32::DebugLoop(void) {}

 lxString bsim_qemu_stm32::MGetPinName(int pin) {
     lxString pinname = "error";

     if (!Proc.compare("stm32f103c8t6")) {
         switch (pin) {
             case 1:
                 pinname = "VBAT";
                 break;
             case 2:
                 pinname = "PC13";
                 break;
             case 3:
                 pinname = "PC14";
                 break;
             case 4:
                 pinname = "PC15";
                 break;
             case 5:
                 pinname = "PD0";
                 break;
             case 6:
                 pinname = "PD1";
                 break;
             case 7:
                 pinname = "NRST";
                 break;
             case 8:
                 pinname = "VSSA";
                 break;
             case 9:
                 pinname = "VDDA";
                 break;
             case 10:
                 pinname = "PA0";
                 break;
             case 11:
                 pinname = "PA1";
                 break;
             case 12:
                 pinname = "PA2";
                 break;
             case 13:
                 pinname = "PA3";
                 break;
             case 14:
                 pinname = "PA4";
                 break;
             case 15:
                 pinname = "PA5";
                 break;
             case 16:
                 pinname = "PA6";
                 break;
             case 17:
                 pinname = "PA7";
                 break;
             case 18:
                 pinname = "PB0";
                 break;
             case 19:
                 pinname = "PB1";
                 break;
             case 20:
                 pinname = "PB2";
                 break;
             case 21:
                 pinname = "PB10";
                 break;
             case 22:
                 pinname = "PB11";
                 break;
             case 23:
                 pinname = "VSS";
                 break;
             case 24:
                 pinname = "VDD";
                 break;
             case 25:
                 pinname = "PB12";
                 break;
             case 26:
                 pinname = "PB13";
                 break;
             case 27:
                 pinname = "PB14";
                 break;
             case 28:
                 pinname = "PB15";
                 break;
             case 29:
                 pinname = "PA8";
                 break;
             case 30:
                 pinname = "PA9";
                 break;
             case 31:
                 pinname = "PA10";
                 break;
             case 32:
                 pinname = "PA11";
                 break;
             case 33:
                 pinname = "PA12";
                 break;
             case 34:
                 pinname = "PA13";
                 break;
             case 35:
                 pinname = "VSS";
                 break;
             case 36:
                 pinname = "VDD";
                 break;
             case 37:
                 pinname = "PA14";
                 break;
             case 38:
                 pinname = "PA15";
                 break;
             case 39:
                 pinname = "PB3";
                 break;
             case 40:
                 pinname = "PB4";
                 break;
             case 41:
                 pinname = "PB5";
                 break;
             case 42:
                 pinname = "PB6";
                 break;
             case 43:
                 pinname = "PB7";
                 break;
             case 44:
                 pinname = "BOOT0";
                 break;
             case 45:
                 pinname = "PB8";
                 break;
             case 46:
                 pinname = "PB9";
                 break;
             case 47:
                 pinname = "VSS";
                 break;
             case 48:
                 pinname = "VDD";
                 break;
         }
     } else if (!Proc.compare("stm32f103rbt6")) {
         switch (pin) {
             case 1:
                 pinname = "VBAT";
                 break;
             case 2:
                 pinname = "PC13";
                 break;
             case 3:
                 pinname = "PC14";
                 break;
             case 4:
                 pinname = "PC15";
                 break;
             case 5:
                 pinname = "PD0";
                 break;
             case 6:
                 pinname = "PD1";
                 break;
             case 7:
                 pinname = "NRST";
                 break;
             case 8:
                 pinname = "PC0";
                 break;
             case 9:
                 pinname = "PC1";
                 break;
             case 10:
                 pinname = "PC2";
                 break;
             case 11:
                 pinname = "PC3";
                 break;
             case 12:
                 pinname = "VSSA";
                 break;
             case 13:
                 pinname = "VDDA";
                 break;
             case 14:
                 pinname = "PA0";
                 break;
             case 15:
                 pinname = "PA1";
                 break;
             case 16:
                 pinname = "PA2";
                 break;
             case 17:
                 pinname = "PA3";
                 break;
             case 18:
                 pinname = "VSS";
                 break;
             case 19:
                 pinname = "VDD";
                 break;
             case 20:
                 pinname = "PA4";
                 break;
             case 21:
                 pinname = "PA5";
                 break;
             case 22:
                 pinname = "PA6";
                 break;
             case 23:
                 pinname = "PA7";
                 break;
             case 24:
                 pinname = "PC4";
                 break;
             case 25:
                 pinname = "PC5";
                 break;
             case 26:
                 pinname = "PB0";
                 break;
             case 27:
                 pinname = "PB1";
                 break;
             case 28:
                 pinname = "PB2";
                 break;
             case 29:
                 pinname = "PB10";
                 break;
             case 30:
                 pinname = "PB11";
                 break;
             case 31:
                 pinname = "VSS";
                 break;
             case 32:
                 pinname = "VDD";
                 break;
             case 33:
                 pinname = "PB12";
                 break;
             case 34:
                 pinname = "PB13";
                 break;
             case 35:
                 pinname = "PB14";
                 break;
             case 36:
                 pinname = "PB15";
                 break;
             case 37:
                 pinname = "PC6";
                 break;
             case 38:
                 pinname = "PC7";
                 break;
             case 39:
                 pinname = "PC8";
                 break;
             case 40:
                 pinname = "PC9";
                 break;
             case 41:
                 pinname = "PA8";
                 break;
             case 42:
                 pinname = "PA9";
                 break;
             case 43:
                 pinname = "PA10";
                 break;
             case 44:
                 pinname = "PA11";
                 break;
             case 45:
                 pinname = "PA12";
                 break;
             case 46:
                 pinname = "PA13";
                 break;
             case 47:
                 pinname = "VSS";
                 break;
             case 48:
                 pinname = "VDD";
                 break;
             case 49:
                 pinname = "PA14";
                 break;
             case 50:
                 pinname = "PA15";
                 break;
             case 51:
                 pinname = "PC10";
                 break;
             case 52:
                 pinname = "PC11";
                 break;
             case 53:
                 pinname = "PC12";
                 break;
             case 54:
                 pinname = "PD2";
                 break;
             case 55:
                 pinname = "PB3";
                 break;
             case 56:
                 pinname = "PB4";
                 break;
             case 57:
                 pinname = "PB5";
                 break;
             case 58:
                 pinname = "PB6";
                 break;
             case 59:
                 pinname = "PB7";
                 break;
             case 60:
                 pinname = "BOOT0";
                 break;
             case 61:
                 pinname = "PB8";
                 break;
             case 62:
                 pinname = "PB9";
                 break;
             case 63:
                 pinname = "VSS";
                 break;
             case 64:
                 pinname = "VDD";
                 break;
         }
     }

     return pinname;
 }

 void bsim_qemu_stm32::MDumpMemory(const char* fname) {
     char cmd[500];

     // change .hex to .bin
     strncpy(fname_, fname, 299);
     fname_[strlen(fname) - 3] = 0;
     strncat(fname_, "bin", 299);

     qemu_cmd("stop");
     if (lxFileExists(fname_)) {
         // save backup copy until end
         strncpy(fname_bak, fname, 299);
         fname_bak[strlen(fname) - 3] = 0;
         strncat(fname_bak, "bak", 299);
#ifdef _WIN_
         for (unsigned int i = 0; i < strlen(fname_); i++) {
             if (fname_[i] == '\\')
                 fname_[i] = '/';
             if (fname_bak[i] == '\\')
                 fname_bak[i] = '/';
         }
#endif
         snprintf(cmd, 500,
                  "{ \"execute\": \"pmemsave\",\"arguments\": { \"val\": 134217728, \"size\": 65536, \"filename\": "
                  "\"%s\" } }\n",
                  fname_bak);
     } else {
         // save file direct
#ifdef _WIN_
         for (unsigned int i = 0; i < strlen(fname_); i++) {
             if (fname_[i] == '\\')
                 fname_[i] = '/';
         }
#endif
         snprintf(cmd, 500,
                  "{ \"execute\": \"pmemsave\",\"arguments\": { \"val\": 134217728, \"size\": 65536, \"filename\": "
                  "\"%s\" } }\n",
                  fname_);
     }
     qemu_cmd(cmd, 1);
     qemu_cmd("cont");
 }

 int bsim_qemu_stm32::DebugInit(int dtyppe)  // argument not used in picm only mplabx
 {
     return 0;  //! mplabxd_init (this, Window1.Get_debug_port ()) - 1;
 }

 int bsim_qemu_stm32::MGetPinCount(void) {
     if (!Proc.compare("stm32f103c8t6"))
         return 48;
     if (!Proc.compare("stm32f103rbt6"))
         return 64;
     return 0;
 }

 void bsim_qemu_stm32::pins_reset(void) {
     for (int p = 0; p < MGetPinCount(); p++) {
         pins[p].avalue = 0;
         pins[p].lvalue = 0;
         pins[p].value = 0;
         pins[p].ptype = PT_DIGITAL;
         pins[p].dir = PD_IN;
         pins[p].ovalue = 0;
         pins[p].oavalue = 0;

         if (MGetPinName(p + 1).Contains("VDD")) {
             pins[p].value = 1;
             pins[p].dir = PD_OUT;
         }
     }
 }

 void bsim_qemu_stm32::MSetPin(int pin, unsigned char value) {
     if (!pin)
         return;
     if ((connected) && (pins[pin - 1].value != value)) {
         unsigned char val = (0x7F & pin);

         if (value)
             val |= 0x80;
         if (send(sockfd, (const char*)&val, 1, MSG_NOSIGNAL) != 1) {
             // printf ("picsimlab MSetPin: send error : %s \n", strerror (errno));
             // exit (1);
             value = !value;
         }
         pins[pin - 1].value = value;
     }
 }

 void bsim_qemu_stm32::MSetPinDOV(int pin, unsigned char ovalue) {
     // set_pin_DOV (pin, ovalue);
 }

 void bsim_qemu_stm32::MSetAPin(int pin, float value) {
     if (!pin)
         return;
     if ((connected) && (pins[pin - 1].avalue != value)) {
         unsigned char channel = 0xFF;

         pins[pin - 1].avalue = value;

         if (!Proc.compare("stm32f103c8t6")) {
             switch (pin) {
                 case 10:  // PA0
                     channel = 0;
                     break;
                 case 11:  // PA1
                     channel = 1;
                     break;
                 case 12:  // PA2
                     channel = 2;
                     break;
                 case 13:  // PA3
                     channel = 3;
                     break;
                 case 14:  // PA4
                     channel = 4;
                     break;
                 case 15:  // PA5
                     channel = 5;
                     break;
                 case 16:  // PA6
                     channel = 6;
                     break;
                 case 17:  // PA7
                     channel = 7;
                     break;
                 case 18:  // PB0
                     channel = 8;
                     break;
                 case 19:  // PB1
                     channel = 9;
                     break;
             }
         } else if (!Proc.compare("stm32f103rbt6")) {
             switch (pin) {
                 case 8:  // PC0
                     channel = 10;
                     break;
                 case 9:  // PC1
                     channel = 11;
                     break;
                 case 10:  // PC2
                     channel = 12;
                     break;
                 case 11:  // PC3
                     channel = 13;
                     break;
                 case 14:  // PA0
                     channel = 0;
                     break;
                 case 15:  // PA1
                     channel = 1;
                     break;
                 case 16:  // PA2
                     channel = 2;
                     break;
                 case 17:  // PA3
                     channel = 3;
                     break;
                 case 20:  // PA4
                     channel = 4;
                     break;
                 case 21:  // PA5
                     channel = 5;
                     break;
                 case 22:  // PA6
                     channel = 6;
                     break;
                 case 23:  // PA7
                     channel = 7;
                     break;
                 case 24:  // PC4
                     channel = 14;
                     break;
                 case 25:  // PC5
                     channel = 15;
                     break;
                 case 26:  // PB0
                     channel = 8;
                     break;
                 case 27:  // PB1
                     channel = 9;
                     break;
             }
         }
         if (channel != 0xFF) {
             if (value > 3.3)
                 value = 3.3;
             if (value < 0)
                 value = 0;

             unsigned short svalue = (unsigned short)(4096 * value / 3.3);

             pins[pin - 1].ptype = PT_ANALOG;

             if (ADCvalues[channel] != svalue) {
                 unsigned char buff[3];

                 buff[0] = channel | 0x40;
                 buff[1] = svalue & 0xFF;
                 buff[2] = svalue >> 8;

                 ADCvalues[channel] = svalue;
                 if (send(sockfd, (const char*)buff, 3, MSG_NOSIGNAL) != 3) {
                     // printf ("picsimlab MSetAPin: send error : %s \n", strerror (errno));
                     // exit (1);
                     pins[pin - 1].avalue = value + 0.5;
                 }
                 // printf("Analog channel %02X = %i\n",channel,svalue);
             }
         }
     }
 }

 unsigned char bsim_qemu_stm32::MGetPin(int pin) {
     if ((pin) && (pin < MGetPinCount())) {
         return pins[pin - 1].value;
     }
     return 0;
 }

 void bsim_qemu_stm32::MReset(int flags) {
     if (connected) {
         qemu_cmd("system_reset");
     }
 }

 const picpin* bsim_qemu_stm32::MGetPinsValues(void) {
     return pins;
 }

 void bsim_qemu_stm32::MStep(void) {
     char buff;
     int n;

     if (connected) {
         if ((n = recv(sockfd, &buff, 1, 0)) > 0) {
             pins[(0x7F & buff) - 1].value = ((0x80 & buff) > 0);
             // printf("%02i %s=%i\n",0x7F & buff,(const char *)MGetPinName(0x7F & buff).c_str(), (0x80 & buff)>0);
         }
     }
 }

 void bsim_qemu_stm32::MStepResume(void) {
     // if (pic.s2 == 1)step ();
 }

 int bsim_qemu_stm32::qemu_cmd(const char* cmd, int raw) {
     int n;
     char buffin[400];
     char buffout[400];
     int size;
     int connected_;

     if (sockmon < 0)
         return -1;
     connected_ = connected;
     connected = 0;
     /*
     //clear messages
      if ((n = recv (sockmon, buffout, 399, 0)) < 0)
      {
       return 1;
      }
     buffout[n] = 0;

     printf ("picsimlab: (%s)=(%s) \n", buffin, buffout);
      */

     if (raw) {
         strcpy(buffin, cmd);
     } else {
         sprintf(buffin, "{ \"execute\": \"%s\" }\n", cmd);
     }

     size = strlen(buffin);

     if (send(sockmon, buffin, size, MSG_NOSIGNAL) != size) {
         printf("picsimlab: mon send error : %s \n", strerror(errno));
         exit(1);
     }

#ifdef _WIN_
     Sleep(1);
#else
     usleep(1000);
#endif

     if ((n = recv(sockmon, buffout, 399, 0)) < 0) {
         return 1;
     }
     buffout[n] = 0;

     printf("picsimlab: (%s)=(%s) \n", buffin, buffout);

     connected = connected_;
     return 0;
 }
