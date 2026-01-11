/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#define _TCP_

#ifndef _WIN_
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#define MSG_NOSIGNAL 0
#endif

#include "../lib/picsimlab.h"
#include "../lib/serial_port.h"
#include "bsim_remote.h"

#include <stdint.h>
#include <unistd.h>

#if __BIG_ENDIAN__
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

static int listenfd = -1;

static const int id[3] = {0, 1, 2};

static void picsimlab_uart_rx_event(bitbang_uart_t* bu, void* arg) {
    if (bu->ctrl_on) {
        const int id = *((const int*)arg);
        unsigned char data = bitbang_uart_recv(bu);
        printf("uart[%i] data recv [%c]\n", id, data);
        // qemu_picsimlab_uart_receive(id, &data, 1);
    }
}

bsim_remote::bsim_remote(void) {
    connected = 0;
    sockfd = -1;
    fname_bak[0] = 0;
    fname_[0] = 0;

    memset(&ADCvalues, 0xFF, 32);

    bitbang_i2c_ctrl_init(&master_i2c[0], this);
    bitbang_i2c_ctrl_init(&master_i2c[1], this);
    bitbang_spi_ctrl_init(&master_spi[0], this);
    bitbang_spi_ctrl_init(&master_spi[1], this);
    bitbang_uart_init(&master_uart[0], this, picsimlab_uart_rx_event, (void*)&id[0]);
    bitbang_uart_init(&master_uart[1], this, picsimlab_uart_rx_event, (void*)&id[1]);
}

bsim_remote::~bsim_remote(void) {
    bitbang_i2c_ctrl_end(&master_i2c[0]);
    bitbang_i2c_ctrl_end(&master_i2c[1]);
    bitbang_spi_ctrl_end(&master_spi[0]);
    bitbang_spi_ctrl_end(&master_spi[1]);
    bitbang_uart_end(&master_uart[0]);
    bitbang_uart_end(&master_uart[1]);
}

void bsim_remote::MSetSerial(const char* port) {
    /*
    set_serial (&pic,0, port, 0, 0, 0);
    set_serial (&pic,1, "", 0, 0, 0);
     */
}

int bsim_remote::MInit(const char* processor, const char* fname, float freq) {
#ifdef _TCP_
    struct sockaddr_in serv;
#else
    struct sockaddr_un serv;
#endif

    std::string sproc = GetSupportedDevices();
    if (sproc.find(processor) == std::string::npos) {
        Proc = "Ripes";
        printf("PICSimLab: Unknown processor %s, loading default !\n", processor);
    }

    sockfd = -1;

    pins_reset();

    serialfd[0] = INVALID_SERIAL;
    serialfd[1] = INVALID_SERIAL;
    serialfd[2] = INVALID_SERIAL;
    serialfd[3] = INVALID_SERIAL;

    if (listenfd < 0) {
#ifdef _TCP_
        int reuse = 1;
        if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("picsimlab: socket error : %s \n", strerror(errno));
            exit(1);
        }

        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed");

        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
        serv.sin_port = htons(7890);
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
            printf("picsimlab: remote bind error : %s \n", strerror(errno));
            exit(1);
        }

        if (listen(listenfd, SOMAXCONN)) {
            printf("picsimlab: remote listen error : %s \n", strerror(errno));
            exit(1);
        }
    }

    PICSimLab.ConfigMenuGUI(GMT_DISABLED);

    setnblock(listenfd);

    return 0;  // ret;
}

const int bsim_remote::TestConnection(void) {
    if (!connected) {
#ifdef _TCP_
        struct sockaddr_in cli;
#else
        struct sockaddr_un cli;
#endif
#ifdef _WIN_
        int clilen;
#else
        unsigned int clilen;
#endif

        clilen = sizeof(cli);
        if ((sockfd = accept(listenfd, (sockaddr*)&cli, &clilen)) < 0) {
            sockfd = -1;
            connected = 0;
            return 0;
        }
        printf("picsimlab: Ripes connected to PICSimLab!\n");

        connected = 1;
        StartThread();
    }
    return connected;
}

const int bsim_remote::DataAvaliable(void) {
    if (!connected)
        return 0;

    char dp;
#ifndef _WIN_
    int ret = recv(sockfd, &dp, 1, MSG_PEEK | MSG_DONTWAIT);
#else
    setnblock(sockfd);
    int ret = recv(sockfd, &dp, 1, MSG_PEEK);
    setblock(sockfd);
#endif

    if (ret != 1) {
#ifndef _WIN_
        if (errno != EAGAIN)
#else
        if (WSAGetLastError() != WSAEWOULDBLOCK)
#endif
        {
            ConnectionError("recv");
        }
        return 0;
    }

    return 1;
}

void bsim_remote::ConnectionError(const char* s_error) {
    printf("picsimlab: remote %s error : %s \n", s_error, strerror(errno));
    Disconnect();
}

void bsim_remote::Disconnect(void) {
    if (connected) {
        if (sockfd >= 0)
            close(sockfd);
        sockfd = -1;
        connected = 0;
        PICSimLab.SetMcuPwr(0);
    }
}

void bsim_remote::MEnd(void) {
    Disconnect();

    PICSimLab.ConfigMenuGUI(GMT_HEX);

    connected = 0;

    StopThread();
}

int bsim_remote::MGetArchitecture(void) {
    return ARCH_UNKNOWN;
}

void bsim_remote::EndServers(void) {
    if (listenfd >= 0)
        close(listenfd);
    listenfd = -1;

    if (sockfd >= 0)
        close(sockfd);
    sockfd = -1;
}

void bsim_remote::MEraseFlash(void) {
    // erase_flash ();
}

void bsim_remote::MSetFreq(float freq_) {
    freq = freq_;
    inc_ns = 1000000000L / freq;
    TimerUpdateFrequency(freq);
}

float bsim_remote::MGetFreq(void) {
    return freq;
}

void bsim_remote::MSetVCC(float vcc) {
    printf("remote: Incomplete!!!!\n");
}

float bsim_remote::MGetVCC(void) {
    return 5.0;
}

float bsim_remote::MGetInstClockFreq(void) {
    return freq;
}

int bsim_remote::CpuInitialized(void) {
    return 1;
}

void bsim_remote::DebugLoop(void) {}

std::string bsim_remote::MGetPinName(int pin) {
    std::string pinname = "error";

    if (!Proc.compare("Ripes")) {
        switch (pin) {
            case 1:
                pinname = "PA0";
                break;
            case 2:
                pinname = "PA1";
                break;
            case 3:
                pinname = "PA2";
                break;
            case 4:
                pinname = "PA3";
                break;
            case 5:
                pinname = "PA4";
                break;
            case 6:
                pinname = "PA5";
                break;
            case 7:
                pinname = "PA6";
                break;
            case 8:
                pinname = "PA7";
                break;
            case 9:
                pinname = "PA8";
                break;
            case 10:
                pinname = "PA9";
                break;
            case 11:
                pinname = "PAa";
                break;
            case 12:
                pinname = "PAb";
                break;
            case 13:
                pinname = "PAc";
                break;
            case 14:
                pinname = "PAd";
                break;
            case 15:
                pinname = "PAe";
                break;
            case 16:
                pinname = "PAf";
                break;
            case 17:
                pinname = "PB0";
                break;
            case 18:
                pinname = "PB1";
                break;
            case 19:
                pinname = "PB2";
                break;
            case 20:
                pinname = "PB3";
                break;
            case 21:
                pinname = "PB4";
                break;
            case 22:
                pinname = "PB5";
                break;
            case 23:
                pinname = "PB6";
                break;
            case 24:
                pinname = "PB7";
                break;
            case 25:
                pinname = "PB8";
                break;
            case 26:
                pinname = "PB9";
                break;
            case 27:
                pinname = "PBa";
                break;
            case 28:
                pinname = "PBb";
                break;
            case 29:
                pinname = "PBc";
                break;
            case 30:
                pinname = "PBd";
                break;
            case 31:
                pinname = "PBe";
                break;
            case 32:
                pinname = "PBf";
                break;
            case 33:
                pinname = "PC0";
                break;
            case 34:
                pinname = "PC1";
                break;
            case 35:
                pinname = "PC2";
                break;
            case 36:
                pinname = "PC3";
                break;
            case 37:
                pinname = "PC4";
                break;
            case 38:
                pinname = "PC5";
                break;
            case 39:
                pinname = "PC6";
                break;
            case 40:
                pinname = "PC7";
                break;
            case 41:
                pinname = "PC8";
                break;
            case 42:
                pinname = "PC9";
                break;
            case 43:
                pinname = "PCa";
                break;
            case 44:
                pinname = "PCb";
                break;
            case 45:
                pinname = "PCc";
                break;
            case 46:
                pinname = "PCd";
                break;
            case 47:
                pinname = "PCe";
                break;
            case 48:
                pinname = "PCf";
                break;
            case 49:
                pinname = "TX0";
                break;
            case 50:
                pinname = "RX0";
                break;
            case 51:
                pinname = "TX1";
                break;
            case 52:
                pinname = "RX1";
                break;
            case 53:
                pinname = "SCK0";
                break;
            case 54:
                pinname = "COPI0";
                break;
            case 55:
                pinname = "CIPO0";
                break;
            case 56:
                pinname = "NCS0";
                break;
            case 57:
                pinname = "SCK1";
                break;
            case 58:
                pinname = "COPI1";
                break;
            case 59:
                pinname = "CIPO1";
                break;
            case 60:
                pinname = "NCS1";
                break;
            case 61:
                pinname = "SCL0";
                break;
            case 62:
                pinname = "SDA0";
                break;
            case 63:
                pinname = "SCL1";
                break;
            case 64:
                pinname = "SDA1";
                break;
            case 65:
                pinname = "VSS";
                break;
            case 66:
                pinname = "VDD";
                break;
            default:
                pinname = "VSS";
                break;
        }
    }

    return pinname;
}

int bsim_remote::MDumpMemory(const char* fname) {
    // only create a empty place holder file
    FILE* fout;
    fout = fopen_UTF8(fname, "w");
    if (fout) {
        fprintf(fout, " ");
        fclose(fout);
        return 0;
    }
    return 1;
}

int bsim_remote::DebugInit(int dtyppe)  // argument not used in picm only mplabx
{
    return 0;  //! mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}

int bsim_remote::MGetPinCount(void) {
    if (!Proc.compare("Ripes"))
        return 100;
    return 0;
}

void bsim_remote::pins_reset(void) {
    std::string pname;
    for (int p = 0; p < MGetPinCount(); p++) {
        pins[p].avalue = 0;
        pins[p].lvalue = 0;
        pins[p].value = 0;
        pins[p].ptype = PT_DIGITAL;
        pins[p].dir = PD_IN;
        pins[p].ovalue = 0;
        pins[p].oavalue = 55;

        pname = MGetPinName(p + 1);

        if (pname[0] == 'P') {
            if (pname[1] == 'A') {
                pins[p].port = (unsigned char*)&Ports[0];
                pins[p].pord = pname[2] - '0';
                if (pins[p].pord > 15)
                    pins[p].pord -= 39;
            } else if (pname[1] == 'B') {
                pins[p].port = (unsigned char*)&Ports[1];
                pins[p].pord = pname[2] - '0';
                if (pins[p].pord > 15)
                    pins[p].pord -= 39;
            } else if (pname[1] == 'C') {
                pins[p].ptype = PT_ANALOG;
            }
        } else if (pname.find("VDD") != std::string::npos) {
            pins[p].port = NULL;
            pins[p].pord = -1;
            pins[p].value = 1;
            pins[p].dir = PD_OUT;
            pins[p].ptype = PT_POWER;
        } else if (pname.find("VSS") != std::string::npos) {
            pins[p].port = NULL;
            pins[p].pord = -1;
            pins[p].value = 0;
            pins[p].dir = PD_OUT;
            pins[p].ptype = PT_POWER;
        } else {
            pins[p].port = NULL;
            pins[p].pord = -1;
        }
    }
}

void bsim_remote::MSetPin(int pin, unsigned char value) {
    if (!pin)
        return;
    if ((pins[pin - 1].dir == PD_IN) && (pins[pin - 1].value != value)) {
        pins[pin - 1].value = value;
        unsigned short* port = (unsigned short*)pins[pin - 1].port;

        if (port) {
            if (value) {
                *port |= 1 << pins[pin - 1].pord;
            } else {
                *port &= ~(1 << pins[pin - 1].pord);
            }
        }
    }
}

void bsim_remote::MSetPinDOV(int pin, unsigned char ovalue) {
    // set_pin_DOV (pin, ovalue);
}

void bsim_remote::MSetPinOAV(int pin, float value) {
    pins[pin - 1].oavalue = value;
}

void bsim_remote::MSetAPin(int pin, float value) {
    if (!pin)
        return;
    if ((connected) && (pins[pin - 1].avalue != value)) {
        unsigned char channel = 0xFF;

        pins[pin - 1].avalue = value;

        if (!Proc.compare("Ripes")) {
            switch (pin) {
                case 33:  // PC0
                    channel = 0;
                    break;
                case 34:  // PC1
                    channel = 1;
                    break;
                case 35:  // PC2
                    channel = 2;
                    break;
                case 36:  // PC3
                    channel = 3;
                    break;
                case 37:  // PC4
                    channel = 4;
                    break;
                case 38:  // PC5
                    channel = 5;
                    break;
                case 39:  // PC6
                    channel = 6;
                    break;
                case 40:  // PC7
                    channel = 7;
                    break;
                case 41:  // PC8
                    channel = 8;
                    break;
                case 42:  // PC9
                    channel = 9;
                    break;
                case 43:  // PCa
                    channel = 10;
                    break;
                case 44:  // PCb
                    channel = 11;
                    break;
                case 45:  // PCc
                    channel = 12;
                    break;
                case 46:  // PCd
                    channel = 13;
                    break;
                case 47:  // PCe
                    channel = 14;
                    break;
                case 48:  // PCf
                    channel = 15;
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
                /*
                unsigned char buff[3];

                buff[0] = channel | 0x40;
                buff[1] = svalue & 0xFF;
                buff[2] = svalue >> 8;
                 */
                ADCvalues[channel] = svalue;
            }
        }
    }
}

unsigned char bsim_remote::MGetPin(int pin) {
    if ((pin) && (pin < MGetPinCount())) {
        return pins[pin - 1].value;
    }
    return 0;
}

int bsim_remote::MReset(int flags) {
    // Disconnect();
    Dirs[0] = 0xFF;
    Dirs[1] = 0xFF;
    Ports[0] = 0x00;
    Ports[1] = 0x00;
    t0CNT = 0;
    t0STA = 0;
    t0CON = 0x7FFF;
    t0PR = 0xFFFF;
    t0iclk = 0;

    bitbang_i2c_rst(&master_i2c[0]);
    bitbang_i2c_rst(&master_i2c[1]);
    bitbang_spi_rst(&master_spi[0]);
    bitbang_spi_rst(&master_spi[1]);
    bitbang_uart_rst(&master_uart[0]);
    bitbang_uart_rst(&master_uart[1]);
    return 0;
}

int bsim_remote::MGetResetPin(void) {
    return 0;
}

int bsim_remote::MGetIOUpdated(void) {
    return ioupdated;
}

void bsim_remote::MClearIOUpdated(void) {
    ioupdated = 0;
}

const picpin* bsim_remote::MGetPinsValues(void) {
    return pins;
}

float* bsim_remote::MGetPinOAVPtr(int pin) {
    return &pins[pin - 1].oavalue;
}

void bsim_remote::MStep() {
    if (ioupdated) {
        for (int id = 0; id < 2; id++) {
            if (master_i2c[id].ctrl_on) {
                if (master_i2c[id].scl_pin) {
                    pins[master_i2c[id].scl_pin - 1].dir = PD_OUT;
                    pins[master_i2c[id].scl_pin - 1].value = master_i2c[id].scl_value;
                }
                if (master_i2c[id].sda_pin) {
                    if (master_i2c[id].sda_dir == PD_OUT) {
                        pins[master_i2c[id].sda_pin - 1].dir = PD_OUT;
                        pins[master_i2c[id].sda_pin - 1].value = master_i2c[id].sda_value;
                    } else {
                        pins[master_i2c[id].sda_pin - 1].dir = PD_IN;
                        master_i2c[id].sda_value = pins[master_i2c[id].sda_pin - 1].value;
                    }
                }
            }
            if (master_spi[id].ctrl_on) {
                if (master_spi[id].sck_pin) {
                    pins[master_spi[id].sck_pin - 1].dir = PD_OUT;
                    pins[master_spi[id].sck_pin - 1].value = master_spi[id].sck_value;
                }
                if (master_spi[id].copi_pin) {
                    pins[master_spi[id].copi_pin - 1].dir = PD_OUT;
                    pins[master_spi[id].copi_pin - 1].value = master_spi[id].copi_value;
                }
                if (master_spi[id].cipo_pin) {
                    pins[master_spi[id].cipo_pin - 1].dir = PD_IN;
                    master_spi[id].cipo_value = pins[master_spi[id].cipo_pin - 1].value;
                }
                if (master_spi[id].cs_pin[0]) {
                    pins[master_spi[id].cs_pin[0] - 1].dir = PD_OUT;
                    pins[master_spi[id].cs_pin[0] - 1].value = master_spi[id].cs_value[0];
                }
                if (master_spi[id].cs_pin[1]) {
                    pins[master_spi[id].cs_pin[1] - 1].dir = PD_OUT;
                    pins[master_spi[id].cs_pin[1] - 1].value = master_spi[id].cs_value[1];
                }
                if (master_spi[id].cs_pin[2]) {
                    pins[master_spi[id].cs_pin[2] - 1].dir = PD_OUT;
                    pins[master_spi[id].cs_pin[2] - 1].value = master_spi[id].cs_value[2];
                }
            }
            if (master_uart[id].ctrl_on) {
                if (master_uart[id].tx_pin) {
                    pins[master_uart[id].tx_pin - 1].dir = PD_OUT;
                    pins[master_uart[id].tx_pin - 1].value = master_uart[id].tx_value;
                }
                if (master_uart[id].rx_pin) {
                    pins[master_uart[id].rx_pin - 1].dir = PD_IN;
                    master_uart[id].rx_value = pins[master_uart[id].rx_pin - 1].value;
                    bitbang_uart_io(&master_uart[id], master_uart[id].rx_value);
                }
            }
        }
    }
}

int bsim_remote::GetUARTRX(const int uart_num) {
    if (uart_num < 2) {
        return master_uart[uart_num].rx_pin;
    }
    return 0;
}

int bsim_remote::GetUARTTX(const int uart_num) {
    if (uart_num < 2) {
        return master_uart[uart_num].tx_pin;
    }
    return 0;
}

//===================== Ripes protocol =========================================

int32_t bsim_remote::recv_payload(char* buff, const uint32_t payload_size) {
    char* dp = buff;
    int ret = 0;
    uint32_t size = payload_size;
    do {
        if ((ret = recv(sockfd, dp, size, MSG_WAITALL)) != (int)size) {
            printf("receive error : %s \n", strerror(errno));
            return -1;
        }
        size -= ret;
        dp += ret;
    } while (size);

    return ret;
}

int32_t bsim_remote::send_cmd(const uint32_t cmd, const char* payload, const uint32_t payload_size) {
#define BSIZE 1024

    char buffer[BSIZE];
    uint32_t ret;
    cmd_header_t cmd_header;

    cmd_header.msg_type = htonl(cmd);
    cmd_header.payload_size = htonl(payload_size);
    cmd_header.time = 0;

    unsigned int dsize = sizeof(cmd_header_t);
    const char* dp = (const char*)&cmd_header;

    if (payload_size) {
        for (unsigned int i = 0; i < dsize; i++) {
            buffer[i] = dp[i];
        }
        for (unsigned int i = 0; i < payload_size; i++) {
            buffer[dsize + i] = payload[i];
        }
        dsize += payload_size;
        if (dsize > BSIZE) {
            printf("PICSimLab: Send Overflow\n");
            exit(-1);
        }
        dp = buffer;
    }

    if ((ret = send(sockfd, dp, dsize, MSG_NOSIGNAL)) != dsize) {
        printf("send error : %s \n", strerror(errno));
        return -1;
    }
    return ret;
}

int32_t bsim_remote::recv_cmd(cmd_header_t* cmd_header) {
    char* dp = (char*)(cmd_header);
    int ret = 0;
    int size = sizeof(cmd_header_t);
    do {
        if ((ret = recv(sockfd, dp, size, MSG_WAITALL)) != size) {
            printf("receive error : %s \n", strerror(errno));
            return -1;
        }

        size -= ret;
        dp += ret;
    } while (size);

    cmd_header->msg_type = ntohl(cmd_header->msg_type);
    cmd_header->payload_size = ntohl(cmd_header->payload_size);
    cmd_header->time = ntohll(cmd_header->time);

    int64_t now = cmd_header->time;

    if (now > timerlast) {
        int64_t delta = now - timerlast;
        timerlast = now;

        if (delta > (TTIMEOUT * 1.1)) {
            delta = (TTIMEOUT * 1.1);
        }
        Run_CPU_ns(delta);
    } else if (now < timerlast) {
        timerlast = now;
    }

    return ret;
}

// Called ever 1s to refresh status

void bsim_remote::RefreshStatus(void) {
    PICSimLab.UpdateStatus(PS_SERIAL, "HWSerial: " + std::string(SERIALDEVICE));
}

//==============================================================================
