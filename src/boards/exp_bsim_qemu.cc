/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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
#define INVALID_HANDLE_VALUE -1
#include <dlfcn.h>
#endif

#include "../picsimlab1.h"
#include "../serial_port.h"
#include "exp_bsim_qemu.h"

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

void (*qemu_picsimlab_register)(void (*picsimlab_write_pin)(int pin, int value));
void (*qemu_picsimlab_set_pin)(int pin, int value);
void (*qemu_picsimlab_set_apin)(int chn, int value);
int (*qemu_picsimlab_flash_dump)(int64_t offset, void* buf, int bytes);

int64_t (*qemu_clock_get_ns)(QEMUClockType type);

void (*timer_init_full)(QEMUTimer* ts, QEMUTimerListGroup* timer_list_group, QEMUClockType type, int scale,
                        int attributes, QEMUTimerCB* cb, void* opaque);

void (*timer_mod_ns)(QEMUTimer* ts, int64_t expire_time);

uint32_t* (*qemu_picsimlab_get_strap)(void);

uint32_t (*qemu_picsimlab_get_TIOCM)(void);

// global pointers to c callbacks
static picpin* g_pins;
static bsim_qemu* g_board = NULL;

void picsimlab_write_pin(int pin, int value) {
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    // printf("================> IO    <====================== %ji\n", now - g_board->timer.last);
    g_board->Run_CPU_ns((now - g_board->timer.last) / 3);
    g_board->timer.last = now;
    g_pins[pin - 1].value = value;
    // printf("pin[%i]=%i\n", pin, value);
}

int bsim_qemu::load_qemu_lib(const char* path) {
#ifndef _WIN_  // LINUX
    lxString fullpath = Window1.GetLibPath() + "qemu/" + path + ".so";

    void* handle = dlopen((const char*)fullpath.c_str(), RTLD_NOW);
    if (handle == nullptr) {
        printf("picsimlab qemu: %s\n", dlerror());
        exit(-1);
    }

#define GET_SYMBOL_AND_CHECK(X)                   \
    *((void**)(&X)) = dlsym(handle, #X);          \
    if (nullptr == X) {                           \
        printf("Qemu lib Lost symbol: " #X "\n"); \
        return 0;                                 \
    }
#else  // WINDOWS
    lxString fullpath = dirname(lxGetExecutablePath()) + "/lib/qemu/" + path + ".dll";

    HMODULE handle = LoadLibraryA((const char*)fullpath.c_str());
    if (handle == NULL) {
        printf("picsimlab qemu: Error loading %s\n", (const char*)fullpath.c_str());
        exit(-1);
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
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_register);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_set_pin);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_set_apin);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_flash_dump);
    GET_SYMBOL_AND_CHECK(qemu_clock_get_ns);
    GET_SYMBOL_AND_CHECK(timer_init_full);
    GET_SYMBOL_AND_CHECK(timer_mod_ns);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_get_strap);
    GET_SYMBOL_AND_CHECK(qemu_picsimlab_get_TIOCM);
#undef GET_SYMBOL_AND_CHECK

    return 1;
}

bsim_qemu::bsim_qemu(void) {
    fname_bak[0] = 0;
    fname_[0] = 0;

    SimType = QEMU_SIM_NONE;

    qemu_started = 0;

    memset(&ADCvalues, 0xFF, 32);

    Window1.SetNeedReboot();
    mtx_qinit = new lxMutex();
    ns_count = 0;
    icount = 5;
}

bsim_qemu::~bsim_qemu(void) {
    delete mtx_qinit;
}

void bsim_qemu::MSetSerial(const char* port) {}

int bsim_qemu::MInit(const char* processor, const char* _fname, float freq) {
    strcpy(fname, _fname);

    lxString sproc = GetSupportedDevices();
    if (!sproc.Contains(processor)) {
        Proc = "stm32f103c8t6";
        printf("PICSimLab: Unknown processor %s, loading default !\n", processor);
    }

    pins_reset();

    serialfd[0] = INVALID_HANDLE_VALUE;
    serialfd[1] = INVALID_HANDLE_VALUE;
    serialfd[2] = INVALID_HANDLE_VALUE;
    serialfd[3] = INVALID_HANDLE_VALUE;

    if (!qemu_started) {
        StartThread();
#ifndef _WIN_
        usleep(100);
#else
        Sleep(1);
#endif
        mtx_qinit->Lock();  // only for wait qemu start
        mtx_qinit->Unlock();

        Window1.menu1_File_LoadHex.SetText("Load Bin");
        Window1.menu1_File_SaveHex.SetEnable(0);
        Window1.filedialog1.SetFileName(lxT("untitled.bin"));
        Window1.filedialog1.SetFilter(lxT("Bin Files (*.bin)|*.bin;*.BIN"));

        qemu_started = 1;
    } else {
        printf("PICSimLab: qemu already started !!!!!\n");
    }

    return 0;  // ret;
}

static void user_timeout_cb(void* opaque) {
    bsim_qemu* board = (bsim_qemu*)opaque;
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer_mod_ns(board->timer.qtimer, now + board->timer.timeout);
    // printf("================> Timer <====================== %ji\n", now - board->timer.last);
    board->Run_CPU_ns((now - board->timer.last) / 3);
    board->timer.last = now;
}

void bsim_qemu::EvThreadRun(CThread& thread) {
    mtx_qinit->Lock();

    // test icount limits
    if (icount < -1) {
        icount = -1;
    }
    if (icount > 10) {
        icount = 10;
    }

    char* resp = serial_port_list();

#define ARGMAX 20

    char* argv[ARGMAX];
    int argc = 0;

    for (int i = 0; i < ARGMAX; i++) {
        argv[i] = (char*)malloc(1024);
    }

    if (SimType == QEMU_SIM_STM32) {
        // FIXME use picsimlab lib folder
        load_qemu_lib("libqemu-stm32");

        // change .hex to .bin
        strncpy(fname_, fname, 2048);
        fname_[strlen(fname_) - 3] = 0;
        strncat(fname_, "bin", 2047);

        if (!lxFileExists(fname_)) {
            // create a empty memory
            FILE* fout;
            fout = fopen(fname_, "w");
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
                printf("picsimlab: qemu Erro creating file %s \n", fname_);
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
    } else if (SimType == QEMU_SIM_ESP32) {
        // FIXME use picsimlab lib folder
        load_qemu_lib("libqemu-xtensa");

        // change .hex to .bin
        strncpy(fname_, fname, 2048);
        fname_[strlen(fname_) - 3] = 0;
        strncat(fname_, "bin", 2047);

        if (!lxFileExists(fname_)) {
            // create a empty memory
            FILE* fout;
            fout = fopen(fname_, "w");
            if (fout) {
                unsigned char buffer[1024];
                memset(buffer, 0, 1024);
                for (int r = 0; r < 4096; r++) {
                    fwrite(buffer, 1024, sizeof(char), fout);
                }
                fclose(fout);
            } else {
                printf("picsimlab: qemu Erro creating file %s \n", fname_);
                exit(-1);
            }
        }

        strcpy(argv[argc++], "qemu-system-xtensa");

        strcpy(argv[argc++], "-M");
        strcpy(argv[argc++], "esp32-picsimlab");

        strcpy(argv[argc++], "-drive");
        sprintf(argv[argc++], "file=%s,if=mtd,format=raw", fname_);
        strcpy(argv[argc++], "-nic");
        strcpy(argv[argc++], "user,model=esp32_wifi,net=192.168.4.0/24,hostfwd=tcp::16555-192.168.4.15:80");

#ifndef _WIN_
        lxString fullpath = Window1.GetLibPath() + "qemu/fw/";
#else
        lxString fullpath = dirname(lxGetExecutablePath()) + "/lib/qemu/fw/";
#endif
        strcpy(argv[argc++], "-L");
        strcpy(argv[argc++], (const char*)fullpath.c_str());

        // strcpy(argv[argc++], "-global");
        // strcpy(argv[argc++], "driver=esp32.gpio,property=strap_mode,value=0x0f");

        if (icount < 0) {
            strcpy(argv[argc++], "-global");
            strcpy(argv[argc++], "driver=timer.esp32.timg,property=wdt_disable,value=true");
        }
    } else {
        printf("picsimlab qemu: simulator %i not supported!!!\n", SimType);
        exit(-1);
    }

    // verify if serial port exists
    if (strstr(resp, SERIALDEVICE)) {
        strcpy(argv[argc++], "-serial");
        strcpy(argv[argc++], SERIALDEVICE);
    }
    if (Window1.Get_debug_status()) {
        strcpy(argv[argc++], "-gdb");
        sprintf(argv[argc++], "tcp::%i", Window1.Get_debug_port());
    }

    // strcpy(argv[argc++], "-d");
    // strcpy(argv[argc++], "unimp");

    if (icount >= 0) {
        strcpy(argv[argc++], "-icount");
        sprintf(argv[argc++], "shift=%i,align=off,sleep=off", icount);
    }
    strcpy(argv[argc++], "-rtc");
    strcpy(argv[argc++], "clock=vm");

    free(resp);

    // printf("picsimlab: %s\n", (const char*)cmd);
    g_pins = pins;
    qemu_picsimlab_register(picsimlab_write_pin);
    g_board = this;
    timer.last = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer.timeout = 10000000L;

    qemu_init(argc, argv, NULL);

    // free argv
    for (int i = 0; i < ARGMAX; i++) {
        free(argv[i]);
    }

    timer.qtimer = (QEMUTimer*)malloc(sizeof(QEMUTimer));
    timer_init_full(timer.qtimer, NULL, QEMU_CLOCK_VIRTUAL, 1, 0, user_timeout_cb, this);

    timer_mod_ns(timer.qtimer, timer.last + timer.timeout);

    mtx_qinit->Unlock();

#ifndef _WIN_
    usleep(100);
#else
    Sleep(1);
#endif

    qemu_main_loop();

    qemu_cleanup();
}

void bsim_qemu::MEnd(void) {
    qemu_mutex_lock_iothread();
    qmp_quit(NULL);
    qemu_mutex_unlock_iothread();

    Window1.menu1_File_LoadHex.SetText("Load Hex");
    Window1.menu1_File_SaveHex.SetEnable(1);
    Window1.filedialog1.SetFileName(lxT("untitled.hex"));
    Window1.filedialog1.SetFilter(lxT("Hex Files (*.hex)|*.hex;*.HEX"));

#ifdef _WIN_
    Sleep(200);
#else
    usleep(200000);
#endif

    if (fname_bak[0]) {
        lxRenameFile(fname_bak, fname_);
    }
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

void bsim_qemu::MDumpMemory(const char* fname) {
    if (SimType == QEMU_SIM_STM32) {
        // change .hex to .bin
        strncpy(fname_, fname, 299);
        fname_[strlen(fname) - 3] = 0;
        strncat(fname_, "bin", 299);
        qemu_mutex_lock_iothread();
        qmp_stop(NULL);
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
    } else if (SimType == QEMU_SIM_ESP32) {
        // change .hex to .bin
        strncpy(fname_, fname, 299);
        fname_[strlen(fname) - 3] = 0;
        strncat(fname_, "bin", 299);
        qemu_mutex_lock_iothread();
        qmp_stop(NULL);
        if (lxFileExists(fname_)) {
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
            char buff[4194304];
            qemu_picsimlab_flash_dump(0, buff, 4194304);
            FILE* fout = fopen(fname_bak, "w");
            if (fout) {
                fwrite(buff, 4194304, 1, fout);
                fclose(fout);
            }
        } else {
            // save file direct
#ifdef _WIN_
            for (unsigned int i = 0; i < strlen(fname_); i++) {
                if (fname_[i] == '\\')
                    fname_[i] = '/';
            }
#endif
            char buff[4194304];
            qemu_picsimlab_flash_dump(0, buff, 4194304);
            FILE* fout = fopen(fname_, "w");
            if (fout) {
                fwrite(buff, 4194304, 1, fout);
                fclose(fout);
            }
        }
        qmp_cont(NULL);
        qemu_mutex_unlock_iothread();
    }
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
        pins[p].oavalue = 0;

        if (MGetPinName(p + 1).Contains("VDD")) {
            pins[p].value = 1;
            pins[p].dir = PD_OUT;
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
    mtx_qinit->Lock();  // only for wait qemu start
    mtx_qinit->Unlock();
    if (flags >= 0) {
        qemu_mutex_lock_iothread();
    }
    qmp_cont(NULL);
    qmp_system_reset(NULL);
    if (flags >= 0) {
        qemu_mutex_unlock_iothread();
    }
}

const picpin* bsim_qemu::MGetPinsValues(void) {
    return pins;
}

void bsim_qemu::MStep(void) {}

void bsim_qemu::MStepResume(void) {}

static const char MipsStr[12][10] = {"No Limit", "1000",  "500",  "250",  "125",  "62.5",
                                     "31.25",    "15.63", "7.81", "3.90", "1.95", "0.98"};

int bsim_qemu::MipsStrToIcount(const char* mipstr) {
    int index = -1;
    for (int i = 1; i < 12; i++) {
        if (!strcmp(MipsStr[i], mipstr)) {
            index = i - 1;
            break;
        }
    }
    return index;
}

const char* bsim_qemu::IcountToMipsStr(int icount) {
    if ((icount >= 0) && (icount < 11)) {
        return MipsStr[icount + 1];
    } else {
        return MipsStr[0];
    }
}

const char* bsim_qemu::IcountToMipsItens(char* buffer) {
    buffer[0] = 0;
    for (int i = 0; i < 12; i++) {
        strcat(buffer, MipsStr[i]);
        strcat(buffer, ",");
    }
    return buffer;
}
