/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#define _USE_MATH_DEFINES

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
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/unistd.h>
#else
#include <winsock2.h>
#ifndef MSG_WAITALL
#define MSG_WAITALL (1 << 3)
#endif
#define MSG_NOSIGNAL 0
#endif
// system headers independent
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../devices/lcd_hd44780.h"
#include "../devices/vterm.h"
#include "picsimlab.h"
#include "rcontrol.h"
#include "spareparts.h"

static int sockfd = -1;
static int listenfd = -1;
static int server_started = 0;

#define BSIZE 1024
static char buffer[BSIZE];
static int bp = 0;
static char file_to_load[BSIZE];

void setnblock(int sock_descriptor) {
#ifndef _WIN_
    int flags;
    /* Set socket to non-blocking */

    if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0) {
        /* Handle error */
        // printf("Error fcntl nblock !!!!!!!\n");
    }

    if (fcntl(sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0) {
        /* Handle error */
        // printf("Error fcntl nblock !!!!!!!\n");
    }
#else
    unsigned long iMode = 1;
    ioctlsocket(sock_descriptor, FIONBIO, &iMode);
#endif
}

void setblock(int sock_descriptor) {
#ifndef _WIN_
    int flags;
    /* Set socket to blocking */

    if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0) {
        /* Handle error */
        // printf("Error fcntl block !!!!!!!\n");
    }

    if (fcntl(sock_descriptor, F_SETFL, flags & (~O_NONBLOCK)) < 0) {
        /* Handle error */
        // printf("Error fcntl block !!!!!!!\n");
    }
#else
    unsigned long iMode = 0;
    ioctlsocket(sock_descriptor, FIONBIO, &iMode);
#endif
}

char* rcontrol_get_file_to_load(void) {
    return file_to_load;
}

int rcontrol_init(const unsigned short tcpport, const int reporterror) {
    struct sockaddr_in serv;

    if (!server_started) {
        dprint("rcontrol: init\n");

        if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("rcontrol: socket error : %s \n", strerror(errno));
            return 1;
        };
        /*
                int reuse = 1;
                if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
                    perror("rcontrol: setsockopt(SO_REUSEADDR) failed");
        */
        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
        serv.sin_port = htons(tcpport);

        if (bind(listenfd, (sockaddr*)&serv, sizeof(serv))) {
            if (reporterror) {
                printf("rcontrol: bind error : %s \n", strerror(errno));
                char stemp[100];
                snprintf(stemp, 100,
                         "Can't open rcontrol TCP port %i\n It is already "
                         "in use by another application!",
                         tcpport);
                PICSimLab.RegisterError(stemp);
            }
            return 1;
        }

        if (listen(listenfd, SOMAXCONN)) {
            printf("rcontrol: listen error : %s \n", strerror(errno));
            return 1;
        }
        setnblock(listenfd);
        server_started = 1;
    }
    return 0;
}

static int sendtext(const char* str) {
    int size = strlen(str);

    if (send(sockfd, str, size, MSG_NOSIGNAL) != size) {
        printf("rcontrol: send error : %s \n", strerror(errno));
        return 1;
    }

    return 0;
}

int rcontrol_start(void) {
    struct sockaddr_in cli;
#ifndef _WIN_
    unsigned int clilen;
#else
    int clilen;
#endif
    clilen = sizeof(cli);

    if (!server_started) {
        return 1;
    }

    if ((sockfd = accept(listenfd, (sockaddr*)&cli, &clilen)) < 0) {
        return 1;
    }

    setnblock(sockfd);
    dprint("rcontrol: Client connected!---------------------------------\n");

    memset(buffer, 0, BSIZE);
    bp = 0;

    return sendtext(
        "\r\nPICSimLab Remote Control Interface\r\n\r\n  Type help "
        "to see supported commands\r\n\r\n>");
}

void rcontrol_stop(void) {
    dprint("rcontrol: Client disconnected!---------------------------------\n");
    if (sockfd >= 0) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
    sockfd = -1;
}

void rcontrol_end(void) {
    rcontrol_stop();
    dprint("rcontrol: end\n");
}

void rcontrol_server_end(void) {
    if (server_started) {
        server_started = 0;
        dprint("rcontrol: server end\n");
        shutdown(listenfd, SHUT_RDWR);
        close(listenfd);
        listenfd = -1;
    }
}

static char decodess(unsigned char v) {
    switch (v & 0x7F) {
        case 0x00:
            return '_';
            break;
        case 0x3F:
            return '0';
        case 0x06:
            return '1';
        case 0x5B:
            return '2';
        case 0x4F:
            return '3';
        case 0x66:
            return '4';
        case 0x6D:
            return '5';
        case 0x7D:
            return '6';
        case 0x07:
            return '7';
        case 0x7F:
            return '8';
        case 0x6F:
            return '9';
        case 0x77:
            return 'A';
        case 0x7C:
            return 'B';
        case 0x58:
        case 0x39:
            return 'C';
        case 0x5E:
            return 'D';
        case 0x79:
            return 'E';
        case 0x71:
            return 'F';
        default:
            return 'i';
    }
}
#define VTBUFFMAX SBUFFMAX
static int Vtcount_in = 0;
unsigned char Vtbuff_in[VTBUFFMAX + 1];

void VtReceiveCallback(unsigned char data) {
    if (Vtcount_in < (VTBUFFMAX - 1)) {
        Vtbuff_in[Vtcount_in] = data;
        Vtcount_in++;
        Vtbuff_in[Vtcount_in] = 0;
    }
}

static int type_is_equal(const char* name, const char* type) {
    return ((name[0] == type[0]) && (name[1] == type[1]));
}

static void ProcessInput(const char* msg, input_t* Input, int* ret) {
    char lstemp[200];
    if (type_is_equal(Input->name, "VS")) {
        short temp = ((*((unsigned char*)Input->status)) << 8) | (*(((unsigned char*)(Input->status)) + 1));
        snprintf(lstemp, 199, "%s %s= %i\r\n", msg, Input->name, temp);
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Input->name, "PB") || type_is_equal(Input->name, "KB") ||
               type_is_equal(Input->name, "PO") || type_is_equal(Input->name, "JP")) {
        snprintf(lstemp, 199, "%s %s= %i\r\n", msg, Input->name, *((unsigned char*)Input->status));
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Input->name, "VT")) {
        vterm_t* vt = (vterm_t*)Input->status;
        if (!vt->ReceiveCallback) {
            vt->ReceiveCallback = VtReceiveCallback;
        }
        snprintf(lstemp, 199, "%s %s= %3i\r\n", msg, Input->name, vt->count_in);
        *ret += sendtext(lstemp);
    } else {
        snprintf(lstemp, 199, "%s %s= Unknow type!\r\n", msg, Input->name);
        *ret += sendtext(lstemp);
    }
}

static void ProcessOutput(const char* msg, output_t* Output, int* ret, int full = 0) {
    char lstemp[SBUFFMAX + 256];
    static unsigned char ss = 0;  // seven segment

    if (type_is_equal(Output->name, "LD")) {
        snprintf(lstemp, 199, "%s %s= %3.0f\r\n", msg, Output->name, *((float*)Output->status) - 55);
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Output->name, "DS")) {
        lcd_t* lcd = (lcd_t*)Output->status;
        char lbuff[81];
        memcpy(lbuff, lcd->ddram_char, 80);
        lbuff[80] = 0;
        for (int i = 0; i < 80; i++) {
            if (lbuff[i] < 16)
                lbuff[i] = '*';
        }
        snprintf(lstemp, 199, "%s %s= |%.16s\r\n", msg, Output->name, &lbuff[0]);

        int size = strlen(lstemp) - 19;
        for (int x = 0; x < size; x++) {
            lstemp[x + size + 19] = ' ';
        }
        snprintf(lstemp + (2 * size + 19), 199, "|%.16s\r\n", &lbuff[40]);
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Output->name, "MT")) {
        unsigned char** status = (unsigned char**)Output->status;
        snprintf(lstemp, 199, "%s %s-> dir= %i speed= %3i position= %3i\r\n", msg, Output->name, *status[0], *status[1],
                 *status[2]);
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Output->name, "DG")) {
        snprintf(lstemp, 199, "%s %s-> angle= %5.1f\r\n", msg, Output->name, *((float*)Output->status) * 180.0 / M_PI);
        *ret += sendtext(lstemp);
    } else if (type_is_equal(Output->name, "SS")) {
        switch (Output->name[3]) {
            case 'A':
                ss = 0x00;
                if (*((int*)Output->status) > 60)
                    ss |= 0x01;
                break;
            case 'B':
                if (*((int*)Output->status) > 60)
                    ss |= 0x02;
                break;
            case 'C':
                if (*((int*)Output->status) > 60)
                    ss |= 0x04;
                break;
            case 'D':
                if (*((int*)Output->status) > 60)
                    ss |= 0x08;
                break;
            case 'E':
                if (*((int*)Output->status) > 60)
                    ss |= 0x10;
                break;
            case 'F':
                if (*((int*)Output->status) > 60)
                    ss |= 0x20;
                break;
            case 'G':
                if (*((int*)Output->status) > 60)
                    ss |= 0x40;
                break;
            case 'P':
                if (*((int*)Output->status) > 60)
                    ss |= 0x80;
                snprintf(lstemp, 199, "%s SS_%c= %c\r\n", msg, Output->name[4], decodess(ss));
                *ret += sendtext(lstemp);
                break;
        }
    } else if (type_is_equal(Output->name, "VT")) {
        vterm_t* vt = (vterm_t*)Output->status;
        if (!vt->ReceiveCallback) {
            vt->ReceiveCallback = VtReceiveCallback;
        }
        if (full) {
            snprintf(lstemp, SBUFFMAX + 255, "%s %s= %3i\r\n%s\r\n", msg, Output->name, Vtcount_in, Vtbuff_in);
            *ret += sendtext(lstemp);
            Vtbuff_in[0] = 0;
            Vtcount_in = 0;
        } else {
            snprintf(lstemp, SBUFFMAX + 255, "%s %s= %3i\r\n", msg, Output->name, Vtcount_in);
            *ret += sendtext(lstemp);
        }
    } else {
        snprintf(lstemp, 199, "%s %s= unknow type !\r\n", msg, Output->name);
        *ret += sendtext(lstemp);
    }
}

static char pintypetoletter(unsigned char type) {
    switch (type) {
        case PT_POWER:
            return 'P';
            break;
        case PT_DIGITAL:
            return 'D';
            break;
        case PT_ANALOG:
            return 'A';
            break;
        case PT_ANAREF:
            return 'R';
            break;
        case PT_USART:
            return 'U';
            break;
        case PT_NC:
            return '-';
            break;
    }
    return '?';
}

int rcontrol_loop(void) {
    int i, j;
    int n;
    int ret = 0;
    char lstemp[200];
    board* Board;
    part* Part;
    input_t* Input;
    output_t* Output;
    const picpin* pins;

    // open connection
    if (sockfd < 0) {
        return rcontrol_start();
    }

    n = recv(sockfd, (char*)&buffer[bp], 1024 - bp, 0);

    if (n > 0) {
        // remove putty telnet handshake
        if (buffer[bp + n] == 3) {
            for (int x = 0; x < bp + n + 1; x++) {
                buffer[x] = 0;
            }
            n = 0;
            bp = 0;
        }

        if (strchr(buffer, '\n')) {
            char cmd[BSIZE];
            int cmdsize = 0;

            while (buffer[cmdsize] != '\n') {
                cmd[cmdsize] = buffer[cmdsize];
                cmdsize++;
            }
            cmd[cmdsize] = 0;

            for (int i = 0; i < BSIZE - cmdsize - 1; i++) {
                buffer[i] = buffer[i + cmdsize + 1];
            }
            bp -= cmdsize - n + 1;

            if (cmd[cmdsize - 1] == '\r') {
                cmd[cmdsize - 1] = 0;  // strip \r
            }

            // strcpy (cmd, (const char *) lowercase (cmd).c_str ());

            dprint("cmd[%s]\n", cmd);

            switch (cmd[0]) {
                case 'c':
                    if (!strncmp(cmd, "clk", 3)) {
                        // Command clk =====================================================

                        if (strlen(cmd) < 4) {
                            snprintf(lstemp, 100, "%2.1f MHz\r\nOk\r\n>", PICSimLab.GetClock());
                            ret = sendtext(lstemp);
                        } else {
                            float clk;
                            sscanf(cmd + 3, "%f", &clk);

                            PICSimLab.SetClock(clk, 0);

                            snprintf(lstemp, 100, "Set to %2.1f MHz\r\nOk\r\n>", PICSimLab.GetClock());
                            ret = sendtext(lstemp);
                        }
                    }
                    break;
                case 'd':
                    if (strstr(cmd, "dumpr")) {
                        // Command dumpr
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        unsigned int addr;
                        unsigned int size;
                        int ret = sscanf(cmd + 5, "%x %u \n", &addr, &size);

                        if (ret == -1)  // all
                        {
                            for (unsigned int i = 0; i < Board->DBGGetRAMSize(); i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);
                                for (int j = 0; j < 16; j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetRAM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }
                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        } else if (ret == 1)  // only one addr
                        {
                            if (addr < Board->DBGGetRAMSize()) {
                                snprintf(lstemp, 100, "%04X: %02X \r\nOk\r\n>", addr, Board->DBGGetRAM_p()[addr]);
                                ret += sendtext(lstemp);
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else  // vector from addr
                        {
                            for (unsigned int i = addr; (i < (addr + size)) && i < Board->DBGGetRAMSize(); i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);

                                for (unsigned int j = 0;
                                     (j < 16) && (j < size - (i - addr)) && (i + j) < Board->DBGGetRAMSize(); j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetRAM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }

                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        }
                    } else if (strstr(cmd, "dumpe")) {
                        // Command dumpe
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        unsigned int addr;
                        unsigned int size;
                        int ret = sscanf(cmd + 5, "%x %u \n", &addr, &size);

                        if (ret == -1)  // all
                        {
                            for (unsigned int i = 0; i < Board->DBGGetEEPROM_Size(); i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);
                                for (int j = 0; j < 16; j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetEEPROM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }
                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        } else if (ret == 1)  // only one addr
                        {
                            if (addr < Board->DBGGetEEPROM_Size()) {
                                snprintf(lstemp, 100, "%04X: %02X \r\nOk\r\n>", addr, Board->DBGGetEEPROM_p()[addr]);
                                ret += sendtext(lstemp);
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else  // vector from addr
                        {
                            for (unsigned int i = addr; (i < (addr + size)) && i < Board->DBGGetEEPROM_Size();
                                 i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);

                                for (unsigned int j = 0;
                                     (j < 16) && (j < size - (i - addr)) && (i + j) < Board->DBGGetEEPROM_Size(); j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetEEPROM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }

                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        }
                    } else if (strstr(cmd, "dumpf")) {
                        // Command dumpf
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        unsigned int addr;
                        unsigned int size;
                        int ret = sscanf(cmd + 5, "%x %u \n", &addr, &size);

                        if (ret == -1)  // all
                        {
                            for (unsigned int i = 0; i < Board->DBGGetROMSize(); i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);
                                for (int j = 0; j < 16; j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetROM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }
                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        } else if (ret == 1)  // only one addr
                        {
                            if (addr < Board->DBGGetROMSize()) {
                                snprintf(lstemp, 100, "%04X: %02X \r\nOk\r\n>", addr, Board->DBGGetROM_p()[addr]);
                                ret += sendtext(lstemp);
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else  // vector from addr
                        {
                            for (unsigned int i = addr; (i < (addr + size)) && i < Board->DBGGetROMSize(); i += 16) {
                                snprintf(lstemp, 100, "%04X: ", i);
                                ret += sendtext(lstemp);

                                for (unsigned int j = 0;
                                     (j < 16) && (j < size - (i - addr)) && (i + j) < Board->DBGGetROMSize(); j++) {
                                    snprintf(lstemp, 100, "%02X ", Board->DBGGetROM_p()[j + i]);
                                    ret += sendtext(lstemp);
                                }

                                snprintf(lstemp, 100, "\r\n");
                                ret += sendtext(lstemp);
                            }
                            snprintf(lstemp, 100, "\r\nOk\r\n>");
                            ret += sendtext(lstemp);
                        }
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'e':
                    if (!strcmp(cmd, "exit")) {
                        // Command exit
                        // ========================================================
                        sendtext("Ok\r\n>");
                        PICSimLab.SetWorkspaceFileName("");
                        PICSimLab.SetToDestroy();
                        return 0;
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'g':
                    if (!strncmp(cmd, "get ", 4)) {
                        // Command
                        // get==========================================================
                        char* ptr;
                        char* ptr2;
                        Board = PICSimLab.GetBoard();

                        if ((ptr = strstr(cmd, " board.in["))) {
                            int in = (ptr[10] - '0') * 10 + (ptr[11] - '0');

                            if (in < Board->GetInputCount()) {
                                Input = Board->GetInput(in);

                                if (Input->status != NULL) {
                                    snprintf(lstemp, 100, "board.in[%02i]", in);
                                    ProcessInput(lstemp, Input, &ret);
                                    sendtext("Ok\r\n>");
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else if ((ptr = strstr(cmd, " board.out["))) {
                            int out = (ptr[11] - '0') * 10 + (ptr[12] - '0');

                            if (out < Board->GetOutputCount()) {
                                Output = Board->GetOutput(out);

                                if (Output->status != NULL) {
                                    snprintf(lstemp, 100, "board.out[%02i]", out);
                                    ProcessOutput(lstemp, Output, &ret, 1);
                                    sendtext("Ok\r\n>");
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else if ((ptr = strstr(cmd, " apin["))) {
                            int pin = (ptr[6] - '0') * 10 + (ptr[7] - '0');
                            if (Board->GetUseSpareParts()) {
                                pins = SpareParts.GetPinsValues();
                            } else {
                                Board = PICSimLab.GetBoard();
                                pins = Board->MGetPinsValues();
                            }
                            snprintf(lstemp, 100, "apin[%02i]= %5.3f \r\nOk\r\n>", pin, pins[pin - 1].avalue);
                            ret = sendtext(lstemp);
                        } else if ((ptr = strstr(cmd, " pin["))) {
                            int pin = (ptr[5] - '0') * 10 + (ptr[6] - '0');
                            if (Board->GetUseSpareParts()) {
                                pins = SpareParts.GetPinsValues();
                            } else {
                                Board = PICSimLab.GetBoard();
                                pins = Board->MGetPinsValues();
                            }
                            snprintf(lstemp, 100, "pin[%02i]= %i \r\nOk\r\n>", pin, pins[pin - 1].value);
                            sendtext(lstemp);
                        } else if ((ptr = strstr(cmd, " pinl["))) {
                            int pin = (ptr[6] - '0') * 10 + (ptr[7] - '0');
                            if (Board->GetUseSpareParts()) {
                                pins = SpareParts.GetPinsValues();
                            } else {
                                Board = PICSimLab.GetBoard();
                                pins = Board->MGetPinsValues();
                            }
                            snprintf(lstemp, 100, "pin[%02i] %c %c %i %03i %5.3f \"%-8s\" \r\nOk\r\n>", pin,
                                     pintypetoletter(pins[pin - 1].ptype), (pins[pin - 1].dir == PD_IN) ? 'I' : 'O',
                                     pins[pin - 1].value, (int)(pins[pin - 1].oavalue - 55), pins[pin - 1].avalue,
                                     (const char*)Board->MGetPinName(pin).c_str());
                            ret = sendtext(lstemp);
                        } else if ((ptr = strstr(cmd, " pinm["))) {
                            int pin = (ptr[6] - '0') * 10 + (ptr[7] - '0');
                            if (Board->GetUseSpareParts()) {
                                pins = SpareParts.GetPinsValues();
                            } else {
                                Board = PICSimLab.GetBoard();
                                pins = Board->MGetPinsValues();
                            }
                            snprintf(lstemp, 100, "pin[%02i] %03i\r\nOk\r\n>", pin, (int)(pins[pin - 1].oavalue - 55));
                            ret = sendtext(lstemp);
                        } else if (Board->GetUseSpareParts()) {
                            if ((ptr = strstr(cmd, "part[")) && (ptr2 = strstr(cmd, "].in["))) {
                                int pn = (ptr[5] - '0') * 10 + (ptr[6] - '0');
                                int in = (ptr2[5] - '0') * 10 + (ptr2[6] - '0');

                                if (pn < SpareParts.GetCount()) {
                                    Part = SpareParts.GetPart(pn);
                                    if (in < Part->GetInputCount()) {
                                        Input = Part->GetInput(in);

                                        if (Input->status != NULL) {
                                            snprintf(lstemp, 100, "part[%02i].in[%02i]", pn, in);
                                            ProcessInput(lstemp, Input, &ret);
                                            sendtext("Ok\r\n>");
                                        } else {
                                            ret = sendtext("ERROR\r\n>");
                                        }
                                    } else {
                                        ret = sendtext("ERROR\r\n>");
                                    }
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else if ((ptr = strstr(cmd, "part[")) && (ptr2 = strstr(cmd, "].out["))) {
                                int pn = (ptr[5] - '0') * 10 + (ptr[6] - '0');
                                int out = (ptr2[6] - '0') * 10 + (ptr2[7] - '0');

                                if (pn < SpareParts.GetCount()) {
                                    Part = SpareParts.GetPart(pn);
                                    if (out < Part->GetOutputCount()) {
                                        Output = Part->GetOutput(out);

                                        if (Output->status != NULL) {
                                            snprintf(lstemp, 100, "part[%02i].out[%02i]", pn, out);
                                            ProcessOutput(lstemp, Output, &ret, 1);
                                            sendtext("Ok\r\n>");
                                        } else {
                                            ret = sendtext("ERROR\r\n>");
                                        }
                                    } else {
                                        ret = sendtext("ERROR\r\n>");
                                    }
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else {
                            ret = sendtext("ERROR\r\n>");
                        }
                        return 0;
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'h':
                    if (!strcmp(cmd, "help")) {
                        // Command help
                        // ========================================================
                        ret += sendtext("List of supported commands:\r\n");
                        ret += sendtext("  clk [val MHz]- show or set simulation clock\r\n");
                        ret += sendtext("  dumpe [a] [s]- dump internal EEPROM memory\r\n");
                        ret += sendtext("  dumpf [a] [s]- dump Flash memory\r\n");
                        ret += sendtext("  dumpr [a] [s]- dump RAM memory\r\n");
                        ret += sendtext("  exit         - shutdown PICSimLab\r\n");
                        ret += sendtext("  get ob       - get object value\r\n");
                        ret += sendtext("  help         - show this message\r\n");
                        ret += sendtext("  info         - show actual setup info and objects\r\n");
                        ret += sendtext("  loadhex file - load hex file (use full path)\r\n");
                        ret += sendtext("  pins         - show pins directions and values\r\n");
                        ret += sendtext("  pinsl        - show pins formated info\r\n");
                        ret += sendtext("  quit         - exit remote control interface\r\n");
                        ret += sendtext("  reset        - reset the board\r\n");
                        ret += sendtext("  set ob vl    - set object with value\r\n");
                        ret += sendtext(
                            "  sim [cmd]    - show simulation status or execute "
                            "cmd start/stop\r\n");
                        ret += sendtext("  sync         - wait to syncronize with timer event\r\n");
                        ret += sendtext("  version      - show PICSimLab version\r\n");

                        ret += sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'i':
                    if (!strcmp(cmd, "info")) {
                        // Command info
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        snprintf(lstemp, 100, "Board:     %s\r\n", (const char*)Board->GetName().c_str());
                        ret += sendtext(lstemp);
                        snprintf(lstemp, 100, "Processor: %s\r\n", (const char*)Board->GetProcessorName().c_str());
                        ret += sendtext(lstemp);
                        snprintf(lstemp, 100, "Frequency: %10.0f Hz\r\n", Board->MGetFreq());
                        ret += sendtext(lstemp);
                        snprintf(lstemp, 100, "Use Spare: %i\r\n", Board->GetUseSpareParts());
                        ret += sendtext(lstemp);

                        for (i = 0; i < Board->GetInputCount(); i++) {
                            Input = Board->GetInput(i);
                            if ((Input->status != NULL)) {
                                snprintf(lstemp, 100, "    board.in[%02i]", i);
                                ProcessInput(lstemp, Input, &ret);
                            }
                        }

                        for (i = 0; i < Board->GetOutputCount(); i++) {
                            Output = Board->GetOutput(i);
                            if (Output->status != NULL) {
                                snprintf(lstemp, 100, "    board.out[%02i]", i);
                                ProcessOutput(lstemp, Output, &ret);
                            }
                        }

                        if (Board->GetUseSpareParts()) {
                            for (i = 0; i < SpareParts.GetCount(); i++) {
                                Part = SpareParts.GetPart(i);
                                snprintf(lstemp, 100, "  part[%02i]: %s\r\n", i, (const char*)Part->GetName().c_str());
                                ret += sendtext(lstemp);

                                for (j = 0; j < Part->GetInputCount(); j++) {
                                    Input = Part->GetInput(j);
                                    if (Input->status != NULL) {
                                        snprintf(lstemp, 100, "    part[%02i].in[%02i]", i, j);
                                        ProcessInput(lstemp, Input, &ret);
                                    }
                                }
                                for (j = 0; j < Part->GetOutputCount(); j++) {
                                    Output = Part->GetOutput(j);
                                    if (Output->status != NULL) {
                                        snprintf(lstemp, 100, "    part[%02i].out[%02i]", i, j);
                                        ProcessOutput(lstemp, Output, &ret);
                                    }
                                }
                            }
                        }
                        ret += sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'l':
                    if (!strncmp(cmd, "loadhex", 7)) {
                        // Command loadhex
                        // ========================================================
                        char* ptr;
                        if ((ptr = strchr(cmd, '\r'))) {
                            ptr[0] = 0;
                        }
                        if ((ptr = strchr(cmd, '\n'))) {
                            ptr[0] = 0;
                        }

                        if (PICSimLab.GetNeedReboot()) {
                            PICSimLab.SetWorkspaceFileName("");
                            PICSimLab.SetToDestroy(RC_LOAD);
                            ret += sendtext("Ok\r\n>");
                            strcpy(file_to_load, cmd + 8);
                            ret = 1;
                        } else {
                            if (PICSimLab.LoadHexFile(cmd + 8)) {
                                ret += sendtext("ERROR\r\n>");
                            } else {
                                ret += sendtext("Ok\r\n>");
                            }
                        }
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'p':
                    if (!strcmp(cmd, "pins")) {
                        // Command pins
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        pins = Board->MGetPinsValues();
                        int p2 = Board->MGetPinCount() / 2;
                        for (i = 0; i < p2; i++) {
                            snprintf(lstemp, 100,
                                     "  pin[%02i] (%8.8s) %c %i                 pin[%02i] (%-8.8s) %c %i "
                                     "\r\n",
                                     i + 1, (const char*)Board->MGetPinName(i + 1).c_str(),
                                     (pins[i].dir == PD_IN) ? '<' : '>', pins[i].value, i + 1 + p2,
                                     (const char*)Board->MGetPinName(i + 1 + p2).c_str(),
                                     (pins[i + p2].dir == PD_IN) ? '<' : '>', pins[i + p2].value);
                            ret += sendtext(lstemp);
                        }
                        ret += sendtext("Ok\r\n>");
                    } else if (!strcmp(cmd, "pinsl")) {
                        // Command pinsl
                        // ========================================================
                        Board = PICSimLab.GetBoard();
                        pins = Board->MGetPinsValues();
                        snprintf(lstemp, 100, "%i pins [%s]:\r\n", Board->MGetPinCount(),
                                 (const char*)Board->GetProcessorName().c_str());
                        ret += sendtext(lstemp);
                        for (i = 0; i < Board->MGetPinCount(); i++) {
                            snprintf(lstemp, 100, "  pin[%02i] %c %c %i %03i %5.3f \"%-8.8s\" \r\n", i + 1,
                                     pintypetoletter(pins[i].ptype), (pins[i].dir == PD_IN) ? 'I' : 'O', pins[i].value,
                                     (int)(pins[i].oavalue - 55), pins[i].avalue,
                                     (const char*)Board->MGetPinName(i + 1).c_str());
                            ret += sendtext(lstemp);
                        }
                        ret += sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'q':
                    if (!strcmp(cmd, "quit")) {
                        // Command quit
                        // ========================================================
                        sendtext("Ok\r\n>");
                        ret = 1;
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'r':
                    if (!strcmp(cmd, "reset")) {
                        // Command reset
                        // =======================================================
                        PICSimLab.GetBoard()->MReset(0);
                        ret = sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 's':
                    if (!strncmp(cmd, "set ", 4)) {
                        // Command set
                        // =========================================================
                        char* ptr;
                        char* ptr2;
                        Board = PICSimLab.GetBoard();

                        if ((ptr = strstr(cmd, " board.in["))) {
                            int in = (ptr[10] - '0') * 10 + (ptr[11] - '0');
                            int value;

                            sscanf(ptr + 13, "%i", &value);

                            dprint("board.in[%02i] = %i \r\n", in, value);

                            if (in < Board->GetInputCount()) {
                                Input = Board->GetInput(in);

                                if (Input->status != NULL) {
                                    *((unsigned char*)Input->status) = value;
                                    sendtext("Ok\r\n>");
                                    if (Input->update) {
                                        *Input->update = 1;
                                    }
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else if ((ptr = strstr(cmd, " apin["))) {
                            int pin = (ptr[6] - '0') * 10 + (ptr[7] - '0');
                            float value;

                            sscanf(ptr + 9, "%f", &value);

                            dprint("apin[%02i] = %f \r\n", pin, value);

                            if (Board->GetUseSpareParts()) {
                                SpareParts.SetAPin(pin, value);
                            } else {
                                Board = PICSimLab.GetBoard();
                                Board->MSetAPin(pin, value);
                            }
                            sendtext("Ok\r\n>");
                        } else if ((ptr = strstr(cmd, " pin["))) {
                            int pin = (ptr[5] - '0') * 10 + (ptr[6] - '0');
                            int value;

                            sscanf(ptr + 8, "%i", &value);

                            dprint("pin[%02i] = %i \r\n", pin, value);

                            Board->IoLockAccess();
                            if (Board->GetUseSpareParts()) {
                                SpareParts.SetPin(pin, value);
                            } else {
                                Board = PICSimLab.GetBoard();
                                Board->MSetPin(pin, value);
                            }
                            Board->IoUnlockAccess();
                            sendtext("Ok\r\n>");
                        } else if (Board->GetUseSpareParts() && (ptr = strstr(cmd, "part[")) &&
                                   (ptr2 = strstr(cmd, "].in["))) {
                            int pn = (ptr[5] - '0') * 10 + (ptr[6] - '0');
                            int in = (ptr2[5] - '0') * 10 + (ptr2[6] - '0');
                            int value;

                            sscanf(ptr2 + 8, "%i", &value);

                            dprint("part[%02i].in[%02i] = %i \r\n", pn, in, value);

                            if (pn < SpareParts.GetCount()) {
                                Part = SpareParts.GetPart(pn);

                                if (in < Part->GetInputCount()) {
                                    Input = Part->GetInput(in);

                                    if (Input->status != NULL) {
                                        if (type_is_equal(Input->name, "VS")) {
                                            *((unsigned char*)Input->status) = (value & 0xFF00) >> 8;
                                            *(((unsigned char*)Input->status) + 1) = value & 0x00FF;
                                        } else if (type_is_equal(Input->name, "PB") ||
                                                   type_is_equal(Input->name, "KB") ||
                                                   type_is_equal(Input->name, "PO") ||
                                                   type_is_equal(Input->name, "JP")) {
                                            *((unsigned char*)Input->status) = value;
                                        } else if (type_is_equal(Input->name, "VT")) {
                                            vterm_t* vt = (vterm_t*)Input->status;
                                            if (!vt->ReceiveCallback) {
                                                vt->ReceiveCallback = VtReceiveCallback;
                                            }
                                            const char* sval = ptr2 + 9;
                                            strcpy((char*)vt->buff_out, sval);
                                            vt->count_out = strlen(sval);
                                        }
                                        if (Input->update) {
                                            *Input->update = 1;
                                        }
                                        sendtext("Ok\r\n>");
                                    } else {
                                        ret = sendtext("ERROR\r\n>");
                                    }
                                } else {
                                    ret = sendtext("ERROR\r\n>");
                                }
                            } else {
                                ret = sendtext("ERROR\r\n>");
                            }
                        } else {
                            ret = sendtext("ERROR\r\n>");
                        }
                        return 0;
                    } else if (!strncmp(cmd, "sim", 3)) {
                        // Command sim =====================================================
                        PICSimLab.SetSync(0);

                        if (strstr(cmd + 3, "stop")) {
                            PICSimLab.SetSimulationRun(0);
                            ret = sendtext("Ok\r\n>");
                        } else if (strstr(cmd + 3, "start")) {
                            PICSimLab.SetSimulationRun(1);
                            ret = sendtext("Ok\r\n>");
                        } else {
                            if (PICSimLab.GetSimulationRun()) {
                                int time;
                                PICSimLab.WindowCmd(PW_MAIN, "timer1", PWA_TIMERGETTIME, NULL, &time);
                                ret = sendtext(
                                    FloatStrFormat("Simulation running %5.2fx\r\nOk\r\n>", 100.0 / time).c_str());
                            } else {
                                ret = sendtext("Simulation stopped\r\nOk\r\n>");
                            }
                        }

                    } else if (!strcmp(cmd, "sync")) {
                        // Command sync =====================================================
                        PICSimLab.SetSync(0);
                        while (!PICSimLab.GetSync()) {
                            usleep(1);  // FIXME avoid use of usleep to reduce cpu usage
                        }
                        ret = sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                case 'v':
                    if (!strcmp(cmd, "version")) {
                        // Command version
                        // =====================================================
                        snprintf(lstemp, 100,
                                 "Developed by L.C. Gamboa\r\n "
                                 "<lcgamboa@yahoo.com>\r\n Version: %s %s %s %s\r\n",
                                 _VERSION_, _DATE_, _ARCH_, _PKG_);
                        ret += sendtext(lstemp);
                        ret += sendtext("Ok\r\n>");
                    } else {
                        ret = sendtext("ERROR\r\n>");
                    }
                    break;
                default:
                    // Uknown command
                    // ========================================================
                    ret = sendtext("ERROR\r\n>");
                    break;
            }
        } else {
            bp += n;
            if (bp > BSIZE)
                bp = BSIZE;
        }
    } else {
        // socket close by client
        if (n < 0) {
#ifndef _WIN_
            if (errno != EAGAIN)
#else
            if (WSAGetLastError() != WSAEWOULDBLOCK)
#endif
            {
                ret = 1;  // recv ERROR
            } else {
                ret = 0;  // recv no data
            }
        } else {
            ret = 0;  // recv no data
        }
    }

    // close connection
    if (ret) {
        sleep(1);  // wait client to close socket first
        rcontrol_stop();
    }

    return ret;
}
