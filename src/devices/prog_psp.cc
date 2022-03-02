/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

// Picstart+ serial programmer support

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN_
//#include <conio.h>
//#include <time.h>
//#include <windows.h>
#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#endif

#include "picsimlab1.h"

/*-------------------serial------------------------------------*/

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

char PROGDEVICE[100];

#ifdef _WIN_
HANDLE phCom;
#define BAUDRATE 19200
#else
int pfd;
#define BAUDRATE B19200
#endif

int prog_sopen(const char* port) {
#ifdef _WIN_

    phCom = CreateFile(lxString(port).c_str(), GENERIC_READ | GENERIC_WRITE,
                       0,     // exclusive access
                       NULL,  // no security
                       OPEN_EXISTING,
                       0,      // no overlapped I/O
                       NULL);  // null template

    if (phCom == INVALID_HANDLE_VALUE) {
        return 1;
    }

#else
    pfd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (pfd < 0) {
        perror(port);
        return 1;
    }
#endif
    return 0;
}

int prog_sclose(void) {
#ifdef _WIN_
    CloseHandle(phCom);
#else
    if (pfd >= 0) {
        close(pfd);
        pfd = -1;
    }
#endif
    return 0;
}

int prog_scfg(void) {
#ifdef _WIN_
    BOOL bPortReady;
    DCB dcb;
    COMMTIMEOUTS CommTimeouts;

    bPortReady = GetCommState(phCom, &dcb);

    dcb.BaudRate = BAUDRATE;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fAbortOnError = TRUE;

    // set XON/XOFF
    dcb.fOutX = FALSE;  // XON/XOFF off for transmit
    dcb.fInX = FALSE;   // XON/XOFF off for receive
    // set RTSCTS
    dcb.fOutxCtsFlow = FALSE;                 // turn off CTS flow control
    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;  //
    // set DSRDTR
    dcb.fOutxDsrFlow = FALSE;  // turn off DSR flow control
    // dcb.fDtrControl = DTR_CONTROL_ENABLE; //
    dcb.fDtrControl = DTR_CONTROL_DISABLE;  //
    // dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; //

    bPortReady = SetCommState(phCom, &dcb);

    // Communication timeouts are optional

    bPortReady = GetCommTimeouts(phCom, &CommTimeouts);

    CommTimeouts.ReadIntervalTimeout = MAXDWORD;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;

    bPortReady = SetCommTimeouts(phCom, &CommTimeouts);

    EscapeCommFunction(phCom, SETRTS);

#else
    struct termios newtio;
    int cmd;

    //        tcgetattr(fd,&oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | IGNBRK;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

    tcflush(pfd, TCIFLUSH);
    tcsetattr(pfd, TCSANOW, &newtio);

    cmd = TIOCM_RTS;
    ioctl(pfd, TIOCMBIS, &cmd);
#endif
    return 0;
}

unsigned long prog_ssend(unsigned char c) {
#ifdef _WIN_
    unsigned long nbytes;

    WriteFile(phCom, &c, 1, &nbytes, NULL);
    return nbytes;
#else
    return write(pfd, &c, 1);
#endif
}

unsigned long prog_srec(unsigned char* c) {
#ifdef _WIN_
    unsigned long nbytes;

    if (phCom >= 0)
        ReadFile(phCom, c, 1, &nbytes, NULL);
    else
        nbytes = 0;
#else
    long nbytes;

    if (pfd >= 0) {
        nbytes = read(pfd, c, 1);
        if (nbytes < 0)
            nbytes = 0;
    } else {
        nbytes = 0;
    }
#endif
    return nbytes;
}

unsigned long prog_srec_tout(unsigned char* c) {
    unsigned int tout = 0;

#ifdef _WIN_
    unsigned long nbytes;
    do {
        Sleep(1);
        ReadFile(phCom, c, 1, &nbytes, NULL);
#else
    long nbytes;
    do {
        usleep(100);
        nbytes = read(pfd, c, 1);
        if (nbytes < 0)
            nbytes = 0;
#endif
        tout++;
    } while ((nbytes == 0) && (tout < 1000));
    return nbytes;
}

/*--------------picstart---------------------------------------------*/

#define MemoryTechnology_Flash 0x01
#define MemoryRangeType_UserId 0x02
#define MemoryRangeType_Config 0x04
#define MemoryRangeType_Eeprom 0x08
#define MemoryRangeType_Cal 0x10
#define findMask_PARITY 0x20

typedef struct {
    // memory code length 	0 1
    unsigned short mclen;
    // address word width 	2 3
    unsigned short aww;
    // data word width 		4 5
    unsigned short dww;
    // user id width		6 7
    unsigned short uiw;
    // user id mask		8 9
    unsigned short uim;
    // memory config mask		10 11 12 13
    unsigned short mcm1;
    unsigned short mcm2;
    // memory eeprom width	14 15
    unsigned short mew;
    // memory eeprom mask		16 17
    unsigned short mem;
    // memory calibration width	18 19
    unsigned short mcw;
    // memory calibration mask	20 21
    unsigned short mcam;
    // memory code start		22 23
    unsigned short mcas;
    // memory code length		24 25
    unsigned short mcl;
    // user id start		26 27
    unsigned short uis;
    // user id length		28
    unsigned char uil;
    // config start		29 30
    unsigned short cs;
    // config length		31
    unsigned char cl;
    // eeprom start		32 33
    unsigned short es;
    // eeprom length		34 35
    unsigned short el;
    // calibration start		36 37
    unsigned short cas;
    // calibration length		38 39
    unsigned short cal;
    // programming			40 41 42 43
    unsigned char overprogram;
    unsigned char tries;
    unsigned char algorithm;
    unsigned char flags;
} _info;

//-------------------------------------------icsp-----------------------------
#define TPROG 0x01
#define TEEPROM 0x02
#define TUID 0x03
#define TCONFIG 0x04

unsigned int prog_addr = 0;
unsigned int eeprom_addr = 0;
unsigned int uid_addr = 0;
unsigned int config_addr = 0;

void icsp_set_addr(_pic* pic, int type, unsigned int value) {
    switch (type) {
        case TPROG:
            prog_addr = value;
            break;
        case TEEPROM:
            eeprom_addr = value;
            break;
        case TUID:
            uid_addr = value;
            break;
        case TCONFIG:
            config_addr = value;
            break;
        default:
            break;
    }
}

unsigned short icsp_write(_pic* pic, int type, unsigned short value) {
    switch (type) {
        case TPROG:
            if (prog_addr < pic->ROMSIZE)
                pic->prog[prog_addr] = value;
            prog_addr += 1;
            break;
        case TEEPROM:
            if (eeprom_addr < pic->EEPROMSIZE)
                pic->eeprom[eeprom_addr] = value;
            eeprom_addr += 1;
            break;
        case TUID:
            if (uid_addr < pic->IDSIZE)
                pic->id[uid_addr] = value;
            uid_addr += 1;
            break;
        case TCONFIG:
            if (config_addr < pic->CONFIGSIZE)
                pic->config[config_addr] = value;
            config_addr += 1;
            break;
        default:
            break;
    }

    return value;
};

unsigned short icsp_read(_pic* pic, int type) {
    unsigned short ret = 0;

    switch (type) {
        case TPROG:
            if (prog_addr < pic->ROMSIZE)
                ret = pic->prog[prog_addr];
            prog_addr += 1;
            break;
        case TEEPROM:
            if (eeprom_addr < pic->EEPROMSIZE)
                ret = pic->eeprom[eeprom_addr];
            eeprom_addr += 1;
            break;
        case TUID:
            if (uid_addr < pic->IDSIZE)
                ret = pic->id[uid_addr];
            uid_addr += 1;
            break;
        case TCONFIG:
            if (config_addr < pic->CONFIGSIZE)
                ret = pic->config[config_addr];
            config_addr += 1;
            break;
        default:
            break;
    }
    return ret;
};

//-----------------------------------------------------------------------------------

_info info;
unsigned short ecm[16];
unsigned short size = 0;
unsigned int start = 0;

int prog_init(void) {
    int s_open = 0;

    if (PROGDEVICE[0] == 0) {
#ifndef _WIN_
        strcpy(PROGDEVICE, "/dev/tnt5");
#else
        strcpy(PROGDEVICE, "COM5");
#endif
    }

    if (prog_sopen(PROGDEVICE) == 0) {
        // printf("Port Open:%s!\n",PROGDEVICE);
        prog_scfg();
        s_open = 1;
    } else {
        // printf("#Erro Open Port:%s!\n",PROGDEVICE);
        s_open = -1;
    }

    return s_open;
}

int prog_loop(_pic* pic) {
    int i;
    int ret;
    unsigned short saux;
    unsigned char chk;
    unsigned char c;
    unsigned char aux;
    if ((ret = prog_srec(&c)) == 1) {
#ifdef _DEBUG_
        printf("%02x", c);
#endif
        Window1.statusbar1.SetField(0, lxT("Programming...."));
        switch (c) {
            case 0x42:  // Blank check full
                prog_ssend(0x42);
                chk = 0;
                aux = 0;
                icsp_set_addr(pic, TPROG, 0);
                for (i = 0; i < info.mclen; i++)
                    if (icsp_read(pic, TPROG) != info.dww)
                        aux = 1;

                if (aux)
                    chk |= 0x01;

                aux = 0;
                icsp_set_addr(pic, TEEPROM, 0);
                for (i = 0; i < info.el; i++)
                    if (icsp_read(pic, TEEPROM) != info.mem)
                        aux = 1;

                if (aux)
                    chk |= 0x08;

                aux = 0;
                icsp_set_addr(pic, TUID, 0);
                for (i = 0; i < info.uil; i++)
                    if (icsp_read(pic, TUID) != info.uim)
                        aux = 1;

                if (aux)
                    chk |= 0x04;

                aux = 0;
                icsp_set_addr(pic, TCONFIG, 0);
                for (i = 0; i < info.cl; i++)
                    if (icsp_read(pic, TCONFIG) != ecm[i])
                        aux = 1;

                if (aux)
                    chk |= 0x02;

                prog_ssend(chk);
                break;

                /*

                0x43	->	Diagnostics
                    <-	0xdd
                subcode	->
                    <-	echo


                0x44	->	Blank check partial
                    <-	0x44
                    <-	status

                */

            case 0x51:  // write program
                prog_ssend(0x51);
                icsp_set_addr(pic, TPROG, start);
                Window1.timer1.SetRunState(0);
                for (i = 0; i < size; i++) {
                    if ((start + i) >= info.mclen)
                        break;
                    prog_srec_tout(((unsigned char*)&saux) + 1);
                    prog_srec_tout(((unsigned char*)&saux));
                    icsp_write(pic, TPROG, saux);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                    Application->ProcessEvents();
                }
                Window1.timer1.SetRunState(1);
                prog_ssend(0);
                break;
            case 0x54:  // Read program
                prog_ssend(0x54);
                icsp_set_addr(pic, TPROG, start);
                for (i = 0; i < size; i++) {
                    if ((start + i) >= info.mclen)
                        break;
                    saux = icsp_read(pic, TPROG);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                }
                prog_ssend(0);
                break;

                /*
                CMD_READ_OSC 'c' 0x63	->	Read calibration values
                    <-	0x63
                    <-	word read
                    ...
                    <-	status
                */

            case 0x64:  // Read data
                prog_ssend(0x64);
                icsp_set_addr(pic, TEEPROM, 0);
                for (i = 0; i < info.el; i++) {
                    prog_ssend(icsp_read(pic, TEEPROM));
                }
                prog_ssend(0);
                break;
            case 0x65:  // Read ID
                prog_ssend(0x65);
                icsp_set_addr(pic, TUID, 0);
                for (i = 0; i < info.uil; i++) {
                    saux = icsp_read(pic, TUID);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                }
                prog_ssend(0);
                break;
            case 0x66:  // Read config
                prog_ssend(0x66);
                icsp_set_addr(pic, TCONFIG, 0);
                for (i = 0; i < info.cl; i++) {
                    saux = icsp_read(pic, TCONFIG);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                }
                prog_ssend(0);
                break;
            case 0x67:  // Write config
                prog_ssend(0x67);
                icsp_set_addr(pic, TCONFIG, 0);
                for (i = 0; i < info.cl; i++) {
                    prog_srec_tout(((unsigned char*)&saux) + 1);
                    prog_srec_tout(((unsigned char*)&saux));
                    saux = icsp_write(pic, TCONFIG, saux);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                }
                prog_ssend(0);
                break;
            case 0x68:  // Write ID
                prog_ssend(0x68);
                icsp_set_addr(pic, TUID, 0);
                for (i = 0; i < info.uil; i++) {
                    prog_srec_tout(((unsigned char*)&saux) + 1);
                    prog_srec_tout(((unsigned char*)&saux));
                    saux = icsp_write(pic, TUID, saux);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend(((saux & 0x00FF)));
                }
                prog_ssend(0);
                break;
            case 0x69:  // Write data
                prog_ssend(0x69);
                icsp_set_addr(pic, TEEPROM, 0);
                for (i = 0; i < info.el; i++) {
                    prog_srec_tout((unsigned char*)(&saux));
                    saux = icsp_write(pic, TEEPROM, saux);
                    prog_ssend((saux & 0x00FF));
                }
                prog_ssend(0);
                break;
            case 0x70:  // Write one config word
                prog_ssend(0x70);
                icsp_set_addr(pic, TCONFIG, (start - 0X300000) >> 1);
                for (i = 0; i < size; i++) {
                    prog_srec_tout(((unsigned char*)&saux) + 1);
                    prog_srec_tout(((unsigned char*)&saux));
                    saux = icsp_write(pic, TCONFIG, saux);
                    prog_ssend(((saux & 0xFF00) >> 8));
                    prog_ssend((saux & 0x00FF));
                }
                prog_ssend(0);
                break;

                /*
                CMD_WRITE_OSC 'q' 0x71	->	Write calibration values
                    <-	0x71
                word to write	->
                    <-	word read back
                    ...
                    <-	status
                */

            case 0x80:  // Get processor info length
                prog_ssend(0x80);
                prog_ssend(0x2c);
                break;
            case 0x81:  // Load processor info
                prog_ssend(0x81);
                chk = 0;
                chk += prog_srec_tout(((unsigned char*)&info.mclen) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mclen));
                chk += prog_srec_tout(((unsigned char*)&info.aww) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.aww));
                chk += prog_srec_tout(((unsigned char*)&info.dww) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.dww));
                chk += prog_srec_tout(((unsigned char*)&info.uiw) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.uiw));
                chk += prog_srec_tout(((unsigned char*)&info.uim) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.uim));
                chk += prog_srec_tout(((unsigned char*)&info.mcm1) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcm1));
                chk += prog_srec_tout(((unsigned char*)&info.mcm2) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcm2));
                chk += prog_srec_tout(((unsigned char*)&info.mew) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mew));
                chk += prog_srec_tout(((unsigned char*)&info.mem) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mem));
                chk += prog_srec_tout(((unsigned char*)&info.mcw) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcw));
                chk += prog_srec_tout(((unsigned char*)&info.mcam) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcam));
                chk += prog_srec_tout(((unsigned char*)&info.mcas) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcas));
                chk += prog_srec_tout(((unsigned char*)&info.mcl) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.mcl));
                chk += prog_srec_tout(((unsigned char*)&info.uis) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.uis));
                chk += prog_srec_tout(((unsigned char*)&info.uil));
                chk += prog_srec_tout(((unsigned char*)&info.cs) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.cs));
                chk += prog_srec_tout(((unsigned char*)&info.cl));
                chk += prog_srec_tout(((unsigned char*)&info.es) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.es));
                chk += prog_srec_tout(((unsigned char*)&info.el) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.el));
                chk += prog_srec_tout(((unsigned char*)&info.cas) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.cas));
                chk += prog_srec_tout(((unsigned char*)&info.cal) + 1);
                chk += prog_srec_tout(((unsigned char*)&info.cal));
                chk += prog_srec_tout(((unsigned char*)&info.overprogram));
                chk += prog_srec_tout(((unsigned char*)&info.tries));
                chk += prog_srec_tout(((unsigned char*)&info.algorithm));
                chk += prog_srec_tout(((unsigned char*)&info.flags));
#ifdef _DEBUG_
                printf(" recebido %i de 44", chk);
#endif
                prog_srec_tout(&chk);
                prog_ssend(0);
                break;
            case 0x82:  // Load extended config masks
                prog_ssend(0x82);
                chk = 0;
                for (i = 0; i < 16; i++) {
                    chk += prog_srec_tout(((unsigned char*)&ecm[i]) + 1);
                    chk += prog_srec_tout(((unsigned char*)&ecm[i]));
                }
#ifdef _DEBUG_
                printf(" recebido %i de 32", chk);
#endif
                prog_srec_tout(&chk);
                prog_ssend(0);
                break;
            case 0x88:  // Ping
                prog_ssend(0xab);
                break;
            case 0x8d:  // Get version
                prog_ssend(0x8d);
                prog_ssend(4);
                prog_ssend(40);
                prog_ssend(12);
                break;
            case 0x8e:  // Set range
                start = 0;
                size = 0;
                prog_ssend(0x8e);
                prog_srec_tout(((unsigned char*)&start) + 2);
                prog_ssend((char)(start >> 16));

                prog_srec_tout(((unsigned char*)&start) + 1);
                prog_ssend((char)((start >> 8) & 0x0FF));

                prog_srec_tout((unsigned char*)&start);
                prog_ssend((char)(start & 0x0FF));

                prog_srec_tout(((unsigned char*)&size) + 1);
                prog_ssend((char)(size >> 8));

                prog_srec_tout((unsigned char*)&size);
                prog_ssend((char)(size & 0x0FF));
#ifdef _DEBUG_
                printf(" range: start %#04X size %#02X\n", start, size);
#endif
                break;
            case 0x8F:  // Erase flash device
                prog_ssend(0x8F);
                icsp_set_addr(pic, TPROG, 0);
                for (i = 0; i < info.mclen; i++)
                    icsp_write(pic, TPROG, info.dww);

                icsp_set_addr(pic, TEEPROM, 0);
                for (i = 0; i < info.el; i++)
                    icsp_write(pic, TEEPROM, info.mem);

                icsp_set_addr(pic, TUID, 0);
                for (i = 0; i < info.uil; i++)
                    icsp_write(pic, TUID, info.uim);

                icsp_set_addr(pic, TCONFIG, 0);
                for (i = 0; i < info.cl; i++)
                    icsp_write(pic, TCONFIG, ecm[i]);

                prog_ssend(0);
                break;
                /*

                0x90	->	Compute program checksum
                    <-	0x90
                    <-	checksum_h
                    <-	checksum_l
                    <-	status


                0x91	->	Compute config checksum
                    <-	0x91
                    <-	checksum_h
                    <-	checksum_l
                    <-	status


                0x92	->	Compute ID checksum
                    <-	0x92
                    <-	checksum_h
                    <-	checksum_l
                    <-	status


                0x93	->	Compute data checksum
                    <-	0x93
                    <-	checksum_h
                    <-	checksum_l
                    <-	status


                0x94	->	Compute cal checksum
                    <-	0x94
                    <-	checksum_h
                    <-	checksum_l
                    <-	status
                */
            default:
#ifdef _DEBUG_
                printf("--> Not Implemeted!");
#endif
                break;
        }
#ifdef _DEBUG_
        printf("\n");
        fflush(stdout);
#endif

        Window1.statusbar1.SetField(0, lxT("Running..."));
        pic_reset(pic, 0);
    }

    /*
    #ifdef _WIN_
            Sleep(1);
    #else
            usleep(10);
    #endif
    */

    return ret;
}

int prog_end(void) {
    prog_sclose();

    return EXIT_SUCCESS;
}
