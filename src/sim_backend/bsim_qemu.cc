/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include <dlfcn.h>
#endif

#include <time.h>
#include <unistd.h>
#include "../lib/picsimlab.h"
#include "../lib/serial_port.h"
#include "bsim_qemu.h"

#define dprintf \
    if (1) {    \
    } else      \
        printf

// function pointers

void (*qemu_init)(int, char**, const char**);
void (*qemu_main_loop)(void);
void (*qemu_cleanup)(void);

void (*qmp_quit)(Error** errp);
void (*qmp_stop)(Error** errp);
void (*qmp_system_reset)(Error** errp);
void (*qmp_pmemsave)(int64_t val, int64_t size, const char* filename, Error** errp);
void (*qmp_memsave)(int64_t val, int64_t size, const char* filename, Error** errp);
void (*qmp_cont)(Error** errp);

void (*qemu_mutex_lock_iothread_impl)(const char* file, int line);
void (*qemu_mutex_unlock_iothread)(void);

void (*qemu_picsimlab_register_callbacks)(void* arg);
void (*qemu_picsimlab_set_pin)(int pin, int value);
void (*qemu_picsimlab_set_apin)(int chn, int value);
int (*qemu_picsimlab_flash_dump)(int64_t offset, void* buf, int bytes);
void (*qemu_picsimlab_uart_receive)(const int id, const uint8_t* buf, int size);

int64_t (*qemu_clock_get_ns)(QEMUClockType type);

void (*timer_init_full)(QEMUTimer* ts, QEMUTimerListGroup* timer_list_group, QEMUClockType type, int scale,
                        int attributes, QEMUTimerCB* cb, void* opaque);

void (*timer_mod_ns)(QEMUTimer* ts, int64_t expire_time);

uint32_t* (*qemu_picsimlab_get_internals)(int cfg);

uint32_t (*qemu_picsimlab_get_TIOCM)(void);

// global pointers to c callbacks
static picpin* g_pins;
static bsim_qemu* g_board = NULL;

static int64_t GotoNow(void) {
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    int64_t delta;

    if (now >= g_board->timer.last) {
        delta = now - g_board->timer.last;
        g_board->timer.last = now;

        if (delta > (TTIMEOUT * 1.1)) {
            delta = (TTIMEOUT * 1.1);
        }
    } else {
        delta = g_board->GetInc_ns();
        g_board->timer.last += delta;
    }

    return delta;
}

static void picsimlab_write_pin(int pin, int value) {
    // printf("================> IO    <====================== %ji\n", now - g_board->timer.last);
    ioupdated = 1;
    g_board->Run_CPU_ns(GotoNow());

    g_pins[pin - 1].value = value;
    // printf("pin[%i]=%i\n", pin, value);
}

static void picsimlab_dir_pins(int pin, int dir) {
    // printf("================> IO    <====================== %ji\n", now - g_board->timer.last);

    if (pin > 0) {  // normal io
        ioupdated = 1;
        g_board->Run_CPU_ns(GotoNow());
        g_pins[pin - 1].dir = !dir;
    } else if (dir == -1) {  // sync input
        ioupdated = 1;
        g_board->Run_CPU_ns(GotoNow());
    } else {  // especial pin cfg
        g_board->PinsExtraConfig(dir);
    }
    // printf("pin[%i]=%s\n", pin, (!dir == PD_IN) ? "PD_IN" : "PD_OUT");
}

static int picsimlab_i2c_event(const uint8_t id, const uint8_t addr, const uint16_t event) {
    g_board->Run_CPU_ns(GotoNow());

    switch (event & 0xFF) {
        case I2C_START_RECV:
        case I2C_START_SEND:

            bitbang_i2c_ctrl_start(&g_board->master_i2c[id]);
            g_board->timer.last += 8000;
            g_board->Run_CPU_ns(8000);

            if (event == I2C_START_RECV) {
                bitbang_i2c_ctrl_write(&g_board->master_i2c[id], (addr << 1) | 0x01);
                dprintf(">>> start recv =0x%02x\n", addr);
            } else {
                bitbang_i2c_ctrl_write(&g_board->master_i2c[id], addr << 1);
                dprintf(">>> start send =0x%02x\n", addr);
            }
            g_board->timer.last += 72000;
            g_board->Run_CPU_ns(72000);
            break;
        case I2C_FINISH:
            bitbang_i2c_ctrl_stop(&g_board->master_i2c[id]);
            g_board->timer.last += 8000;
            g_board->Run_CPU_ns(8000);
            dprintf("<<< stop =0x%02x\n", addr);
            break;
        case I2C_NACK:
            break;
        case I2C_WRITE:
            dprintf("==> send addr=0x%02x value=0x%02x\n", addr, event >> 8);
            bitbang_i2c_ctrl_write(&g_board->master_i2c[id], event >> 8);  // TODO verify ACK
            g_board->timer.last += 72000;
            g_board->Run_CPU_ns(72000);
            return 1;
            break;
        case I2C_READ:
            bitbang_i2c_ctrl_read(&g_board->master_i2c[id]);  // TODO verify ACK
            g_board->timer.last += 72000;
            g_board->Run_CPU_ns(72000);
            dprintf("<== recv addr=0x%02x value=0x%02x\n", addr, g_board->master_i2c[id].datar);
            return g_board->master_i2c[id].datar;
            break;
    }
    return 0;
}

static uint8_t picsimlab_spi_event(const uint8_t id, const uint16_t event) {
    g_board->Run_CPU_ns(GotoNow());
    uint64_t cycle_ns = g_board->TimerGet_ns(g_board->master_spi[id].TimerID);

    switch (event & 0xFF) {
        case 0:  // tranfer
            bitbang_spi_ctrl_write(&g_board->master_spi[id], event >> 8);
            g_board->timer.last += cycle_ns * 36;
            g_board->Run_CPU_ns(cycle_ns * 36);
            dprintf("SPI MASTER SEND 0x%02X  RECV 0x%02X\n", event >> 8, g_board->master_spi[id].data);
            return g_board->master_spi[id].data;
            break;
        case 1:  // CS
            ioupdated = 1;
            dprintf("SPI MASTER CS 0x%02X\n", event >> 8);
            switch (event >> 9) {
                case 0:
                    g_board->master_spi[id].cs_value[0] = (event >> 8) & 1;
                    break;
                case 1:
                    g_board->master_spi[id].cs_value[1] = (event >> 8) & 1;
                    break;
                case 2:
                    g_board->master_spi[id].cs_value[2] = (event >> 8) & 1;
                    break;
            }
            g_board->timer.last += cycle_ns * 2;
            g_board->Run_CPU_ns(cycle_ns * 2);
            break;
    }
    return 0;
}

static void picsimlab_uart_tx_event(const uint8_t id, const uint8_t value) {
    dprintf("Uart[%i] %c \n", id, value);

    uint32_t baud_rate = *qemu_picsimlab_get_internals(QEMU_INTERNAL_UART0_BAUD + id);
    bitbang_uart_set_speed(&g_board->master_uart[id], baud_rate);

    unsigned long delta = (1e10 / baud_rate);

    g_board->Run_CPU_ns(GotoNow());

    bitbang_uart_send(&g_board->master_uart[id], value);
    g_board->timer.last += delta;
    g_board->Run_CPU_ns(delta);
}

static void picsimlab_uart_rx_event(bitbang_uart_t* bu, void* arg) {
    if (bu->ctrl_on) {
        const int id = *((const int*)arg);
        unsigned char data = bitbang_uart_recv(bu);
        // printf("uart[%i] data recv [%c]\n", id, data);
        qemu_picsimlab_uart_receive(id, &data, 1);
    }
}

static void picsimlab_rmt_event(const uint8_t channel, const uint32_t config0, const uint32_t value) {
    long inc = (config0 & 0xF) * 12.5;
    long t;

    // static int count = 0;
    // float step = (config0 & 0xF) / 80e6;
    // printf("%4i RMT event channel[%d] %d(%e) %d(%e)\n", count++, channel, (value & 0x8000) >> 15,
    //        (value & 0x7FFF) * step, ((value >> 16) & 0x8000) >> 15, ((value >> 16) & 0x7FFF) * step);

    ioupdated = 1;
    t = (value & 0x7FFF) * inc;
    g_board->rmt_out.out[channel] = (value & 0x8000) >> 15;
    g_board->timer.last += t;
    g_board->Run_CPU_ns(t);

    ioupdated = 1;
    t = ((value >> 16) & 0x7FFF) * inc;
    g_board->rmt_out.out[channel] = ((value >> 16) & 0x8000) >> 15;
    g_board->timer.last += t;
    g_board->Run_CPU_ns(t);
}

static callbacks_t callbacks = {picsimlab_write_pin, picsimlab_dir_pins,      picsimlab_i2c_event,
                                picsimlab_spi_event, picsimlab_uart_tx_event, NULL,
                                picsimlab_rmt_event};

int bsim_qemu::load_qemu_lib(const char* path) {
#ifndef _WIN_  // LINUX
    std::string fullpath = PICSimLab.GetLibPath() + "qemu/" + path + ".so";

    void* handle = dlopen((const char*)fullpath.c_str(), RTLD_NOW);
    if (handle == nullptr) {
        printf("picsimlab qemu: %s\n", dlerror());
        return 0;
    }

#define GET_SYMBOL_AND_CHECK(X)                   \
    *((void**)(&X)) = dlsym(handle, #X);          \
    if (nullptr == X) {                           \
        printf("Qemu lib Lost symbol: " #X "\n"); \
        return 0;                                 \
    }
#else  // WINDOWS
    char expath[512];
    char dpath[512];
    PICSimLab.SystemCmd(PSC_GETEXECUTABLEPATH, NULL, expath);
    PICSimLab.SystemCmd(PSC_DIRNAME, expath, dpath);
    std::string fullpath = std::string(dpath) + "/lib/qemu/" + path + ".dll";

    HMODULE handle = LoadLibraryA((const char*)fullpath.c_str());
    if (handle == NULL) {
        printf("picsimlab qemu: Error loading %s\n", (const char*)fullpath.c_str());
        return 0;
    }

#define GET_SYMBOL_AND_CHECK(X)                          \
    *((void**)(&X)) = (void*)GetProcAddress(handle, #X); \
    if (nullptr == X) {                                  \
        printf("Qemu lib Lost symbol: " #X "\n");        \
        return 0;                                        \
    }
#endif
    GET_SYMBOL_AND_CHECK(qemu_init);
    GET_SYMBOL_AND_CHECK(qemu_main_loop);
    GET_SYMBOL_AND_CHECK(qemu_cleanup);
    GET_SYMBOL_AND_CHECK(qmp_quit);
    GET_SYMBOL_AND_CHECK(qmp_stop);
    GET_SYMBOL_AND_CHECK(qmp_system_reset);
    GET_SYMBOL_AND_CHECK(qmp_pmemsave);
    GET_SYMBOL_AND_CHECK(qmp_memsave);
    GET_SYMBOL_AND_CHECK(qmp_cont);
    GET_SYMBOL_AND_CHECK(qemu_mutex_lock_iothread_impl);
    GET_SYMBOL_AND_CHECK(qemu_mutex_unlock_iothread);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_register_callbacks);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_set_pin);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_set_apin);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_flash_dump);
    GET_SYMBOL_AND_CHECK(qemu_clock_get_ns);
    GET_SYMBOL_AND_CHECK(timer_init_full);
    GET_SYMBOL_AND_CHECK(timer_mod_ns);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_get_internals);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_get_TIOCM);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_uart_receive);
#undef GET_SYMBOL_AND_CHECK

    return 1;
}

static const int id[3] = {0, 1, 2};

bsim_qemu::bsim_qemu(void) {
    fname_bak[0] = 0;
    fname_[0] = 0;

    SimType = QEMU_SIM_NONE;

    qemu_started = 0;

    memset(&ADCvalues, 0xFF, 32);

    PICSimLab.SetNeedReboot();
    mtx_qinitId = PICSimLab.SystemCmd(PSC_MUTEXCREATE, NULL);
    ns_count = 0;
    icount = -1;
    use_cmdline_extra = 0;
    serial_open = 0;
    application_offset = 0;
    ConfEnableSerial = 1;
    ConfigWaitGdb = 0;

    bitbang_i2c_ctrl_init(&master_i2c[0], this);
    bitbang_i2c_ctrl_init(&master_i2c[1], this);
    bitbang_spi_ctrl_init(&master_spi[0], this);
    bitbang_spi_ctrl_init(&master_spi[1], this);
    bitbang_uart_init(&master_uart[0], this, picsimlab_uart_rx_event, (void*)&id[0]);
    bitbang_uart_init(&master_uart[1], this, picsimlab_uart_rx_event, (void*)&id[1]);
    bitbang_uart_init(&master_uart[2], this, picsimlab_uart_rx_event, (void*)&id[2]);
    bitbang_uart_set_speed(&master_uart[0], 115200);
    bitbang_uart_set_speed(&master_uart[1], 115200);
    bitbang_uart_set_speed(&master_uart[2], 115200);
}

bsim_qemu::~bsim_qemu(void) {
    bitbang_i2c_ctrl_end(&master_i2c[0]);
    bitbang_i2c_ctrl_end(&master_i2c[1]);
    bitbang_spi_ctrl_end(&master_spi[0]);
    bitbang_spi_ctrl_end(&master_spi[1]);
    bitbang_uart_end(&master_uart[0]);
    bitbang_uart_end(&master_uart[1]);
    bitbang_uart_end(&master_uart[2]);
    PICSimLab.SystemCmd(PSC_MUTEXDESTROY, (const char*)&mtx_qinitId);
}

void bsim_qemu::MSetSerial(const char* port) {}

int bsim_qemu::MInit(const char* processor, const char* _fname, float freq_) {
    strcpy(fname, _fname);

    MSetFreq(freq_);

    std::string sproc = GetSupportedDevices();
    if (sproc.find(processor) == std::string::npos) {
        Proc = "stm32f103c8t6";
        printf("PICSimLab: Unknown processor %s, loading default !\n", processor);
    }

    pins_reset();

    serialfd[0] = INVALID_SERIAL;
    serialfd[1] = INVALID_SERIAL;
    serialfd[2] = INVALID_SERIAL;
    serialfd[3] = INVALID_SERIAL;

#ifndef __EMSCRIPTEN__
    if (!qemu_started) {
        StartThread();
        while (!qemu_started) {
#ifndef _WIN_
            usleep(1000);
#else
            Sleep(1);
#endif
            PICSimLab.SystemCmd(PSC_MUTEXLOCK, (const char*)&mtx_qinitId);  // only for wait qemu start
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
        }
#else  // qemu is not supported in emscripten version yet
    qemu_started = -1;
#endif

        PICSimLab.ConfigMenuGUI(GMT_BIN);

#ifndef __EMSCRIPTEN__
    } else {
        printf("PICSimLab: qemu already started !!!!!\n");
    }
#endif

    return 0;  // ret;
}

static void user_timeout_cb(void* opaque) {
    bsim_qemu* board = (bsim_qemu*)opaque;
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer_mod_ns(board->timer.qtimer, now + board->timer.timeout);
    if (PICSimLab.GetSimulationRun()) {
        ioupdated = 0;
        board->Run_CPU_ns(GotoNow());
    }
    board->timer.last = now;
}

void bsim_qemu::EvThreadRun(void) {
    PICSimLab.SystemCmd(PSC_MUTEXLOCK, (const char*)&mtx_qinitId);

    // test icount limits
    if (icount < -1) {
        icount = -1;
    }
    if (icount > 11) {
        icount = 11;
    }

    char* resp = serial_port_list();

#define ARGMAX 100

    char* argv[ARGMAX];
    int argc = 0;

    for (int i = 0; i < ARGMAX; i++) {
        argv[i] = (char*)malloc(1024);
        argv[i][0] = 0;
    }

    if (SimType == QEMU_SIM_STM32) {
        if (!load_qemu_lib("libqemu-stm32")) {
            PICSimLab.RegisterError("Error loading libqemu-stm32");
            qemu_started = -1;
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
            return;
        }

        // change .hex to .bin
        strncpy(fname_, fname, 2048);
        fname_[strlen(fname_) - 3] = 0;
        strncat(fname_, "bin", 2047);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, fname_)) {
            // create a empty memory
            FILE* fout;
            fout = fopen_UTF8(fname_, "wb");
            if (fout) {
                unsigned char sp[4] = {0x00, 0x08, 0x00, 0x20};
                unsigned char handler[4] = {0x51, 0x01, 0x00, 0x00};
                unsigned char loop[4] = {0xFE, 0xE7, 0x00, 0xBF};

                fwrite(&sp, 4, sizeof(char), fout);
                for (int r = 0; r < 83; r++) {
                    fwrite(&handler, 4, sizeof(char), fout);
                }
                fwrite(&loop, 4, sizeof(char), fout);
                fclose(fout);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fname_);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "qemu-stm32");

        strcpy(argv[argc++], "-M");
        if (!Proc.compare("stm32f103c8t6")) {
            strcpy(argv[argc++], "stm32-f103c8-picsimlab-new");
        } else {
            strcpy(argv[argc++], "stm32-p103-picsimlab-new");
        }

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=pflash,format=raw", fname_);

        // strcpy(argv[argc++], "-d");
        // strcpy(argv[argc++], "unimp");

        strcpy(argv[argc++], "-rtc");
        strcpy(argv[argc++], "clock=vm");

    } else if (SimType == QEMU_SIM_ESP32) {
        if (!load_qemu_lib("libqemu-xtensa")) {
            PICSimLab.RegisterError("Error loading libqemu-xtensa");
            qemu_started = -1;
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
            return;
        }

        // change .hex to .bin
        strncpy(fname_, fname, 2048);
        fname_[strlen(fname_) - 3] = 0;
        strncat(fname_, "bin", 2047);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, fname_)) {
            // create a empty memory
            FILE* fout;
            printf("PICSimLab: Flash file dont´t exist, creating new empty: %s.\n", fname_);
            fout = fopen_UTF8(fname_, "wb");
            if (fout) {
                unsigned char buffer[1024];
                memset(buffer, 0, 1024);
                for (unsigned int r = 0; r < (DBGGetROMSize() / 1024); r++) {
                    fwrite(buffer, 1024, sizeof(char), fout);
                }
                fclose(fout);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fname_);
                exit(-1);
            }
        } else {
            FILE* fin;
            FILE* fout;
            fin = fopen_UTF8(fname_, "rb");
            if (fin) {
                fseek(fin, 0, SEEK_END);
                long size = ftell(fin);
                fseek(fin, 0, SEEK_SET);
                printf("PICSimLab: qemu ESP32 image size is %li \n", size);

                if ((unsigned int)size != DBGGetROMSize()) {
                    char dname[2048];

                    if (PICSimLab.GetInstanceNumber()) {
                        sprintf(dname, "%s/mdump_%s_%s_%i.bin", (const char*)PICSimLab.GetHomePath().c_str(),
                                boards_list[PICSimLab.GetLab()].name_, (const char*)GetProcessorName().c_str(),
                                PICSimLab.GetInstanceNumber());
                    } else {
                        sprintf(dname, "%s/mdump_%s_%s.bin", (const char*)PICSimLab.GetHomePath().c_str(),
                                boards_list[PICSimLab.GetLab()].name_, (const char*)GetProcessorName().c_str());
                    }

                    if (strcmp(dname, fname_)) {
                        printf("PICSimLab: Loading application to address 0x%X\n", application_offset);

                        fout = fopen_UTF8(dname, "r+b");
                        if (fout) {
                            int count;
                            unsigned char buffer[1024];

                            fseek(fout, application_offset, SEEK_SET);

                            while (!feof(fin)) {
                                count = fread(buffer, 1, 1024, fin);
                                fwrite(buffer, 1, count, fout);
                            }
                            fclose(fout);
                        }
                        strncpy(fname_, dname, 2048);
                    } else {
                        printf(
                            "PICSimLab: Flash Size is wrong! Actual size: %li Expected size: %i Removing file: %s \n",
                            size, DBGGetROMSize(), fname_);
                        PICSimLab.SystemCmd(PSC_REMOVEFILE, fname_);
                        exit(-1);
                    }
                }
                fclose(fin);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fname_);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "qemu-system-xtensa");

        strcpy(argv[argc++], "-M");
        strcpy(argv[argc++], "esp32-picsimlab");

#ifndef _WIN_
        std::string fullpath = PICSimLab.GetLibPath() + "qemu/fw/";
#else
        char expath[512];
        char dpath[512];
        PICSimLab.SystemCmd(PSC_GETEXECUTABLEPATH, NULL, expath);
        PICSimLab.SystemCmd(PSC_DIRNAME, expath, dpath);
        std::string fullpath = std::string(dpath) + "/lib/qemu/fw/";
#endif

        if ((!PICSimLab.SystemCmd(PSC_FILEEXISTS, (fullpath + "esp32-v3-rom.bin").c_str())) ||
            (!PICSimLab.SystemCmd(PSC_FILEEXISTS, (fullpath + "esp32-v3-rom-app.bin").c_str()))) {
            PICSimLab.RegisterError("Error loading esp32-v3-rom.bin or esp32-v3-rom-app.bin");
            qemu_started = -1;
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
            return;
        }

        // change .bin to .efuse
        char fnefuse[2048];
        strncpy(fnefuse, fname, 2048);
        fnefuse[strlen(fnefuse) - 3] = 0;
        strncat(fnefuse, "efuse", 2047);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, fnefuse)) {  // create efuse file if it don´t exists
            FILE* fout;
            fout = fopen_UTF8(fnefuse, "wb");
            if (fout) {
                char efuse[124];
                unsigned char mac_addr[6], mac_addr_crc;

                mac_addr_crc = 0;
                srand(time(NULL));
                mac_addr[0] = rand() & 0xFE;
                mac_addr[1] = rand() & 0xFF;
                mac_addr[2] = rand() & 0xFF;
                mac_addr[3] = rand() & 0xFF;
                mac_addr[4] = rand() & 0xFF;
                mac_addr[5] = rand() & 0xFF;

                for (int j = 0; j < 6; j++) {
                    mac_addr_crc = mac_addr_crc ^ mac_addr[j];
                    for (int i = 0; i < 8; i++) {
                        if (mac_addr_crc & 0x01) {
                            mac_addr_crc = ((mac_addr_crc >> 1) ^ (0x8C)) & 0xFF;
                        } else {
                            mac_addr_crc = (mac_addr_crc >> 1) & 0xFF;
                        }
                    }
                }

                memset(efuse, 0, 124);
                // Chip revision bits
                efuse[13] = 0x80;
                efuse[22] = 0x10;

                // MAC address
                efuse[4] = mac_addr[5];
                efuse[5] = mac_addr[4];
                efuse[6] = mac_addr[3];
                efuse[7] = mac_addr[2];
                efuse[8] = mac_addr[1];
                efuse[9] = mac_addr[0];
                efuse[10] = mac_addr_crc;

                fwrite(efuse, 1, 124, fout);
                fclose(fout);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fnefuse);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "-L");
        strcpy(argv[argc++], (const char*)fullpath.c_str());

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=mtd,format=raw", fname_);

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=none,format=raw,id=efuse", fnefuse);

        strcpy(argv[argc++], "-global");
        sprintf(argv[argc++], "driver=nvram.esp32.efuse,property=drive,value=efuse");
    } else if (SimType == QEMU_SIM_ESP32_C3) {
        if (!load_qemu_lib("libqemu-riscv32")) {
            PICSimLab.RegisterError("Error loading libqemu-riscv32");
            qemu_started = -1;
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
            return;
        }

        // change .hex to .bin
        strncpy(fname_, fname, 2048);
        fname_[strlen(fname_) - 3] = 0;
        strncat(fname_, "bin", 2047);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, fname_)) {
            // create a empty memory
            FILE* fout;
            printf("PICSimLab: Flash file dont´t exist, creating new empty: %s.\n", fname_);
            fout = fopen_UTF8(fname_, "wb");
            if (fout) {
                unsigned char buffer[1024];
                memset(buffer, 0, 1024);
                for (unsigned int r = 0; r < (DBGGetROMSize() / 1024); r++) {
                    fwrite(buffer, 1024, sizeof(char), fout);
                }
                fclose(fout);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fname_);
                exit(-1);
            }
        } else {
            FILE* fin;
            FILE* fout;
            fin = fopen_UTF8(fname_, "rb");
            if (fin) {
                fseek(fin, 0, SEEK_END);
                long size = ftell(fin);
                fseek(fin, 0, SEEK_SET);
                printf("PICSimLab: qemu ESP32-C3 image size is %li \n", size);

                if ((unsigned int)size != DBGGetROMSize()) {
                    char dname[2048];

                    if (PICSimLab.GetInstanceNumber()) {
                        sprintf(dname, "%s/mdump_%s_%s_%i.bin", (const char*)PICSimLab.GetHomePath().c_str(),
                                boards_list[PICSimLab.GetLab()].name_, (const char*)GetProcessorName().c_str(),
                                PICSimLab.GetInstanceNumber());
                    } else {
                        sprintf(dname, "%s/mdump_%s_%s.bin", (const char*)PICSimLab.GetHomePath().c_str(),
                                boards_list[PICSimLab.GetLab()].name_, (const char*)GetProcessorName().c_str());
                    }

                    if (strcmp(dname, fname_)) {
                        printf("PICSimLab: Loading application to address 0x%X\n", application_offset);
                        fout = fopen_UTF8(dname, "r+b");
                        if (fout) {
                            int count;
                            unsigned char buffer[1024];

                            fseek(fout, application_offset, SEEK_SET);

                            while (!feof(fin)) {
                                count = fread(buffer, 1, 1024, fin);
                                fwrite(buffer, 1, count, fout);
                            }
                            fclose(fout);
                        }
                        strncpy(fname_, dname, 2048);
                    } else {
                        printf(
                            "PICSimLab: Flash Size is wrong! Actual size: %li Expected size: %i Removing file: %s \n",
                            size, DBGGetROMSize(), fname_);
                        PICSimLab.SystemCmd(PSC_REMOVEFILE, fname_);
                        exit(-1);
                    }
                }
                fclose(fin);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fname_);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "qemu-system-riscv32");

        strcpy(argv[argc++], "-M");
        strcpy(argv[argc++], "esp32c3-picsimlab");

#ifndef _WIN_
        std::string fullpath = PICSimLab.GetLibPath() + "qemu/fw/";
#else
        char expath[512];
        char dpath[512];
        PICSimLab.SystemCmd(PSC_GETEXECUTABLEPATH, NULL, expath);
        PICSimLab.SystemCmd(PSC_DIRNAME, expath, dpath);
        std::string fullpath = std::string(dpath) + "/lib/qemu/fw/";
#endif

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, (fullpath + "esp32c3-rom.bin").c_str())) {
            PICSimLab.RegisterError("Error loading esp32c3-rom.bin");
            qemu_started = -1;
            PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
            return;
        }

        // change .bin to .efuse
        char fnefuse[2048];
        strncpy(fnefuse, fname, 2048);
        fnefuse[strlen(fnefuse) - 3] = 0;
        strncat(fnefuse, "efuse", 2047);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, fnefuse)) {  // create efuse file if it don´t exists
            FILE* fout;
            fout = fopen_UTF8(fnefuse, "wb");
            if (fout) {
                char efuse[1024];
                unsigned char mac_addr[6], mac_addr_crc;

                mac_addr_crc = 0;
                srand(time(NULL));
                mac_addr[0] = rand() & 0xFE;
                mac_addr[1] = rand() & 0xFF;
                mac_addr[2] = rand() & 0xFF;
                mac_addr[3] = rand() & 0xFF;
                mac_addr[4] = rand() & 0xFF;
                mac_addr[5] = rand() & 0xFF;

                for (int j = 0; j < 6; j++) {
                    mac_addr_crc = mac_addr_crc ^ mac_addr[j];
                    for (int i = 0; i < 8; i++) {
                        if (mac_addr_crc & 0x01) {
                            mac_addr_crc = ((mac_addr_crc >> 1) ^ (0x8C)) & 0xFF;
                        } else {
                            mac_addr_crc = (mac_addr_crc >> 1) & 0xFF;
                        }
                    }
                }

                memset(efuse, 0, 1024);
                // Version Minor
                efuse[38] = 0x0c;

                // MAC address
                efuse[24] = mac_addr[5];
                efuse[25] = mac_addr[4];
                efuse[26] = mac_addr[3];
                efuse[27] = mac_addr[2];
                efuse[28] = mac_addr[1];
                efuse[29] = mac_addr[0];
                // efuse[10] = mac_addr_crc;

                // 128bit uid
                for (int i = 0; i < 16; i++) {
                    efuse[48 + i] = rand();
                }

                fwrite(efuse, 1, 1024, fout);
                fclose(fout);
            } else {
                printf("PICSimLab: qemu Error creating file %s \n", fnefuse);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "-L");
        strcpy(argv[argc++], (const char*)fullpath.c_str());

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=mtd,format=raw", fname_);

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=none,format=raw,id=efuse", fnefuse);

        strcpy(argv[argc++], "-global");
        sprintf(argv[argc++], "driver=nvram.esp32c3.efuse,property=drive,value=efuse");
    } else {
        printf("PICSimLab qemu: simulator %i not supported!!!\n", SimType);
        exit(-1);
    }

    // verify if serial port exists
    if (resp && ConfEnableSerial) {
        if (strstr(resp, SERIALDEVICE)) {
            // try open
            serialfd_t serialfd;
            if (serial_port_open(&serialfd, SERIALDEVICE)) {
                serial_port_close(&serialfd);
                strcpy(argv[argc++], "-serial");
                strcpy(argv[argc++], SERIALDEVICE);
                serial_open = 1;
            } else {
                serial_open = 0;
            }
        }
    }

    if ((icount >= 0) && (icount < 11)) {
        strcpy(argv[argc++], "-icount");
        sprintf(argv[argc++], "shift=%i,align=off,sleep=on", icount);
    } else if (icount == 11) {
        strcpy(argv[argc++], "-icount");
        sprintf(argv[argc++], "shift=auto,align=off,sleep=off");
    }

    if (ConfigWaitGdb) {
        strcpy(argv[(argc)++], "-S");
        PICSimLab.SetDebugStatus(1);
        PICSimLab.SetCpuState(CPU_WAITING_GDB);
        printf("PICSimLab: Waiting for GDB connection ...\n");
    }

    if (PICSimLab.GetDebugStatus()) {
        strcpy(argv[argc++], "-gdb");
        sprintf(argv[argc++], "tcp::%i", PICSimLab.GetDebugPort());
    }

    BoardOptions(&argc, argv);

    // disable extra in case of invalid qemu option finish the simulator

    char ftest[2048];
    PICSimLab.SystemCmd(PSC_GETTEMPDIR, NULL, ftest);
    strncat(ftest, "/picsimlab_qemu_fail", 1023);

    if (PICSimLab.SystemCmd(PSC_FILEEXISTS, ftest)) {
        use_cmdline_extra = 0;
        PICSimLab.RegisterError("Invalid qemu extra option!");
        PICSimLab.SystemCmd(PSC_REMOVEFILE, ftest);
    }

    if (use_cmdline_extra) {
        FILE* ft = fopen_UTF8(ftest, "w");
        fprintf(ft, "fail\n");
        fclose(ft);
    }

    if (use_cmdline_extra && cmdline_extra.length()) {
        char buff[2048];
        strncpy(buff, (const char*)cmdline_extra.c_str(), 2047);

        char* arg = strtok(buff, " \n");

        while (arg) {
            strcpy(argv[argc++], arg);
            arg = strtok(NULL, " \n");
        }
    }

    if (resp) {
        free(resp);
    }

    g_board = this;
    // printf("picsimlab: %s\n", (const char*)cmd);
    g_pins = pins;
    callbacks.pinmap = GetPinMap();
    qemu_picsimlab_register_callbacks((void*)&callbacks);
    timer.last = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer.timeout = TTIMEOUT;

    qemu_init(argc, argv, NULL);

    if (use_cmdline_extra) {
        // all options good, remove fail file
        PICSimLab.SystemCmd(PSC_REMOVEFILE, ftest);
    }

    cmdline = "";
    printf("PICSimLab: ");
    // free argv
    for (int i = 0; i < ARGMAX; i++) {
        if (argv[i]) {
            printf("%s ", argv[i]);
            cmdline += argv[i];
            cmdline += " ";
        }
        free(argv[i]);
    }
    printf("\n");

    timer.qtimer = (QEMUTimer*)malloc(sizeof(QEMUTimer));
    timer_init_full(timer.qtimer, NULL, QEMU_CLOCK_VIRTUAL, 1, 0, user_timeout_cb, this);

    timer_mod_ns(timer.qtimer, timer.last + timer.timeout);

    qemu_started = 1;
    PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
#ifndef _WIN_
    usleep(1000);
#else
    Sleep(1);
#endif

    qemu_main_loop();

    qemu_cleanup();
}

void bsim_qemu::MEnd(void) {
    if (qemu_started != 1) {
        return;
    }
    qemu_mutex_lock_iothread();
    qmp_quit(NULL);
    qemu_mutex_unlock_iothread();

    PICSimLab.ConfigMenuGUI(GMT_HEX);

#ifdef _WIN_
    Sleep(200);
#else
    usleep(200000);
#endif

    if (fname_bak[0]) {
        PICSimLab.SystemCmd(PSC_RENAMEFILE, fname_bak, fname_);
    }

    StopThread();
}

int bsim_qemu::MGetArchitecture(void) {
    if (SimType == QEMU_SIM_STM32) {
        return ARCH_STM32;
    }

    return ARCH_UNKNOWN;
}

void bsim_qemu::MEraseFlash(void) {
    // erase_flash ();
}

void bsim_qemu::MSetFreq(float freq_) {
    freq = freq_;
    inc_ns = 1000000000L / freq;
    TimerUpdateFrequency(freq);
}

float bsim_qemu::MGetFreq(void) {
    return freq;
}

void bsim_qemu::MSetVCC(float vcc) {
    printf("qemu: Incomplete!!!!\n");
}

float bsim_qemu::MGetVCC(void) {
    return 3.3;
}

float bsim_qemu::MGetInstClockFreq(void) {
    return freq;
}

int bsim_qemu::CpuInitialized(void) {
    return 1;
}

void bsim_qemu::DebugLoop(void) {}

int bsim_qemu::MDumpMemory(const char* fname) {
    if (qemu_started != 1) {
        return 1;
    }

    if (SimType == QEMU_SIM_STM32) {
        // change .hex to .bin
        strncpy(fname_, fname, 299);
        fname_[strlen(fname) - 3] = 0;
        strncat(fname_, "bin", 299);
        qemu_mutex_lock_iothread();
        qmp_stop(NULL);
        if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fname_)) {
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
            qmp_pmemsave(0x8000000, 65536, fname_bak, NULL);
        } else {
            // save file direct
#ifdef _WIN_
            for (unsigned int i = 0; i < strlen(fname_); i++) {
                if (fname_[i] == '\\')
                    fname_[i] = '/';
            }
#endif
            qmp_pmemsave(0x8000000, 65536, fname_, NULL);
        }
        qmp_cont(NULL);
        qemu_mutex_unlock_iothread();
    } else if ((SimType == QEMU_SIM_ESP32) || (SimType == QEMU_SIM_ESP32_C3)) {
        // change .hex to .bin
        strncpy(fname_, fname, 299);
        fname_[strlen(fname) - 3] = 0;
        strncat(fname_, "bin", 299);
        qemu_mutex_lock_iothread();
        qmp_stop(NULL);
        if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fname_)) {
            // save backup copy until end
            strncpy(fname_bak, fname, 299);
            fname_bak[strlen(fname) - 3] = 0;
            strncat(fname_bak, "bak", 299);
#ifdef _WIN_
            for (unsigned int i = 0; i < strlen(fname); i++) {
                if (fname_[i] == '\\')
                    fname_[i] = '/';
                if (fname_bak[i] == '\\')
                    fname_bak[i] = '/';
            }
#endif
            char* buff = new char[DBGGetROMSize()];
            qemu_picsimlab_flash_dump(0, buff, DBGGetROMSize());
            FILE* fout = fopen_UTF8(fname_bak, "wb");
            if (fout) {
                fwrite(buff, DBGGetROMSize(), 1, fout);
                fclose(fout);
            }
            delete[] buff;
        } else {
            // save file direct
#ifdef _WIN_
            for (unsigned int i = 0; i < strlen(fname_); i++) {
                if (fname_[i] == '\\')
                    fname_[i] = '/';
            }
#endif
            char* buff = new char[DBGGetROMSize()];
            qemu_picsimlab_flash_dump(0, buff, DBGGetROMSize());
            FILE* fout = fopen_UTF8(fname_, "wb");
            if (fout) {
                fwrite(buff, DBGGetROMSize(), 1, fout);
                fclose(fout);
            }
            delete[] buff;
        }
        qmp_cont(NULL);
        qemu_mutex_unlock_iothread();
    }
    return 0;
}

int bsim_qemu::DebugInit(int dtyppe)  // argument not used in picm only mplabx
{
    return 0;  //! mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}

void bsim_qemu::pins_reset(void) {
    for (int p = 0; p < MGetPinCount(); p++) {
        pins[p].avalue = 0;
        pins[p].lvalue = 0;
        pins[p].value = 0;
        pins[p].ptype = PT_DIGITAL;
        pins[p].dir = PD_IN;
        pins[p].ovalue = 0;
        pins[p].oavalue = 55;

        if ((MGetPinName(p + 1).find("VDD") != std::string::npos) ||
            (MGetPinName(p + 1).find("VIN") != std::string::npos)) {
            pins[p].value = 1;
            pins[p].dir = PD_OUT;
            pins[p].ptype = PT_POWER;
        }
        if ((MGetPinName(p + 1).find("VSS") != std::string::npos) ||
            (MGetPinName(p + 1).find("GND") != std::string::npos)) {
            pins[p].value = 0;
            pins[p].dir = PD_OUT;
            pins[p].ptype = PT_POWER;
        }
    }
}

void bsim_qemu::MSetPin(int pin, unsigned char value) {
    if (!pin)
        return;

    if (pins[pin - 1].value != value) {
        pins[pin - 1].value = value;

        qemu_picsimlab_set_pin(pin, value);
    }
}

void bsim_qemu::MSetPinDOV(int pin, unsigned char ovalue) {
    // set_pin_DOV (pin, ovalue);
}

unsigned char bsim_qemu::MGetPin(int pin) {
    if ((pin) && (pin < MGetPinCount())) {
        return pins[pin - 1].value;
    }
    return 0;
}

void bsim_qemu::MReset(int flags) {
    if (qemu_started != 1) {
        return;
    }
    PICSimLab.SystemCmd(PSC_MUTEXLOCK, (const char*)&mtx_qinitId);  // only for wait qemu start
    PICSimLab.SystemCmd(PSC_MUTEXUNLOCK, (const char*)&mtx_qinitId);
    if (flags >= 0) {
        qemu_mutex_lock_iothread();
    }
    if (!ConfigWaitGdb) {
        qmp_cont(NULL);
    }
    qmp_system_reset(NULL);
    if (flags >= 0) {
        qemu_mutex_unlock_iothread();
    }
    bitbang_i2c_rst(&master_i2c[0]);
    bitbang_i2c_rst(&master_i2c[1]);
    bitbang_spi_rst(&master_spi[0]);
    bitbang_spi_rst(&master_spi[1]);
    bitbang_uart_rst(&master_uart[0]);
    bitbang_uart_rst(&master_uart[1]);
    bitbang_uart_rst(&master_uart[2]);
    bitbang_pwm_rst(&pwm_out);
    bitbang_out_rst(&rmt_out);
}

const picpin* bsim_qemu::MGetPinsValues(void) {
    return pins;
}

void bsim_qemu::MStep(void) {
    PICSimLab.SetCpuState(CPU_RUNNING);
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
        for (int i = 0; i < pwm_out.channels_count; i++) {
            if (pwm_out.channels[i].pin && pwm_out.channels[i].enabled) {
                pins[pwm_out.channels[i].pin - 1].dir = PD_OUT;
                pins[pwm_out.channels[i].pin - 1].value = pwm_out.channels[i].out;
            }
        }
        for (int i = 0; i < rmt_out.channels_count; i++) {
            if (rmt_out.pins[i]) {
                pins[rmt_out.pins[i] - 1].dir = PD_OUT;
                pins[rmt_out.pins[i] - 1].value = rmt_out.out[i];
            }
        }
    }
}

static const char MipsStr[13][10] = {"No Limit", "1000", "500",  "250",  "125",  "62.5", "31.25",
                                     "15.63",    "7.81", "3.90", "1.95", "0.98", "Auto"};

int bsim_qemu::MipsStrToIcount(const char* mipstr) {
    int index = -1;
    for (int i = 1; i < 13; i++) {
        if (!strcmp(MipsStr[i], mipstr)) {
            index = i - 1;
            break;
        }
    }
    return index;
}

const char* bsim_qemu::IcountToMipsStr(int icount) {
    if ((icount >= 0) && (icount < 12)) {
        return MipsStr[icount + 1];
    } else {
        return MipsStr[0];
    }
}

const char* bsim_qemu::IcountToMipsItens(char* buffer) {
    buffer[0] = 0;
    for (int i = 0; i < 13; i++) {
        strcat(buffer, MipsStr[i]);
        strcat(buffer, ",");
    }
    return buffer;
}

void bsim_qemu::IoLockAccess(void) {
    qemu_mutex_lock_iothread();
}

void bsim_qemu::IoUnlockAccess(void) {
    qemu_mutex_unlock_iothread();
}

int bsim_qemu::GetUARTRX(const int uart_num) {
    if (uart_num < 3) {
        return master_uart[uart_num].rx_pin;
    }
    return 0;
}

int bsim_qemu::GetUARTTX(const int uart_num) {
    if (uart_num < 3) {
        return master_uart[uart_num].tx_pin;
    }
    return 0;
}
