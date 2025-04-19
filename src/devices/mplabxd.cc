/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "mplabxd.h"

// #define _DEBUG_
#define dprint \
    if (1) {   \
    } else     \
        printf

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
// system headers dependent
#ifndef _WIN_
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#define min(a, b) ((a) < (b) ? (a) : (b))
#include <fcntl.h>
#include <sys/unistd.h>
#else
#include <winsock.h>
static WORD wVersionRequested = 2;
static WSADATA wsaData;
#ifndef MSG_WAITALL
#define MSG_WAITALL (1 << 3)
#endif
#define MSG_NOSIGNAL 0
#endif
// system headers independent
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/picsimlab.h"

typedef struct sockaddr sockaddr;

// debug protocol commands

#define STARTD 0xF0  // start debug
#define STOPD 0xFF   // stop  debug

#define STEP 0x01    // do one step
#define RESET 0x05   // do reset
#define RUN 0x10     // run
#define HALT 0x15    // halt
#define GETPC 0x20   // return PC
#define SETPC 0x25   // set PC
#define SETBK 0x30   // set Breakpoint
#define STRUN 0x31   // Return Run status
#define SDWBK 0x32   // set data write Breakpoint
#define SDRBK 0x33   // set data read Breakpoint
#define GETID 0x35   // Return CPU ID
#define GETNAM 0x36  // Retunr CPU name
#define PROGD 0x40   // Write all data ram
#define PROGP 0x45   // Write all program flash
#define PROGC 0x50   // Write all Configuration
#define PROGI 0x55   // Write all IDs
#define PROGE 0x57   // Write all data EEPROM
#define READD 0x60   // Read all data RAM
#define READDV 0x61  // Read data value from RAM
#define READP 0x65   // Read all program flash
#define READC 0x70   // Read all Configuration
#define READI 0x75   // Read all IDs
#define READE 0x80   // Read all data EEPROM

#ifdef _WIN_

__attribute__((constructor)) static void initialize_socket(void) {
    WSAStartup(wVersionRequested, &wsaData);
    if (wsaData.wVersion != wVersionRequested) {
        fprintf(stderr, "\n Wrong version\n");
        return;
    }
}

__attribute__((destructor)) static void finalize_socket(void) {
    WSACleanup();
}
#endif

static int sockfd = -1;
static int listenfd = -1;
static int server_started = 0;

static board* dbg_board = NULL;
static unsigned char* ramsend = NULL;
static unsigned char* ramreceived = NULL;

void setnblock(int sock_descriptor);
void setblock(int sock_descriptor);

int mplabxd_init(board* mboard, unsigned short tcpport) {
    struct sockaddr_in serv;

    dbg_board = mboard;

    if (!server_started) {
        dprint("mplabxd_init\n");

        if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("mplabxd: socket error : %s \n", strerror(errno));
            return 1;
        };

        int reuse = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("mplabxd: setsockopt(SO_REUSEADDR) failed");

        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
        serv.sin_port = htons(tcpport);

        if (bind(listenfd, (sockaddr*)&serv, sizeof(serv))) {
            printf("mplabxd: bind error : %s \n", strerror(errno));
            char stemp[100];
            snprintf(stemp, 100, "Can't open mplabxd TCP port %i\n It is already in use by another application!",
                     tcpport);
            PICSimLab.RegisterError(stemp);
            return 1;
        }

        if (listen(listenfd, SOMAXCONN)) {
            printf("mplabxd: listen error : %s \n", strerror(errno));
            return 1;
        }
        setnblock(listenfd);
        server_started = 1;
    }

    if (!ramsend) {
        ramsend = (unsigned char*)malloc(dbg_board->DBGGetRAMSize());
        ramreceived = (unsigned char*)malloc(dbg_board->DBGGetRAMSize());
    }
    return 0;
}

int mplabxd_start(void) {
    struct sockaddr_in cli;
#ifndef _WIN_
    unsigned int clilen;
#else
    int clilen;
#endif
    clilen = sizeof(cli);

    if ((sockfd = accept(listenfd, (sockaddr*)&cli, &clilen)) < 0) {
        return 1;
    }

    setnblock(sockfd);
    dprint("Debug connected!---------------------------------\n");
    return 0;
}

void mplabxd_stop(void) {
    dprint("Debug disconnected!---------------------------------\n");
    if (sockfd >= 0) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
    sockfd = -1;
}

void mplabxd_end(void) {
    mplabxd_stop();
    if (ramsend) {
        free(ramsend);
        free(ramreceived);
        ramsend = NULL;
        ramreceived = NULL;
        dbg_board = NULL;
    }
}

void mplabxd_server_end(void) {
    if (server_started) {
        dprint("mplabxd: server end\n");
        shutdown(listenfd, SHUT_RDWR);
        close(listenfd);
    }
    listenfd = -1;
    server_started = 0;
}

enum { BKCODE = 1, BKWDATA, BKRDATA };

// code
static int bpc = 0;
static unsigned int bp[100];
// data write
static int bpdwc = 0;
static unsigned int bpdw[100];
// data read
static int bpdrc = 0;
static unsigned int bpdr[100];

static unsigned short dbuff[2];

int mplabxd_testbp(void) {
    int i;
    if (!PICSimLab.GetMcuDbg()) {
        for (i = 0; i < bpc; i++) {
            if (dbg_board->DBGGetPC() == bp[i]) {
                dprint("breakpoint 0x%04X!!!!!=========================\n", bp[i]);
                PICSimLab.SetCpuState(CPU_BREAKPOINT);
                PICSimLab.Set_mcudbg(1);
                return PICSimLab.GetMcuDbg();
            }
        }
        for (i = 0; i < bpdwc; i++) {
            if (dbg_board->DBGGetRAMLAWR() == bpdw[i]) {
                dprint("breakpoint data wr 0x%04X!!!!!=========================\n", bpdw[i]);
                PICSimLab.SetCpuState(CPU_BREAKPOINT);
                PICSimLab.Set_mcudbg(1);
                return PICSimLab.GetMcuDbg();
            }
        }
        for (i = 0; i < bpdrc; i++) {
            if (dbg_board->DBGGetRAMLARD() == bpdr[i]) {
                dprint("breakpoint data rd 0x%04X!!!!!=========================\n", bpdr[i]);
                PICSimLab.SetCpuState(CPU_BREAKPOINT);
                PICSimLab.Set_mcudbg(1);
                return PICSimLab.GetMcuDbg();
            }
        }
    }
    return PICSimLab.GetMcuDbg();
}

int mplabxd_loop(void) {
    unsigned int pc;
    int i;

    int n;
    int ret = 0;

    unsigned char* uram;

    unsigned char cmd, reply;

    // open connection
    if (sockfd < 0)
        if (mplabxd_start())
            return 1;

    if ((n = recv(sockfd, (char*)&cmd, 1, 0)) < 0) {
        // printf ("receive error : %s \n", strerror (errno));
        // exit (1);
    }

    if (n == 1) {
        setblock(sockfd);

        dprint("cmd %02X  ", cmd);
        // fflush(stdout);

        reply = 0x00;

        switch (cmd) {
            case STARTD:
                dprint("STARTD cmd ----------------------\n");
                PICSimLab.Set_mcudbg(1);
                break;
            case STOPD:
                dprint("STOPD cmd -----------------------\n");
                ret = 1;
                PICSimLab.Set_mcudbg(0);
                PICSimLab.SetCpuState(CPU_RUNNING);
                bpc = 0;
                bpdwc = 0;
                bpdrc = 0;
                break;
            case STEP:
                dprint("STEP cmd\n");
                dbg_board->MStep();
                PICSimLab.SetCpuState(CPU_STEPPING);
                break;
            case RESET:
                PICSimLab.Set_mcudbg(1);
                dprint("RESET cmd\n");
                dbg_board->MStepResume();
                dbg_board->MReset(1);
                dbg_board->MStep();
                break;
            case RUN:
                PICSimLab.Set_mcudbg(0);
                dprint("RUN cmd\n");
                dbg_board->MStep();  // to go out break point
                PICSimLab.SetCpuState(CPU_RUNNING);
                break;
            case HALT:
                PICSimLab.Set_mcudbg(1);
                dbg_board->MStepResume();
                dprint("HALT cmd\n");
                PICSimLab.SetCpuState(CPU_HALTED);
                break;
            case GETPC:
                pc = dbg_board->DBGGetPC();
                dprint("GETPC %04Xcmd\n", pc);
                if (send(sockfd, (char*)&pc, 4, MSG_NOSIGNAL) != 4) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                break;
            case SETPC:
                if ((n = recv(sockfd, (char*)&pc, 4, MSG_WAITALL)) != 4) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dbg_board->DBGSetPC(pc);
                dprint("SETPC cmd\n");
                break;
            case SETBK:
                if ((n = recv(sockfd, (char*)&bpc, 2, MSG_WAITALL)) != 2) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("bp count =%i\n", bpc);
                if (bpc >= 100)
                    bpc = 100;
                if (bpc > 0) {
                    if ((n = recv(sockfd, (char*)&bp, bpc * 4, MSG_WAITALL)) != bpc * 4) {
                        printf("mplabxd: receive error : %s \n", strerror(errno));
                        ret = 1;
                        reply = 0x01;
                    }
#ifdef _DEBUG_
                    for (i = 0; i < bpc; i++)
                        printf("bp %i = %#06X\n", i, bp[i]);
#endif
                }
                dprint("SETBK cmd\n");
                break;
            case STRUN:
                i = PICSimLab.GetMcuDbg();
                if (send(sockfd, (char*)&i, 1, MSG_NOSIGNAL) != 1) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("STRUN cmd =%i\n", PICSimLab.GetMcuDbg());
                break;
            case SDWBK:
                if ((n = recv(sockfd, (char*)&bpdwc, 2, MSG_WAITALL)) != 2) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("bpdw count =%i\n", bpdwc);
                if (bpdwc >= 100)
                    bpdwc = 100;
                if (bpdwc > 0) {
                    if ((n = recv(sockfd, (char*)&bpdw, bpdwc * 4, MSG_WAITALL)) != bpdwc * 4) {
                        printf("mplabxd: receive error : %s \n", strerror(errno));
                        ret = 1;
                        reply = 0x01;
                    }
#ifdef _DEBUG_
                    for (i = 0; i < bpdwc; i++)
                        printf("bpdw %i = %#06X\n", i, bpdw[i]);
#endif
                }
                dprint("SDWBK cmd\n");
                break;
            case SDRBK:
                if ((n = recv(sockfd, (char*)&bpdrc, 2, MSG_WAITALL)) != 2) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("bpdr count =%i\n", bpdrc);
                if (bpdrc >= 100)
                    bpdrc = 100;
                if (bpdrc > 0) {
                    if ((n = recv(sockfd, (char*)&bpdr, bpdrc * 4, MSG_WAITALL)) != bpdrc * 4) {
                        printf("mplabxd: receive error : %s \n", strerror(errno));
                        ret = 1;
                        reply = 0x01;
                    }
#ifdef _DEBUG_
                    for (i = 0; i < bpdrc; i++)
                        printf("bpdr %i = %#06X\n", i, bpdr[i]);
#endif
                }
                dprint("SDRBK cmd\n");
                break;
            case GETID:
                dprint("GETID cmd\n");
                if (send(sockfd, (char*)dbg_board->DBGGetProcID_p(), 2, MSG_NOSIGNAL) != 2) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                break;
            case GETNAM:
                dprint("GETNAM cmd\n");
                char buff[20];
                buff[0] = dbg_board->GetProcessorName().length();
                strncpy(buff + 1, (const char*)dbg_board->GetProcessorName().c_str(), 18);
                if (send(sockfd, buff, 20, MSG_NOSIGNAL) != 20) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                break;
            case PROGD:
                if ((n = recv(sockfd, (char*)ramreceived, dbg_board->DBGGetRAMSize(), MSG_WAITALL)) !=
                    (int)dbg_board->DBGGetRAMSize()) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("PROGD cmd\n");
                uram = dbg_board->DBGGetRAM_p();
                for (i = 0; i < (int)dbg_board->DBGGetRAMSize(); i++) {
                    if (ramsend[i] != ramreceived[i]) {
                        uram[i] = ramreceived[i];
                        dprint("PROGD cmd RAM %04X updated!\n", i);
                    }
                }
                break;
            case PROGP:
                if ((n = recv(sockfd, (char*)dbg_board->DBGGetROM_p(), dbg_board->DBGGetROMSize(), MSG_WAITALL)) !=
                    (int)dbg_board->DBGGetROMSize()) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
#ifdef _DEBUG_
                for (i = 0; i < (int)dbg_board->DBGGetROMSize(); i++)
                    printf("%#02X ", dbg_board->DBGGetROM_p()[i]);
#endif
                dprint("PROGP cmd  %i of %i\n", n, dbg_board->DBGGetROMSize());
                break;
            case PROGC:
                if ((n = recv(sockfd, (char*)dbg_board->DBGGetCONFIG_p(), dbg_board->DBGGetCONFIGSize(),
                              MSG_WAITALL)) != (int)dbg_board->DBGGetCONFIGSize()) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
#ifdef _DEBUG_
                for (i = 0; i < (int)dbg_board->DBGGetCONFIGSize(); i++)
                    printf("%#02X ", dbg_board->DBGGetCONFIG_p()[i]);
#endif
                dprint("PROGC cmd  %i of %i\n", n, dbg_board->DBGGetCONFIGSize());
                break;
            case PROGI:
                if ((n = recv(sockfd, (char*)dbg_board->DBGGetID_p(), dbg_board->DBGGetIDSize(), MSG_WAITALL)) !=
                    (int)dbg_board->DBGGetIDSize()) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
#ifdef _DEBUG_
                for (i = 0; i < (int)dbg_board->DBGGetIDSize(); i++)
                    printf("%#02X ", dbg_board->DBGGetID_p()[i]);
#endif
                dprint("PROGI cmd\n");
                break;
            case PROGE:
                if ((n = recv(sockfd, (char*)dbg_board->DBGGetEEPROM_p(), dbg_board->DBGGetEEPROM_Size(),
                              MSG_WAITALL)) != (int)dbg_board->DBGGetEEPROM_Size()) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
#ifdef _DEBUG_
                for (i = 0; i < (int)dbg_board->DBGGetEEPROM_Size(); i++)
                    printf("%#02X ", dbg_board->DBGGetEEPROM_p()[i]);
#endif
                dprint("PROGE cmd\n");
                break;
            case READD:
                memcpy(ramsend, dbg_board->DBGGetRAM_p(), dbg_board->DBGGetRAMSize());
                if (send(sockfd, (char*)ramsend, dbg_board->DBGGetRAMSize(), MSG_NOSIGNAL) !=
                    (int)dbg_board->DBGGetRAMSize()) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("READD cmd  size=0x%04X, ret= %i\n", dbg_board->DBGGetRAMSize(), ret);
                break;
            case READDV:
                if ((n = recv(sockfd, (char*)&dbuff, 4, MSG_WAITALL)) != 4) {
                    printf("mplabxd: receive error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("address=%02X  values=%i \n", dbuff[0], dbuff[1]);
                if (send(sockfd, (char*)&dbg_board->DBGGetRAM_p()[dbuff[0]], dbuff[1], MSG_NOSIGNAL) != dbuff[1]) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("READDV cmd\n");
                break;
            case READP:
                if (send(sockfd, (const char*)dbg_board->DBGGetROM_p(), dbg_board->DBGGetROMSize(), MSG_NOSIGNAL) !=
                    (int)dbg_board->DBGGetROMSize()) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("READP cmd\n");
                break;
            case READC:
                if (send(sockfd, (const char*)dbg_board->DBGGetCONFIG_p(), dbg_board->DBGGetCONFIGSize(),
                         MSG_NOSIGNAL) != (int)dbg_board->DBGGetCONFIGSize()) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("READC cmd\n");
                break;
            case READI:
                if (send(sockfd, (char*)dbg_board->DBGGetID_p(), dbg_board->DBGGetIDSize(), MSG_NOSIGNAL) !=
                    (int)dbg_board->DBGGetIDSize()) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                }
                dprint("READI cmd\n");
                break;
            case READE:
                if (send(sockfd, (char*)dbg_board->DBGGetEEPROM_p(), dbg_board->DBGGetEEPROM_Size(), MSG_NOSIGNAL) !=
                    (int)dbg_board->DBGGetEEPROM_Size()) {
                    printf("mplabxd: send error : %s \n", strerror(errno));
                    ret = 1;
                    reply = 0x01;
                };
                dprint("READI cmd\n");
                break;
            default:
                dprint("UNKNOWN cmd !!!!!!!!!!!!!\n");
                break;
        }

        if (send(sockfd, (char*)&reply, 1, MSG_NOSIGNAL) != 1) {
            printf("mplabxd: send error : %s \n", strerror(errno));
            ret = 1;
        }

        setnblock(sockfd);
    }

    // close connection
    if (ret)
        mplabxd_stop();

    return ret;
}
