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

// main window

// #define CONVERTER_MODE

// print timer debug info
// #define TDEBUG

#include "picsimlab1.h"
#include "picsimlab1_d.cc"

CPWindow1 Window1;

// Implementation

#include "picsimlab2.h"
#include "picsimlab3.h"
#include "picsimlab4.h"
#include "picsimlab5.h"

#include "lib/oscilloscope.h"
#include "lib/picsimlab.h"
#include "lib/spareparts.h"

#include "lib/rcontrol.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#ifdef _WIN_
#include <imagehlp.h>
#include <windows.h>
#else
#include <err.h>
#include <execinfo.h>
#include <signal.h>
#endif
#endif
#include <curl/curl.h>

#ifdef _USE_PICSTARTP_
// picstart plus
int prog_init(void);
int prog_loop(_pic* pic);
int prog_end(void);
#endif

#ifdef CONVERTER_MODE
static std::string cvt_fname;
#endif

#ifdef _WIN_

double cpuTime() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return (double)(d.dwLowDateTime | ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    } else {
        //  Handle error
        return 0;
    }
}
#else
#include <sys/time.h>
#include <time.h>

double cpuTime() {
    return (double)clock() / CLOCKS_PER_SEC;
}
#endif

extern "C" {
void file_ready(const char* fname, const char* dir = NULL);
}

void CPWindow1::timer1_EvOnTime(CControl* control) {
    // avoid run again before terminate previous
    if (PICSimLab.status & (ST_T1 | ST_DI))
        return;

    PICSimLab.SetSync(1);
    PICSimLab.status |= ST_T1;

#ifdef _NOTHREAD
    // printf ("overtimer = %i \n", timer1.GetOverTime ());
    if (timer1.GetOverTime() < BASETIMER)
#else
    if ((!PICSimLab.tgo) && (timer1.GetTime() == BASETIMER))
#endif
    {
        if (crt) {
            label2.SetColor(SystemColor(lxCOLOR_WINDOWTEXT));
            label2.Draw();
        }
        crt = 0;
    } else {
        if (!crt) {
            label2.SetColor(255, 0, 0);
            label2.Draw();
        }
        crt = 1;
    }

    if (!PICSimLab.tgo) {
        zerocount++;

        if (zerocount > 3) {
            zerocount = 0;

            if (timer1.GetTime() > BASETIMER) {
                timer1.SetTime(timer1.GetTime() - 5);
            }
        }
    } else {
        zerocount = 0;
    }

    PICSimLab.tgo++;
#ifndef _NOTHREAD
    {
        std::unique_lock<std::mutex> lk(cpu_mutex);
        cpu_cond.notify_one();
    }
#endif

    if (PICSimLab.tgo > 3) {
        if (timer1.GetTime() < 330) {
            timer1.SetTime(timer1.GetTime() + 5);
        }
        PICSimLab.tgo = 1;
    }

    DrawBoard();

    PICSimLab.status &= ~ST_T1;
}

CPWindow1::~CPWindow1(void) {}

void CPWindow1::DrawBoard(void) {
    if (PICSimLab.GetNeedResize()) {
        double scalex, scaley, scale_temp;

        scalex = ((Window1.GetClientWidth() - 175) * 1.0) / PICSimLab.plWidth;
        scaley = ((Window1.GetClientHeight() - 10) * 1.0) / PICSimLab.plHeight;

        if (scalex < 0.1)
            scalex = 0.1;
        if (scaley < 0.1)
            scaley = 0.1;
        if (scalex > 4)
            scalex = 4;
        if (scaley > 4)
            scaley = 4;

        if (scalex < scaley)
            scale_temp = scalex;
        else
            scale_temp = scaley;

        if (PICSimLab.GetScale() != scale_temp) {
            PICSimLab.SetScale(scale_temp);

            int nw = (PICSimLab.plWidth * PICSimLab.GetScale());
            if (nw == 0)
                nw = 1;
            int nh = (PICSimLab.plHeight * PICSimLab.GetScale());
            if (nh == 0)
                nh = 1;

            PICSimLab.SetScale(((double)nw) / PICSimLab.plWidth);

            draw1.SetWidth(nw);
            draw1.SetHeight(nh);

            draw1.SetVisible(0);
            draw1.SetImgFileName(
                GetLocalFile(PICSimLab.GetSharePath() + "boards/" + PICSimLab.GetBoard()->GetPictureFileName()),
                PICSimLab.GetScale(), PICSimLab.GetScale());
        }

        if (PICSimLab.GetBoard()) {
            PICSimLab.GetBoard()->SetScale(PICSimLab.GetScale());
            PICSimLab.GetBoard()->EvOnShow();
            PICSimLab.GetBoard()->Draw();
        }
        draw1.SetVisible(1);

        if (PICSimLab.GetBoard()->GetUseOscilloscope()) {
            menu1_Modules_Oscilloscope_EvMenuActive(this);
        }
        if (PICSimLab.GetBoard()->GetUseSpareParts()) {
            Window5.Show();
            Window5.timer1.SetRunState(1);
        }
        PICSimLab.SetNeedResize(0);
        statusbar1.Draw();
    } else if (PICSimLab.GetBoard()) {
        PICSimLab.GetBoard()->Draw();
    }
#ifndef _WIN_
    Draw();
#endif
}

void CPWindow1::thread1_EvThreadRun(CControl*) {
    double t0, t1, etime;
    do {
        if (PICSimLab.tgo) {
            t0 = cpuTime();

            PICSimLab.status |= ST_TH;
            PICSimLab.GetBoard()->Run_CPU();
            if (PICSimLab.GetDebugStatus())
                PICSimLab.GetBoard()->DebugLoop();
            PICSimLab.tgo--;
            PICSimLab.status &= ~ST_TH;

            t1 = cpuTime();

#if defined(_NOTHREAD)
            /*
            if ((t1 - t0) / (Window1.timer1.GetTime ()*1e-5) > 110)
             {
              tgo++;
             }
            else
             {
              tgo = 0;
             }
             */
            PICSimLab.tgo = 0;
#endif
            etime = t1 - t0;
            PICSimLab.SetIdleMs((PICSimLab.GetIdleMs() * 0.9) + ((Window1.timer1.GetTime() - etime * 1000) * 0.1));
#ifdef TDEBUG
            float ld = (etime) / (Window1.timer1.GetTime() * 1e-5);
            printf("PTime= %lf  tgo= %2i  zeroc= %2i  Timer= %3u Perc.= %5.1lf Idle= %5.1lf\n", etime, tgo, zerocount,
                   Window1.timer1.GetTime(), ld, PICSimLab.GetIdleMs());
#endif
            if (PICSimLab.GetIdleMs() < 0)
                PICSimLab.SetIdleMs(0);
        } else {
#ifndef _NOTHREAD
            {
                std::unique_lock<std::mutex> lk(cpu_mutex);
                cpu_cond.wait(lk);
            }
#endif
        }

    } while (!thread1.TestDestroy());
}

void CPWindow1::thread2_EvThreadRun(CControl*) {
    do {
        usleep(1000);
        if (rcontrol_loop()) {
            usleep(100000);
        }
    } while (!thread2.TestDestroy());
}

void CPWindow1::thread3_EvThreadRun(CControl*) {
    PICSimLab.GetBoard()->EvThreadRun();
}

void CPWindow1::timer2_EvOnTime(CControl* control) {
    // avoid run again before terminate previous
    if (PICSimLab.status & (ST_T2 | ST_DI))
        return;

    PICSimLab.status |= ST_T2;
    if (PICSimLab.GetBoard() != NULL) {
        PICSimLab.GetBoard()->RefreshStatus();

        switch (PICSimLab.GetCpuState()) {
            case CPU_RUNNING:
                statusbar1.SetField(0, "Running...");
                break;
            case CPU_STEPPING:
                statusbar1.SetField(0, "Stepping...");
                break;
            case CPU_HALTED:
                statusbar1.SetField(0, "Halted!");
                break;
            case CPU_BREAKPOINT:
                statusbar1.SetField(0, "BreakPoint!");
                break;
            case CPU_POWER_OFF:
                statusbar1.SetField(0, "Power Off!");
                break;
            case CPU_ERROR:
                statusbar1.SetField(0, "Error!");
                break;
            case CPU_WAITING_GDB:
                statusbar1.SetField(0, "Waiting for GDB...");
                break;
        }
    }

    label2.SetText(FloatStrFormat("Spd: %3.2fx", ((float)BASETIMER) / timer1.GetTime()));

    if (PICSimLab.GetErrorCount()) {
#ifndef __EMSCRIPTEN__
        Message_sz(lxString::FromUTF8(PICSimLab.GetError(0).c_str()), 600, 240);
#else
        printf("Error: %s\n", PICSimLab.GetError(0).c_str());
#endif
        PICSimLab.DeleteError(0);
    }
    PICSimLab.status &= ~ST_T2;

#ifdef CONVERTER_MODE
    if (cvt_fname.Length() > 3) {
        SaveWorkspace(cvt_fname);
    }
#endif

    if (GetNeedClkUpdate()) {
        PICSimLab.SetClock(PICSimLab.GetClock());
    }

    int reason = PICSimLab.GetToDestroy();
    switch (reason) {
        case RC_EXIT:
            WDestroy();
            break;
        case RC_LOAD: {
            PICSimLab.LoadHexFile(rcontrol_get_file_to_load());
            WDestroy();
        } break;
        default:
            break;
    }
}

void CPWindow1::draw1_EvMouseMove(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                  unsigned int state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseMove(button, x, y, state);
}

void CPWindow1::draw1_EvMouseButtonPress(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseButtonPress(button, x, y, state);
}

void CPWindow1::draw1_EvMouseButtonRelease(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseButtonRelease(button, x, y, state);
}

void CPWindow1::draw1_EvKeyboardPress(CControl* control, const unsigned int key, const unsigned int hkey,
                                      const unsigned int mask) {
    PICSimLab.GetBoard()->EvKeyPress(key, mask);
}

void CPWindow1::draw1_EvKeyboardRelease(CControl* control, const unsigned int key, const unsigned int hkey,
                                        const unsigned int mask) {
    PICSimLab.GetBoard()->EvKeyRelease(key, mask);
}

#ifndef __EMSCRIPTEN__
// https://www.gamedev.net/forums/topic/457984-walking-the-stack-in-c-with-mingw32/
#ifdef _WIN_
static void windows_print_stacktrace(CONTEXT* context) {
    SymInitialize(GetCurrentProcess(), 0, true);

    STACKFRAME frame = {0};

#if defined(_M_AMD64)
    const DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    /* setup initial stack frame */
    frame.AddrPC.Offset = context->Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context->Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context->Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
#else
    const DWORD machine = IMAGE_FILE_MACHINE_I386;
    /* setup initial stack frame */
    frame.AddrPC.Offset = context->Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context->Esp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context->Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;
#endif

    while (StackWalk(machine, GetCurrentProcess(), GetCurrentThread(), &frame, context, 0, SymFunctionTableAccess,
                     SymGetModuleBase, 0)) {
        printf("PICSimLab stack: %p\n", (void*)frame.AddrPC.Offset);
    }

    SymCleanup(GetCurrentProcess());
}

static LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS* ExceptionInfo) {
    switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            fputs("PICSimLab Error: EXCEPTION_ACCESS_VIOLATION\n", stderr);
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            fputs("PICSimLab Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n", stderr);
            break;
        case EXCEPTION_BREAKPOINT:
            fputs("PICSimLab Error: EXCEPTION_BREAKPOINT\n", stderr);
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            fputs("PICSimLab Error: EXCEPTION_DATATYPE_MISALIGNMENT\n", stderr);
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            fputs("PICSimLab Error: EXCEPTION_FLT_DENORMAL_OPERAND\n", stderr);
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            fputs("PICSimLab Error: EXCEPTION_FLT_DIVIDE_BY_ZERO\n", stderr);
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            fputs("PICSimLab Error: EXCEPTION_FLT_INEXACT_RESULT\n", stderr);
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            fputs("PICSimLab Error: EXCEPTION_FLT_INVALID_OPERATION\n", stderr);
            break;
        case EXCEPTION_FLT_OVERFLOW:
            fputs("PICSimLab Error: EXCEPTION_FLT_OVERFLOW\n", stderr);
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            fputs("PICSimLab Error: EXCEPTION_FLT_STACK_CHECK\n", stderr);
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            fputs("PICSimLab Error: EXCEPTION_FLT_UNDERFLOW\n", stderr);
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            fputs("PICSimLab Error: EXCEPTION_ILLEGAL_INSTRUCTION\n", stderr);
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            fputs("PICSimLab Error: EXCEPTION_IN_PAGE_ERROR\n", stderr);
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            fputs("PICSimLab Error: EXCEPTION_INT_DIVIDE_BY_ZERO\n", stderr);
            break;
        case EXCEPTION_INT_OVERFLOW:
            fputs("PICSimLab Error: EXCEPTION_INT_OVERFLOW\n", stderr);
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            fputs("PICSimLab Error: EXCEPTION_INVALID_DISPOSITION\n", stderr);
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            fputs("PICSimLab Error: EXCEPTION_NONCONTINUABLE_EXCEPTION\n", stderr);
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            fputs("PICSimLab Error: EXCEPTION_PRIV_INSTRUCTION\n", stderr);
            break;
        case EXCEPTION_SINGLE_STEP:
            fputs("PICSimLab Error: EXCEPTION_SINGLE_STEP\n", stderr);
            break;
        case EXCEPTION_STACK_OVERFLOW:
            fputs("PICSimLab Error: EXCEPTION_STACK_OVERFLOW\n", stderr);
            break;
        default:
            fputs("PICSimLab Error: Unrecognized Exception\n", stderr);
            break;
    }
    fflush(stderr);
    /* If this is a stack overflow then we can't walk the stack, so just show
      where the error happened */
    if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode) {
        windows_print_stacktrace(ExceptionInfo->ContextRecord);
    } else {
#if defined(_M_AMD64)
        printf("PICSimLab Error: %p\n", (void*)ExceptionInfo->ContextRecord->Rip);
#else
        printf("PICSimLab Error: %p\n", (void*)ExceptionInfo->ContextRecord->Eip);
#endif
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

static void set_signal_handler(void) {
    SetUnhandledExceptionFilter(windows_exception_handler);
}
#else

#define MAX_STACK_FRAMES 64
static void* stack_traces[MAX_STACK_FRAMES];
static void posix_print_stack_trace() {
    int i, trace_size = 0;
    char** messages = (char**)NULL;

    trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    messages = backtrace_symbols(stack_traces, trace_size);

    /* skip the first couple stack frames (as they are this function and
     our handler) and also skip the last frame as it's (always?) junk. */
    for (i = 3; i < (trace_size - 1); ++i) {
        printf("PICSimLab Stack[%02i]: %s\n", i - 3, messages[i]);
    }
    if (messages) {
        free(messages);
    }
}

static void posix_signal_handler(int sig, siginfo_t* siginfo, void* context) {
    (void)context;
    switch (sig) {
        case SIGSEGV:
            fputs("PICSimLab Caught SIGSEGV: Segmentation Fault\n", stderr);
            break;
        case SIGINT:
            fputs("PICSimLab Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n", stderr);
            break;
        case SIGFPE:
            switch (siginfo->si_code) {
                case FPE_INTDIV:
                    fputs("PICSimLab Caught SIGFPE: (integer divide by zero)\n", stderr);
                    break;
                case FPE_INTOVF:
                    fputs("PICSimLab Caught SIGFPE: (integer overflow)\n", stderr);
                    break;
                case FPE_FLTDIV:
                    fputs("PICSimLab Caught SIGFPE: (floating-point divide by zero)\n", stderr);
                    break;
                case FPE_FLTOVF:
                    fputs("PICSimLab Caught SIGFPE: (floating-point overflow)\n", stderr);
                    break;
                case FPE_FLTUND:
                    fputs("PICSimLab Caught SIGFPE: (floating-point underflow)\n", stderr);
                    break;
                case FPE_FLTRES:
                    fputs("PICSimLab Caught SIGFPE: (floating-point inexact result)\n", stderr);
                    break;
                case FPE_FLTINV:
                    fputs("PICSimLab Caught SIGFPE: (floating-point invalid operation)\n", stderr);
                    break;
                case FPE_FLTSUB:
                    fputs("PICSimLab Caught SIGFPE: (subscript out of range)\n", stderr);
                    break;
                default:
                    fputs("PICSimLab Caught SIGFPE: Arithmetic Exception\n", stderr);
                    break;
            }
        case SIGILL:
            switch (siginfo->si_code) {
                case ILL_ILLOPC:
                    fputs("PICSimLab Caught SIGILL: (illegal opcode)\n", stderr);
                    break;
                case ILL_ILLOPN:
                    fputs("PICSimLab Caught SIGILL: (illegal operand)\n", stderr);
                    break;
                case ILL_ILLADR:
                    fputs("PICSimLab Caught SIGILL: (illegal addressing mode)\n", stderr);
                    break;
                case ILL_ILLTRP:
                    fputs("PICSimLab Caught SIGILL: (illegal trap)\n", stderr);
                    break;
                case ILL_PRVOPC:
                    fputs("PICSimLab Caught SIGILL: (privileged opcode)\n", stderr);
                    break;
                case ILL_PRVREG:
                    fputs("PICSimLab Caught SIGILL: (privileged register)\n", stderr);
                    break;
                case ILL_COPROC:
                    fputs("PICSimLab Caught SIGILL: (coprocessor error)\n", stderr);
                    break;
                case ILL_BADSTK:
                    fputs("PICSimLab Caught SIGILL: (internal stack error)\n", stderr);
                    break;
                default:
                    fputs("PICSimLab Caught SIGILL: Illegal Instruction\n", stderr);
                    break;
            }
            break;
        case SIGTERM:
            fputs("PICSimLab Caught SIGTERM: a termination request was sent to the program\n", stderr);
            break;
        case SIGABRT:
            fputs("PICSimLab Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
            break;
        default:
            break;
    }
    posix_print_stack_trace();
    _Exit(1);
}

static uint8_t* alternate_stack;
static void set_signal_handler(void) {
    /* setup alternate stack */
    {
        alternate_stack = (uint8_t*)malloc(SIGSTKSZ);

        stack_t ss = {};
        /* malloc is usually used here, I'm not 100% sure my static allocation
           is valid but it seems to work just fine. */
        ss.ss_sp = (void*)alternate_stack;
        ss.ss_size = SIGSTKSZ;
        ss.ss_flags = 0;

        if (sigaltstack(&ss, NULL) != 0) {
            err(1, "sigaltstack");
        }
    }

    /* register our signal handlers */
    {
        struct sigaction sig_action = {};
        sig_action.sa_sigaction = posix_signal_handler;
        sigemptyset(&sig_action.sa_mask);

#ifdef __APPLE__
        /* for some reason we backtrace() doesn't work on osx
           when we use an alternate stack */
        sig_action.sa_flags = SA_SIGINFO;
#else
        sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
#endif

        if (sigaction(SIGSEGV, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
        if (sigaction(SIGFPE, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
        if (sigaction(SIGINT, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
        if (sigaction(SIGILL, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
        if (sigaction(SIGTERM, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
        if (sigaction(SIGABRT, &sig_action, NULL) != 0) {
            err(1, "sigaction");
        }
    }
}
#endif
#else
static void set_signal_handler(void) {};
#endif

void CPWindow1::_EvOnCreate(CControl* control) {
    char home[1024];
    lxFileName fn;
    lxFileName fn_spare;
    char fname[1200];
    char fname_error[1200];
    int close_error = 0;

    set_signal_handler();

    strncpy(home, (const char*)lxGetUserDataDir("picsimlab").c_str(), 1023);
    PICSimLab.SetWorkspaceFileName("");
    PICSimLab.SetHomePath(home);
    PICSimLab.SetPath((const char*)lxGetCwd().c_str());

    PICSimLab.OnUpdateStatus = &CPWindow1::OnUpdateStatus;
    PICSimLab.OnConfigure = &CPWindow1::OnConfigure;
    PICSimLab.OnClockSet = &CPWindow1::OnClockSet;
    PICSimLab.OnReadPreferences = &CPWindow1::OnReadPreferences;
    PICSimLab.OnSavePrefs = &CPWindow1::OnSavePrefs;
    PICSimLab.OnLoadHexFile = &CPWindow1::OnLoadHexFile;
    PICSimLab.OnOpenLoadHexFileDialog = &CPWindow1::OnOpenLoadHexFileDialog;
    PICSimLab.OnEndSimulation = &CPWindow1::OnEndSimulation;
    PICSimLab.OnUpdateGUI = &CPWindow1::OnUpdateGUI;
    PICSimLab.OnConfigMenuGUI = &CPWindow1::OnConfigMenuGUI;
    PICSimLab.OnCanvasCmd = &CPWindow1::OnCanvasCmd;
    PICSimLab.OnWindowCmd = &CPWindow1::OnWindowCmd;
    PICSimLab.OnSystemCmd = &CPWindow1::OnSystemCmd;

    SpareParts.OnCanvasCmd = &CPWindow5::OnCanvasCmd;
    SpareParts.OnWindowCmd = &CPWindow5::OnWindowCmd;

    Oscilloscope.OnWindowCmd = &CPWindow4::OnWindowCmd;

    PICSimLab.Init();

    // board menu
    for (int i = 0; i < BOARDS_LAST; i++) {
        MBoard[i].SetFOwner(this);
        MBoard[i].SetName(std::to_string(i));
        MBoard[i].SetText(boards_list[i].name);
        MBoard[i].EvMenuActive = EVMENUACTIVE & CPWindow1::menu1_EvBoard;
        menu1_Board.CreateChild(&MBoard[i]);
    }

    Oscilloscope.Init();

    SpareParts.Init();

#ifndef _SHARE_
#error Define the _SHARE_ path is necessary
#endif

    if (std::string(_SHARE_).find("http") != std::string::npos) {
        PICSimLab.SetSharePath(std::string(_SHARE_));
    } else {
        PICSimLab.SetSharePath((const char*)(dirname(lxGetExecutablePath()) + "/" + std::string(_SHARE_)).c_str());
    }
    fn.Assign(PICSimLab.GetSharePath());
    fn.MakeAbsolute();
    PICSimLab.SetSharePath((const char*)(fn.GetFullPath() + "/").c_str());

#ifndef _LIB_
#error Define the _LIB_ path is necessary
#endif

#ifndef _VERSION_
#error Define the _VERSION_ path is necessary
#endif

#ifndef _DATE_
#error Define the _DATE_ path is necessary
#endif

#ifndef _ARCH_
#error Define the _ARCH_ path is necessary
#endif

#ifndef _PKG_
#error Define the _PKG_ path is necessary
#endif

    PICSimLab.SetLibPath((const char*)(dirname(lxGetExecutablePath()) + "/" + std::string(_LIB_)).c_str());
    fn.Assign(PICSimLab.GetLibPath());
    fn.MakeAbsolute();
    PICSimLab.SetLibPath((const char*)(fn.GetFullPath() + "/").c_str());

#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
    snprintf(fname, 1199, "%s/picsimlab_log%i.txt", home, PICSimLab.GetInstanceNumber());
    printf("PICSimLab: Console output redirected to file: \"%s\"\n", fname);
    if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fname)) {
        FILE* flog = fopen_UTF8(fname, "r");
        if (flog) {
            char line[1024];
            int finishok = 0;
            while (fgets(line, 1023, flog)) {
                line[1023] = 0;
                if (!strncmp(line, "PICSimLab: Finish Ok", 20)) {
                    finishok++;
                }
            }
            fclose(flog);
            if (finishok != 1) {
                close_error = 1;
                snprintf(fname_error, 1199, "%s/picsimlab_error%i.txt", home, PICSimLab.GetInstanceNumber());
                lxRenameFile(fname, fname_error);
            }
        }
    }

#ifdef _WIN_
    if (AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);
    }
#endif
    lxCreateDir(home);
    if (freopen(fname, "w", stdout) == NULL) {
        printf("PICSimLab: stdout redirect error [%i] %s \n", errno, strerror(errno));
    }

    if (dup2(fileno(stdout), fileno(stderr)) == -1) {
        printf("PICSimLab: stderr redirect error [%i] %s \n", errno, strerror(errno));
    }
    printf("PICSimLab: Console output redirected to file: \"%s\"\n", fname);
#endif

    printf("PICSimLab: Version \"%s %s %s %s\"\n", _VERSION_, _DATE_, _ARCH_, _PKG_);

    printf("PICSimLab: Command Line: ");
    for (int i = 0; i < Application->Aargc; i++) {
#ifdef wxUSE_UNICODE
        printf("%s ", (const char*)lxString(Application->Aargvw[i]).utf8_str());
#else
        printf("%s ", Application->Aargv[i]);
#endif
    }
    printf("\n");

    fflush(stdout);

    if (Application->Aargc == 2) {  // only .pzw file
#ifdef wxUSE_UNICODE
        fn.Assign(Application->Aargvw[1]);
#else
        fn.Assign(Application->Aargv[1]);
#endif
        fn.MakeAbsolute();
        // load options
        PICSimLab.Configure(home, 1, 1);

        // check if it is a demonstration

        std::string fns = (const char*)fn.GetFullPath().utf8_str();

        lxFileName fn_dir;
        fn_dir.Assign(PICSimLab.GetSharePath() + "boards/");
        fn_dir.MakeAbsolute();

        if ((fns.find(fn_dir.GetFullPath()) != std::string::npos) && (fns.find("demo.pzw") != std::string::npos)) {
            PICSimLab.LoadWorkspace((const char*)fn.GetFullPath().utf8_str(), 0);
            PICSimLab.SetWorkspaceFileName("");
        } else {
            PICSimLab.LoadWorkspace((const char*)fn.GetFullPath().utf8_str());
        }

    } else if ((Application->Aargc >= 3) && (Application->Aargc <= 5)) {
        // arguments: Board Processor File.hex(.bin) file.pcf

        if (Application->Aargc >= 4) {
#ifdef wxUSE_UNICODE
            fn.Assign(Application->Aargvw[3]);
#else
            fn.Assign(Application->Aargv[3]);
#endif
            fn.MakeAbsolute();
        }
        if (Application->Aargc == 5) {
#ifdef wxUSE_UNICODE
            fn_spare.Assign(Application->Aargvw[4]);
#else
            fn_spare.Assign(Application->Aargv[4]);
#endif
            fn_spare.MakeAbsolute();
        }

        PICSimLab.SetLabs(-1, PICSimLab.GetLab_());
        for (int i = 0; i < BOARDS_LAST; i++) {
            if (!strcmp(boards_list[i].name_, Application->Aargv[1])) {
                PICSimLab.SetLabs(i, PICSimLab.GetLab_());
                break;
            }
        }

        if (PICSimLab.GetLab() != -1) {
            if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
                snprintf(fname, 1100, "%s/picsimlab_%i.ini", home, PICSimLab.GetInstanceNumber());
            } else {
                snprintf(fname, 1100, "%s/picsimlab.ini", home);
            }
            PICSimLab.PrefsClear();
            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fname)) {
                if (PICSimLab.PrefsLoadFromFile(fname)) {
                    PICSimLab.SavePrefs("picsimlab_lab", boards_list[PICSimLab.GetLab()].name_);
                    PICSimLab.SavePrefs(std::string(boards_list[PICSimLab.GetLab()].name_) + "_proc",
                                        Application->Aargv[2]);
                    if (Application->Aargc == 5) {
                        PICSimLab.SavePrefs("spare_on", "1");
                    }
                    PICSimLab.PrefsSaveToFile(fname);
                }
            }
        } else {
            Application->Aargc = 1;
            printf("PICSimLab: Unknown board %s !\n", Application->Aargv[1]);
        }

        // search for file name
        if (Application->Aargc >= 4) {
            // load options
            PICSimLab.Configure(home, 0, 1, (const char*)fn.GetFullPath().utf8_str());
            if (Application->Aargc == 5) {
                SpareParts.LoadConfig((const char*)fn_spare.GetFullPath().utf8_str());
            }
        } else {
            // load options
            PICSimLab.Configure(home, 0, 1);
        }
    } else if (close_error) {  // no arguments with error
        printf(
            "PICSimLab: Error closing PICSimLab in last time! \nUsing default mode.\n Erro log file: %s\n If the "
            "problem persists, please consider opening an issue on github..",
            fname_error);

        FILE* ferror;
        ferror = fopen(fname_error, "a");

        if (ferror) {
            char btdir[256];
            PICSimLab.SystemCmd(PSC_GETTEMPDIR, "PICSimLab", btdir);

            fprintf(ferror,
                    "\n\nPICSimLab: Error closing PICSimLab in last time! \nUsing default mode.\n Erro log file: %s\n "
                    "If the problem persists, please consider opening an issue on github :\n"
                    "https://github.com/lcgamboa/picsimlab/issues\n\n"
                    "A backup was made of your project that presented a problem with the name following the template:\n"
                    "%s/backup_<boardname>_<timestamp>.pzw\n\n",
                    fname_error, btdir);
            fclose(ferror);
        }

        lxLaunchDefaultApplication(fname_error);

        // force use demo
        PICSimLab.Configure(home, 2, 1);

        PICSimLab.RegisterError("Error closing PICSimLab in last time!\n Using default mode.\n Error log file: " +
                                std::string(fname_error) +
                                "\n If the problem persists, please consider opening an issue on github.\n ");
    } else {  // no arguments
        // load options
        PICSimLab.Configure(home, 0, 1);
    }
    label1.SetText(PICSimLab.GetBoard()->GetClkLabel());
}

void CPWindow1::OnConfigure(void) {
    Window1.Configure();
}

void CPWindow1::OnClockSet(const float clk, const int update) {
    if (update) {
        if (clk < 1) {
            Window1.combo1.SetText(FloatStrFormat("%2.1f", clk));
        } else {
            Window1.combo1.SetText(FloatStrFormat("%2.0f", clk));
        }
        Window1.SetNeedClkUpdate(0);
    } else {
        Window1.SetNeedClkUpdate(1);
    }
}

void CPWindow1::OnReadPreferences(const char* name, const char* value, const int create) {
    if (!strcmp(name, "picsimlab_debug")) {
#ifndef NO_DEBUG
        Window1.togglebutton1.SetCheck(PICSimLab.GetDebugStatus());
#endif
    }

    if (!strcmp(name, "picsimlab_position")) {
        int i, j;
        sscanf(value, "%i,%i", &i, &j);
        Window1.SetX(i);
        Window1.SetY(j);
        printf("PICSimLab: Window position x=%i y=%i\n", i, j);
    }

    if (!strcmp(name, "picsimlab_scale")) {
        if (create) {
            Window1.draw1.SetWidth(PICSimLab.plWidth * PICSimLab.GetScale());
            Window1.SetWidth(185 + PICSimLab.plWidth * PICSimLab.GetScale());
            Window1.draw1.SetHeight(PICSimLab.plHeight * PICSimLab.GetScale());
            Window1.SetHeight(90 + PICSimLab.plHeight * PICSimLab.GetScale());
        }
    }

    if (!strcmp(name, "picsimlab_lfile")) {
        if (PICSimLab.GetFNAME().length() > 1)
            Window1.menu1_File_ReloadLast.SetEnable(1);
        else
            Window1.menu1_File_ReloadLast.SetEnable(1);
    }
}

void CPWindow1::OnSavePrefs(void) {
    PICSimLab.SavePrefs("picsimlab_position", std::to_string(Window1.GetX()) + "," + std::to_string(Window1.GetY()));
}

void CPWindow1::OnLoadHexFile(const std::string fname) {
    if (PICSimLab.GetMcuRun())
        Window1.SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                              ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                              : ("PICSimLab - ")) +
                         std::string(boards_list[PICSimLab.GetLab()].name) + " - " +
                         PICSimLab.GetBoard()->GetProcessorName() + " - " + ((const char*)basename(fname).c_str()));
    else
        Window1.SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                              ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                              : ("PICSimLab - ")) +
                         std::string(boards_list[PICSimLab.GetLab()].name) + " - " +
                         PICSimLab.GetBoard()->GetProcessorName());
}

void CPWindow1::OnOpenLoadHexFileDialog(void) {
    Window1.menu1_File_LoadHex_EvMenuActive(NULL);
}

void CPWindow1::OnEndSimulation(void) {
    Window1.timer1.SetRunState(0);
    Window1.timer2.SetRunState(0);

    msleep(BASETIMER);
    while (PICSimLab.status) {
        msleep(1);
        Application->ProcessEvents();
    }
    PICSimLab.tgo = 100000;
#ifndef _NOTHREAD
    {
        std::unique_lock<std::mutex> lk(Window1.cpu_mutex);
        Window1.cpu_cond.notify_one();
    }
#endif

    Window1.thread1.Destroy();
    PICSimLab.tgo = 0;

#ifndef __EMSCRIPTEN__
    Window1.thread2.Destroy();
#endif

    // refresh window position to window reopen in same position
    Window1.GetX();
    Window1.GetY();
}

void CPWindow1::Configure(void) {
    menu1_Microcontroller.DestroyChilds();
    std::string sdev = PICSimLab.GetBoard()->GetSupportedDevices();
    int f;
    int dc = 0;
    while (sdev.size() > 0) {
        f = sdev.find(",");
        if (f < 0)
            break;
        MMicro[dc].SetFOwner(this);
        MMicro[dc].SetName("Micro_" + std::to_string(dc + 1));
        MMicro[dc].SetText(sdev.substr(0, f));
        MMicro[dc].EvMenuActive = EVMENUACTIVE & CPWindow1::menu1_EvMicrocontroller;
        menu1_Microcontroller.CreateChild(&MMicro[dc]);
        MMicro[dc].SetVisible(true);
        sdev = sdev.substr(f + 1, sdev.size() - f - 1);
        dc++;

        if (dc >= MAX_MIC) {
            printf("PICSimLab: microcontroller menu only support %i entries!\n", MAX_MIC);
            exit(-1);
        }
    }

    filedialog1.SetDir(PICSimLab.GetPath());

    draw1.SetVisible(0);
    draw1.SetImgFileName(
        GetLocalFile(PICSimLab.GetSharePath() + "boards/" + PICSimLab.GetBoard()->GetPictureFileName()),
        PICSimLab.GetScale(), PICSimLab.GetScale());

#ifndef NO_TOOLS
    if ((!PICSimLab.GetBoard()->GetProcessorName().compare("atmega328p")) ||
        (!PICSimLab.GetBoard()->GetProcessorName().compare("atmega2560"))) {
        menu1_Tools_ArduinoBootloader.SetEnable(true);
    } else {
        menu1_Tools_ArduinoBootloader.SetEnable(false);
    }
#endif

    PICSimLab.GetBoard()->Reset();
    PICSimLab.GetBoard()->EvOnShow();
    PICSimLab.GetBoard()->Draw();
    draw1.SetVisible(1);

    SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                  ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                  : ("PICSimLab - ")) +
             std::string(boards_list[PICSimLab.GetLab()].name) + " - " + PICSimLab.GetBoard()->GetProcessorName());

    thread1.Run();  // parallel thread
#ifndef __EMSCRIPTEN__
    // FIXME remote control disabled
    thread2.Run();  // parallel thread
#endif
    timer1.SetRunState(1);
    timer2.SetRunState(1);
}

// Change  frequency

void CPWindow1::combo1_EvOnComboChange(CControl* control) {
    PICSimLab.SetNSTEP((int)(atof(combo1.GetText()) * NSTEPKT));

    if (PICSimLab.GetJUMPSTEPS()) {
        PICSimLab.SetNSTEPJ(PICSimLab.GetNSTEP() / PICSimLab.GetJUMPSTEPS());
    } else {
        PICSimLab.SetNSTEPJ(PICSimLab.GetNSTEP());
    }

    PICSimLab.GetBoard()->MSetFreq(PICSimLab.GetNSTEP() * NSTEPKF);
    Oscilloscope.SetBaseTimer();

    Application->ProcessEvents();

    PICSimLab.tgo = 1;
    timer1.SetTime(BASETIMER);
}

void CPWindow1::_EvOnDestroy(CControl* control) {
    rcontrol_server_end();
    PICSimLab.GetBoard()->EndServers();
    PICSimLab.SetNeedReboot(0);
    PICSimLab.EndSimulation();

    if (strlen(PICSimLab.GetPzwTmpdir())) {
        lxRemoveDir(PICSimLab.GetPzwTmpdir());
    }

#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
    fflush(stdout);
    freopen(NULLFILE, "w", stdout);
    fflush(stderr);
    freopen(NULLFILE, "w", stderr);
    char fname[1200];
    snprintf(fname, 1199, "%s/picsimlab_log%i.txt", (const char*)PICSimLab.GetHomePath().c_str(),
             PICSimLab.GetInstanceNumber());

    // redirect
    char tmpname[1200];
    snprintf(tmpname, 1200, "%s/picsimlab_log%i-XXXXXX", (const char*)lxGetTempDir("PICSimLab").c_str(),
             PICSimLab.GetInstanceNumber());
    close(mkstemp(tmpname));
    unlink(tmpname);
    strncat(tmpname, ".txt", 1199);
    lxRenameFile(fname, tmpname);

    FILE* flog;
    FILE* ftmp;
    flog = fopen_UTF8(fname, "w");
    if (flog) {
        // copy
        ftmp = fopen_UTF8(tmpname, "r");
        if (ftmp) {
            char buff[4096];
            int nr;
            while ((nr = fread(buff, 1, 4096, ftmp)) > 0) {
                fwrite(buff, nr, 1, flog);
            }
            fclose(ftmp);
            fprintf(flog, "\nPICSimLab: Finish Ok\n");
        }
        fclose(flog);
        unlink(tmpname);
    }
#else
    printf("PICSimLab: Finish Ok\n");
    fflush(stdout);
#endif
}

void CPWindow1::menu1_File_LoadHex_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    filedialog1.Run();
#endif
}

void CPWindow1::menu1_File_SaveHex_EvMenuActive(CControl* control) {
    pa = PICSimLab.GetMcuPwr();
    filedialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
#ifdef __EMSCRIPTEN__
    filedialog1.SetDir("/tmp/");
    filedialog1.SetFileName("untitled.hex");
    filedialog1_EvOnClose(1);
#else
    filedialog1.Run();
#endif
}

void CPWindow1::filedialog1_EvOnClose(int retId) {
    pa = PICSimLab.GetMcuPwr();
    PICSimLab.SetMcuPwr(0);

    while (PICSimLab.status & ST_TH)
        usleep(100);  // wait thread

    if (retId && (filedialog1.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
        PICSimLab.SetPath((const char*)filedialog1.GetDir().utf8_str());
        PICSimLab.SetFNAME((const char*)filedialog1.GetFileName().utf8_str());
        PICSimLab.LoadHexFile((const char*)filedialog1.GetFileName().utf8_str());
        menu1_File_ReloadLast.SetEnable(1);
    }

    if (retId && (filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
        if (PICSimLab.GetBoard()->MDumpMemory((const char*)filedialog1.GetFileName().utf8_str())) {
            PICSimLab.RegisterError(std::string("Error saving file: ") +
                                    (const char*)filedialog1.GetFileName().utf8_str());
            printf("PICSimLab: Error saving file: %s\n", (const char*)filedialog1.GetFileName().utf8_str());
        }
#ifdef __EMSCRIPTEN__
        EM_ASM_(
            {
                var filename = UTF8ToString($0);
                var buf = FS.readFile(filename);
                var blob = new Blob([buf], { "type" : "application/octet-stream" });
                var text = URL.createObjectURL(blob);

                var element = document.createElement('a');
                element.setAttribute('href', text);
                element.setAttribute('download', filename);

                element.style.display = 'none';
                document.body.appendChild(element);

                element.click();

                document.body.removeChild(element);
                URL.revokeObjectURL(text);
            },
            filedialog1.GetFileName().c_str());
#endif
    }

    PICSimLab.SetMcuPwr(pa);
}

void CPWindow1::menu1_File_Exit_EvMenuActive(CControl* control) {
    WDestroy();
}

void CPWindow1::menu1_Help_Contents_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER
    // lxLaunchDefaultBrowser("file://"+PICSimLab.GetSharePath() + "docs/picsimlab.html");
    char stemp[256];
    snprintf(stemp, 255, "https://lcgamboa.github.io/picsimlab_docs/%s/index.html", _VERSION_);
    lxLaunchDefaultBrowser(stemp);
#else
    Window2.html1.SetLoadFile(PICSimLab.GetSharePath() + "docs/picsimlab.html");
    Window2.Show();
#endif
}

void CPWindow1::menu1_Help_Board_EvMenuActive(CControl* control) {
    char bname[30];
    strncpy(bname, boards_list[PICSimLab.GetLab()].name_, 29);

    char* ptr;
    // remove _ from names
    while ((ptr = strchr(bname, '_'))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }

    char stemp[256];
    snprintf(stemp, 255, "https://lcgamboa.github.io/picsimlab_docs/%s/%s.html", _VERSION_, bname);
    lxLaunchDefaultBrowser(stemp);
}

void CPWindow1::menu1_Help_About_Board_EvMenuActive(CControl* control) {
    Message_sz("Board " + lxString(boards_list[PICSimLab.GetLab()].name) + "\nDeveloped by " +
                   lxString::FromUTF8(PICSimLab.GetBoard()->GetAboutInfo().c_str()),
               400, 200);
}

void CPWindow1::menu1_Help_About_PICSimLab_EvMenuActive(CControl* control) {
    char stemp[256];
    snprintf(stemp, 255, "Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s %s", _VERSION_, _DATE_,
             _ARCH_, _PKG_);
    Message_sz(stemp, 400, 200);
}

void CPWindow1::menu1_Help_Examples_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER_EXAMPLES
    // lxLaunchDefaultBrowser("file://"+PICSimLab.GetSharePath() + "docs/picsimlab.html");
    lxLaunchDefaultBrowser("https://lcgamboa.github.io/picsimlab_examples/board_" +
                           std::string(boards_list[PICSimLab.GetLab()].name_) + ".html#board_" +
                           std::string(boards_list[PICSimLab.GetLab()].name_) + "_" +
                           PICSimLab.GetBoard()->GetProcessorName());
    // SetToDestroy();
#else
    OldPath = filedialog2.GetDir();

    filedialog2.SetDir(PICSimLab.GetSharePath() + "/docs/hex/board_" + std::string(boards_list[lab].name_) + "/" +
                       PICSimLab.GetBoard()->GetProcessorName() + "/");

    menu1_File_LoadWorkspace_EvMenuActive(control);
#endif
}

// Resize

void CPWindow1::_EvOnShow(CControl* control) {
    PICSimLab.SetNeedResize(1);
    if (!PICSimLab.GetSimulationRun()) {
        DrawBoard();
    }
}

void CPWindow1::_EvOnDropFile(CControl* control, const std::string fname) {
    printf("PICSimLab: File droped: %s\n", (const char*)fname.c_str());
    file_ready(basename(fname), dirname(fname));
}

void CPWindow1::menu1_File_Configure_EvMenuActive(CControl* control) {
    Window3.ShowExclusive();
}

void CPWindow1::menu1_File_ReloadLast_EvMenuActive(CControl* control) {
    PICSimLab.LoadHexFile(PICSimLab.GetFNAME());
}

void CPWindow1::board_Event(CControl* control) {
    PICSimLab.GetBoard()->board_Event(control->GetName().c_str());
}

void CPWindow1::board_ButtonEvent(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                  unsigned int state) {
    PICSimLab.GetBoard()->board_ButtonEvent(control->GetName().c_str(), button, x, y, state);
}

void CPWindow1::menu1_Modules_Oscilloscope_EvMenuActive(CControl* control) {
    PICSimLab.GetBoard()->SetUseOscilloscope(1);
    Window4.Show();
}

void CPWindow1::menu1_Modules_Spareparts_EvMenuActive(CControl* control) {
    PICSimLab.GetBoard()->SetUseSpareParts(1);
    Window5.Show();
    Window5.timer1.SetRunState(1);
    PICSimLab.GetBoard()->Reset();
}

// Change board

void CPWindow1::menu1_EvBoard(CControl* control) {
    if (!PICSimLab.GetErrorCount()) {
        PICSimLab.SetLabs(atoi(((CItemMenu*)control)->GetName()), PICSimLab.GetLab());
        PICSimLab.SetFNAME(" ");
        PICSimLab.EndSimulation(1);
        PICSimLab.Configure(PICSimLab.GetHomePath().c_str());
        PICSimLab.SetNeedResize(1);
        label1.SetText(PICSimLab.GetBoard()->GetClkLabel());
    }
}

// change microcontroller

void CPWindow1::menu1_EvMicrocontroller(CControl* control) {
    if (!PICSimLab.GetErrorCount()) {
        PICSimLab.SetProcessorName(PICSimLab.GetBoard()->GetProcessorName());
        PICSimLab.GetBoard()->SetProcessorName((const char*)((CItemMenu*)control)->GetText().c_str());

        SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                      ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                      : ("PICSimLab - ")) +
                 std::string(boards_list[PICSimLab.GetLab()].name) + " - " + PICSimLab.GetBoard()->GetProcessorName());

        PICSimLab.SetFNAME(" ");
        PICSimLab.EndSimulation();
        PICSimLab.Configure(PICSimLab.GetHomePath().c_str());
        PICSimLab.SetNeedResize(1);
        SpareParts.SetUseAlias(0);
    }
}

void CPWindow1::togglebutton1_EvOnToggleButton(CControl* control) {
#ifdef NO_DEBUG
    statusbar1.SetField(1, " ");
#else
    int osc_on = PICSimLab.GetBoard()->GetUseOscilloscope();
    int spare_on = PICSimLab.GetBoard()->GetUseSpareParts();

    PICSimLab.SetDebugStatus(togglebutton1.GetCheck());

    PICSimLab.EndSimulation();
    PICSimLab.Configure(PICSimLab.GetHomePath().c_str());

    if (osc_on)
        menu1_Modules_Oscilloscope_EvMenuActive(this);
    if (spare_on)
        menu1_Modules_Spareparts_EvMenuActive(this);

    PICSimLab.SetNeedResize(1);
#endif
}

void CPWindow1::menu1_File_SaveWorkspace_EvMenuActive(CControl* control) {
    filedialog2.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
#ifdef __EMSCRIPTEN__
    filedialog2.SetDir("/tmp/");
    filedialog2.SetFileName("untitled.pzw");
    filedialog2_EvOnClose(1);
#else
    filedialog2.Run();
#endif
}

void CPWindow1::OnUpdateStatus(const int field, const std::string msg) {
    Window1.statusbar1.SetField(field, msg);
}

void CPWindow1::OnConfigMenuGUI(const PICSimlabGUIMenu type) {
    switch (type) {
        case GMT_HEX:
            Window1.menu1_File_LoadHex.SetText("Load Hex");
            Window1.menu1_File_SaveHex.SetText("Save Hex");
            Window1.menu1_File_LoadHex.SetEnable(1);
            Window1.menu1_File_SaveHex.SetEnable(1);
            Window1.filedialog1.SetFileName("untitled.hex");
            Window1.filedialog1.SetFilter("Hex Files (*.hex)|*.hex;*.HEX");
            break;
        case GMT_BIN:
            Window1.menu1_File_LoadHex.SetText("Load Bin");
            Window1.menu1_File_SaveHex.SetText("Save Bin");
            Window1.menu1_File_LoadHex.SetEnable(1);
            Window1.menu1_File_SaveHex.SetEnable(0);
            Window1.filedialog1.SetFileName("untitled.bin");
            Window1.filedialog1.SetFilter("Bin Files (*.bin)|*.bin;*.BIN");
            break;
        case GMT_DISABLED:
            Window1.menu1_File_LoadHex.SetEnable(0);
            Window1.menu1_File_SaveHex.SetEnable(0);
            Window1.menu1_File_ReloadLast.SetEnable(0);
            break;
        default:
            break;
    }
}

int CPWindow1::OnCanvasCmd(const CanvasCmd_t cmd) {
    switch (cmd.cmd) {
        case CC_INIT:
            Window1.draw1.Canvas.Init(cmd.Init.sx, cmd.Init.sy, cmd.Init.angle);
            return 0;
            break;
        case CC_CHANGESCALE:
            Window1.draw1.Canvas.ChangeScale(cmd.ChangeScale.sx, cmd.ChangeScale.sy);
            return 0;
            break;
        case CC_END:
            Window1.draw1.Canvas.End();
            return 0;
            break;
        case CC_SETBITMAP:
            Window1.draw1.Canvas.SetBitmap(Window1.Bitmaps[cmd.SetBitmap.BitmapId], cmd.SetBitmap.xs, cmd.SetBitmap.ys);
            return 0;
            break;
        case CC_SETCOLOR:
            Window1.draw1.Canvas.SetColor(cmd.SetColor.r, cmd.SetColor.g, cmd.SetColor.b);
            return 0;
            break;
        case CC_SETFGCOLOR:
            Window1.draw1.Canvas.SetFgColor(cmd.SetFgColor.r, cmd.SetFgColor.g, cmd.SetFgColor.b);
            return 0;
            break;
        case CC_SETBGCOLOR:
            Window1.draw1.Canvas.SetBgColor(cmd.SetBgColor.r, cmd.SetBgColor.g, cmd.SetBgColor.b);
            return 0;
            break;
        case CC_SETFONTSIZE:
            Window1.draw1.Canvas.SetFontSize(cmd.SetFontSize.pointsize);
            return 0;
            break;
        case CC_SETFONTWEIGHT:
            Window1.draw1.Canvas.SetFontWeight(cmd.SetFontWeight.weight);
            return 0;
            break;
        case CC_SETLINEWIDTH:
            Window1.draw1.Canvas.SetLineWidth(cmd.SetLineWidth.lwidth);
            return 0;
            break;
        case CC_POINT:
            Window1.draw1.Canvas.Point(cmd.Point.x, cmd.Point.y);
            return 0;
            break;
        case CC_LINE:
            Window1.draw1.Canvas.Line(cmd.Line.x1, cmd.Line.y1, cmd.Line.x2, cmd.Line.y2);
            return 0;
            break;
        case CC_RECTANGLE:
            Window1.draw1.Canvas.Rectangle(cmd.Rectangle.filled, cmd.Rectangle.x, cmd.Rectangle.y, cmd.Rectangle.width,
                                           cmd.Rectangle.height);

            return 0;
            break;
        case CC_CIRCLE:
            Window1.draw1.Canvas.Circle(cmd.Circle.filled, cmd.Circle.x, cmd.Circle.y, cmd.Circle.radius);
            return 0;
            break;
        case CC_ROTATEDTEXT:
            Window1.draw1.Canvas.RotatedText(cmd.RotatedText.str, cmd.RotatedText.x, cmd.RotatedText.y,
                                             cmd.RotatedText.angle);
            return 0;
            break;
        case CC_TEXTONRECT: {
            lxRect rect;
            rect.x = cmd.TextOnRect.rect.x;
            rect.y = cmd.TextOnRect.rect.y;
            rect.width = cmd.TextOnRect.rect.width;
            rect.height = cmd.TextOnRect.rect.height;
            Window1.draw1.Canvas.TextOnRect(cmd.TextOnRect.str, rect, cmd.TextOnRect.align);
            return 0;
        } break;
        case CC_POLYGON:
            Window1.draw1.Canvas.Polygon(cmd.Polygon.filled, (lxPoint*)cmd.Polygon.points, cmd.Polygon.npoints);
            return 0;
            break;
        case CC_PUTBITMAP:
            Window1.draw1.Canvas.PutBitmap(Window1.Bitmaps[cmd.PutBitmap.BitmapId], cmd.PutBitmap.x, cmd.PutBitmap.y);
            return 0;
            break;
        case CC_GETBGCOLOR: {
            lxColor bgc = Window1.draw1.Canvas.GetBgColor();
            *cmd.GetBgColor.r = bgc.Red();
            *cmd.GetBgColor.g = bgc.Green();
            *cmd.GetBgColor.b = bgc.Blue();
            return 0;
        } break;
        case CC_CREATE:
            Window1.draw1.Canvas.Create(Window1.GetWWidget(), Window1.Bitmaps[cmd.Create.BitmapId]);
            return 0;
            break;
        case CC_DESTROY:
            Window1.draw1.Canvas.Destroy();
            return 0;
            break;
        case CC_FREEBITMAP:
            if (Window1.Bitmaps[cmd.FreeBitmap.BitmapId]) {
                delete Window1.Bitmaps[cmd.FreeBitmap.BitmapId];
                Window1.Bitmaps[cmd.FreeBitmap.BitmapId] = NULL;
                return 0;
            }
            break;
        case CC_GETBITMAPSIZE: {
            lxSize ps = Window1.Bitmaps[cmd.GetBitmapSize.BitmapId]->GetSize();
            *cmd.GetBitmapSize.w = ps.GetWidth();
            *cmd.GetBitmapSize.h = ps.GetHeight();
            return 0;
        } break;
        case CC_LOADIMAGE: {
            lxImage image(&Window1);
            if (image.LoadFile(GetLocalFile(cmd.LoadImage.fname), cmd.LoadImage.orientation, cmd.LoadImage.scale,
                               cmd.LoadImage.scale, cmd.LoadImage.usealpha)) {
                // find enpty bitmap
                int bid = -1;
                for (int i = 0; i < BITMAPS_MAX; i++) {
                    if (Window1.Bitmaps[i] == NULL) {
                        bid = i;
                        break;
                    }
                }

                if ((bid >= 0) && (bid < BITMAPS_MAX)) {
                    Window1.Bitmaps[bid] = new lxBitmap(&image, &Window1);
                    image.Destroy();
                    return bid;
                }
            }
            printf("PICSimLab: Erro CC_LOADIMAGE!\n");
            return -1;
        } break;
        case CC_CREATEIMAGE: {
            lxImage image(&Window1);
            if (image.CreateBlank(cmd.CreateImage.width, cmd.CreateImage.height, cmd.CreateImage.orientation,
                                  cmd.CreateImage.scale, cmd.CreateImage.scale)) {
                // find enpty bitmap
                int bid = -1;
                for (int i = 0; i < BITMAPS_MAX; i++) {
                    if (Window1.Bitmaps[i] == NULL) {
                        bid = i;
                        break;
                    }
                }
                if ((bid >= 0) && (bid < BITMAPS_MAX)) {
                    Window1.Bitmaps[bid] = new lxBitmap(&image, &Window1);
                    image.Destroy();
                    return bid;
                }
            }
            printf("PICSimLab: Erro CC_CREATEIMAGE!\n");
            return -1;
        } break;
        case CC_ARC:
            Window1.draw1.Canvas.Arc(cmd.Arc.filled, cmd.Arc.x1, cmd.Arc.y1, cmd.Arc.x2, cmd.Arc.y2, cmd.Arc.xc,
                                     cmd.Arc.yc);
            break;
        case CC_ELLIPTICARC:
            Window1.draw1.Canvas.EllipticArc(cmd.EllipticArc.filled, cmd.EllipticArc.x, cmd.EllipticArc.y,
                                             cmd.EllipticArc.width, cmd.EllipticArc.height, cmd.EllipticArc.start,
                                             cmd.EllipticArc.end);
            break;
        case CC_LINES:
            Window1.draw1.Canvas.Lines((lxPoint*)cmd.Lines.points, cmd.Lines.npoints);
            break;
        case CC_LAST:
        default:
            break;
    }
    return -1;
}

void* CPWindow1::OnUpdateGUI(const int id, const PICSimlabGUIType type, const PICSimlabGUIAction action,
                             const void* arg) {
    char name[128];
    switch (type) {
        case GT_GAUGE:
            switch (action) {
                case GA_ADD: {
                    const char* text = (const char*)arg;

                    if ((text[0] == '*')) {  // Inline version

                        // gauge
                        snprintf(name, 128, "b_gauge%d", id);
                        CGauge* pgauge = new CGauge();
                        pgauge->SetFOwner(&Window1);
                        pgauge->SetName(name);
                        pgauge->SetX(35);
                        pgauge->SetY(74 + (id * 25));
                        pgauge->SetWidth(110);
                        pgauge->SetHeight(20);
                        pgauge->SetEnable(1);
                        pgauge->SetVisible(1);
                        pgauge->SetRange(100);
                        pgauge->SetValue(0);
                        pgauge->SetType(4);
                        Window1.CreateChild(pgauge);
                        // label1
                        snprintf(name, 128, "b_label%d", id);
                        CLabel* plabel = new CLabel();
                        plabel->SetFOwner(&Window1);
                        plabel->SetName(name);
                        plabel->SetX(6);
                        plabel->SetY(75 + (id * 25));
                        plabel->SetWidth(50);
                        plabel->SetHeight(20);
                        plabel->SetEnable(1);
                        plabel->SetVisible(1);
                        plabel->SetText((text + 1));
                        plabel->SetAlign(1);
                        Window1.CreateChild(plabel);
                    } else {
                        // gauge
                        snprintf(name, 128, "b_gauge%d", id);
                        CGauge* bgauge = new CGauge();
                        bgauge->SetFOwner(&Window1);
                        bgauge->SetName(name);
                        bgauge->SetX(13);
                        bgauge->SetY(102 + (id * 50));
                        bgauge->SetWidth(140);
                        bgauge->SetHeight(20);
                        bgauge->SetEnable(1);
                        bgauge->SetVisible(1);
                        bgauge->SetRange(100);
                        bgauge->SetValue(0);
                        bgauge->SetType(4);
                        Window1.CreateChild(bgauge);
                        // label
                        snprintf(name, 128, "b_label%d", id);
                        CLabel* blabel = new CLabel();
                        blabel->SetFOwner(&Window1);
                        blabel->SetName(name);
                        blabel->SetX(12);
                        blabel->SetY(80 + (id * 50));
                        blabel->SetWidth(140);
                        blabel->SetHeight(20);
                        blabel->SetEnable(1);
                        blabel->SetVisible(1);
                        blabel->SetText(text);
                        blabel->SetAlign(1);
                        Window1.CreateChild(blabel);
                    }
                } break;
                case GA_DEL: {
                    // gauge
                    snprintf(name, 128, "b_gauge%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                } break;
                case GA_SET: {
                    // gauge
                    snprintf(name, 128, "b_gauge%d", id);
                    CGauge* bgauge = (CGauge*)Window1.GetChildByName(name);
                    bgauge->SetValue(*((int*)arg));
                } break;
                default:
                    break;
            }
            break;
        case GT_SCROLL:
            switch (action) {
                case GA_ADD: {
                    // scroll
                    snprintf(name, 128, "b_scroll%d", id);
                    CScroll* bscroll = new CScroll();
                    bscroll->SetFOwner(&Window1);
                    bscroll->SetName(name);
                    bscroll->SetX(13);
                    bscroll->SetY(102 + (id * 50));
                    bscroll->SetWidth(140);
                    bscroll->SetHeight(20);
                    bscroll->SetEnable(1);
                    bscroll->SetVisible(1);
                    bscroll->SetRange(200);
                    bscroll->SetPosition(0);
                    bscroll->SetType(4);
                    Window1.CreateChild(bscroll);
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = new CLabel();
                    blabel->SetFOwner(&Window1);
                    blabel->SetName(name);
                    blabel->SetX(12);
                    blabel->SetY(80 + (id * 50));
                    blabel->SetWidth(140);
                    blabel->SetHeight(20);
                    blabel->SetEnable(1);
                    blabel->SetVisible(1);
                    blabel->SetText((const char*)arg);
                    blabel->SetAlign(1);
                    Window1.CreateChild(blabel);
                } break;
                case GA_DEL: {
                    // scroll
                    snprintf(name, 128, "b_scroll%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                } break;
                case GA_SET: {
                    // scroll
                    snprintf(name, 128, "b_scroll%d", id);
                    CScroll* bscroll = (CScroll*)Window1.GetChildByName(name);
                    bscroll->SetPosition(*((int*)arg));
                } break;
                case GA_GET: {
                    // scroll
                    snprintf(name, 128, "b_scroll%d", id);
                    CScroll* bscroll = (CScroll*)Window1.GetChildByName(name);
                    int* val = (int*)arg;
                    *val = bscroll->GetPosition();
                    return val;
                } break;
                case GA_SET_LABEL: {
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = (CLabel*)Window1.GetChildByName(name);
                    blabel->SetText(((const char*)arg));
                } break;
                default:
                    break;
            }
            break;
        case GT_LABEL:
            switch (action) {
                case GA_ADD: {
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = new CLabel();
                    blabel->SetFOwner(&Window1);
                    blabel->SetName(name);
                    blabel->SetX(12);
                    blabel->SetY(80 + (id * 50));
                    blabel->SetWidth(140);
                    blabel->SetHeight(20);
                    blabel->SetEnable(1);
                    blabel->SetVisible(1);
                    blabel->SetText((const char*)arg);
                    blabel->SetAlign(1);
                    Window1.CreateChild(blabel);
                } break;
                case GA_DEL:
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                    break;
                case GA_SET_LABEL: {
                    // gauge
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = (CLabel*)Window1.GetChildByName(name);
                    blabel->SetText((const char*)arg);
                } break;
                default:
                    break;
            }
            break;
        case GT_BUTTON:
            switch (action) {
                case GA_ADD: {
                    // button
                    snprintf(name, 128, "b_button%d", id);
                    CButton* bbutton = new CButton();
                    bbutton->SetFOwner(&Window1);
                    bbutton->SetName(name);
                    bbutton->SetX(13);
                    bbutton->SetY(80 + (id * 50));
                    bbutton->SetWidth(130);
                    bbutton->SetHeight(24);
                    bbutton->SetEnable(1);
                    bbutton->SetVisible(1);
                    bbutton->SetText((const char*)arg);
                    bbutton->SetTag(4);
                    bbutton->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow1::board_ButtonEvent;
                    Window1.CreateChild(bbutton);
                } break;
                case GA_DEL:
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                    break;
                case GA_SET_LABEL: {
                    // gauge
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = (CLabel*)Window1.GetChildByName(name);
                    blabel->SetText((const char*)arg);
                } break;
                default:
                    break;
            }
            break;
        case GT_COMBO:
            switch (action) {
                case GA_ADD: {
                    // combo
                    snprintf(name, 128, "b_combo%d", id);
                    CCombo* bcombo = new CCombo();
                    bcombo->SetFOwner(&Window1);
                    bcombo->SetName(name);
                    bcombo->SetX(13);
                    bcombo->SetY(102 + (id * 50));
                    bcombo->SetWidth(140);
                    bcombo->SetHeight(24);
                    bcombo->SetEnable(1);
                    bcombo->SetVisible(1);
                    bcombo->SetText("set");
                    bcombo->SetTag(id);
                    bcombo->EvOnComboChange = EVONCOMBOCHANGE & CPWindow1::board_Event;
                    Window1.CreateChild(bcombo);
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    CLabel* blabel = new CLabel();
                    blabel->SetFOwner(&Window1);
                    blabel->SetName(name);
                    blabel->SetX(12);
                    blabel->SetY(80 + (id * 50));
                    blabel->SetWidth(140);
                    blabel->SetHeight(20);
                    blabel->SetEnable(1);
                    blabel->SetVisible(1);
                    blabel->SetText((const char*)arg);
                    blabel->SetAlign(1);
                    Window1.CreateChild(blabel);
                } break;
                case GA_DEL: {
                    // combo
                    snprintf(name, 128, "b_combo%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                    // label
                    snprintf(name, 128, "b_label%d", id);
                    Window1.DestroyChild(Window1.GetChildByName(name));
                } break;
                case GA_SET: {
                    // gauge
                    snprintf(name, 128, "b_combo%d", id);
                    CCombo* bcombo = (CCombo*)Window1.GetChildByName(name);
                    const char* value = ((const char*)arg);
                    if (value[0] == ',') {
                        bcombo->SetItems((value + 1));
                    } else {
                        bcombo->SetText(value);
                    }
                } break;
                case GA_GET: {
                    // gauge
                    snprintf(name, 128, "b_combo%d", id);
                    CCombo* bcombo = (CCombo*)Window1.GetChildByName(name);
                    char* value = ((char*)arg);
                    strncpy(value, (const char*)bcombo->GetText().c_str(), 128);
                    return value;
                } break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return NULL;
}

void CPWindow1::menu1_File_LoadWorkspace_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog2.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    filedialog2.Run();
#endif
}

void CPWindow1::menu1_File_LoadBoardDemo_EvMenuActive(CControl* control) {
    std::string fdemo =
        PICSimLab.GetSharePath() + "boards/" + std::string(boards_list[PICSimLab.GetLab()].name) + "/demo.pzw";

    if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fdemo.c_str())) {
        PICSimLab.LoadWorkspace(fdemo);
        PICSimLab.SetWorkspaceFileName("");
    } else {
        PICSimLab.RegisterError("Demo for board " + std::string(boards_list[PICSimLab.GetLab()].name) + " not found!");
        printf("PICSimLab: Demo for board  %s not found! (%s)\n", boards_list[PICSimLab.GetLab()].name,
               (const char*)fdemo.c_str());
    }
}

void CPWindow1::filedialog2_EvOnClose(int retId) {
    if (retId && (filedialog2.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
        PICSimLab.LoadWorkspace((const char*)filedialog2.GetFileName().utf8_str());
        if (PICSimLab.GetOldPath().size() > 1) {
            filedialog2.SetDir(PICSimLab.GetOldPath());
            PICSimLab.SetOldPath("");
        }
    }

    if (retId && (filedialog2.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
        PICSimLab.SaveWorkspace((const char*)filedialog2.GetFileName().utf8_str());
    }
}

void CPWindow1::menu1_Tools_SerialTerm_EvMenuActive(CControl* control) {
    char stfname[1024];
    snprintf(stfname, 1024, "%s/open_w_cutecom_or_gtkterm.sterm", (const char*)lxGetTempDir("PICSimLab").c_str());

    if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, stfname)) {
        // create one dumb file to associate whit serial terminal
        FILE* fout;
        fout = fopen_UTF8(stfname, "w");
        if (fout) {
            int buff = 0x11223344;  // fake magic number
            fwrite(&buff, 4, 1, fout);
            fprintf(fout,
                    "To associate .sterm extension, open this file with one serial terminal (cutecom, gtkterm, ...)");
        }
        fclose(fout);
    }
    lxLaunchDefaultApplication(stfname);
}

void CPWindow1::menu1_Tools_SerialRemoteTank_EvMenuActive(CControl* control) {
#ifdef _WIN_
    lxExecute(PICSimLab.GetSharePath() + "/../srtank.exe");
#else

    lxExecute(dirname(lxGetExecutablePath()) + "/srtank");
#endif
}

void CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive(CControl* control) {
#ifdef _WIN_
    lxExecute(PICSimLab.GetSharePath() + "/../espmsim.exe");
#else
    lxExecute(dirname(lxGetExecutablePath()) + "/espmsim");
#endif
}

void CPWindow1::menu1_Tools_ArduinoBootloader_EvMenuActive(CControl* control) {
    PICSimLab.LoadHexFile(PICSimLab.GetSharePath() + "bootloaders/arduino_" + PICSimLab.GetBoard()->GetProcessorName() +
                          ".hex");
}

void CPWindow1::menu1_Tools_MPLABXDebuggerPlugin_EvMenuActive(CControl* control) {
    lxLaunchDefaultBrowser("https://github.com/lcgamboa/picsimlab_md/releases");
}

void CPWindow1::menu1_Tools_PinViewer_EvMenuActive(CControl* control) {
#ifdef _WIN_
    lxExecute(PICSimLab.GetSharePath() + "/../PinViewer.exe " + std::to_string(PICSimLab.GetRemotecPort()));
#else
    lxExecute(dirname(lxGetExecutablePath()) + "/PinViewer " + std::to_string(PICSimLab.GetRemotecPort()));
#endif
}

// emscripten interface

extern "C" {

void file_ready(const char* fname, const char* dir) {
    const char tmp[] = "/tmp";

    if (!dir) {
        dir = tmp;
    }

    if (strstr(fname, ".pzw")) {
        printf("PICSimLab: Loading .pzw...\n");
        Window1.filedialog2.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
        Window1.filedialog2.SetDir(dir);
        Window1.filedialog2.SetFileName(fname);
        Window1.filedialog2_EvOnClose(1);
    } else if (strstr(fname, ".hex")) {
        printf("PICSimLab: Loading .hex...\n");
        Window1.filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
        Window1.filedialog1.SetDir(dir);
        Window1.filedialog1.SetFileName(fname);
        Window1.filedialog1_EvOnClose(1);
    } else if (strstr(fname, ".pcf")) {
        char buff[1024];
        strncpy(buff, dir, 1023);
        strncat(buff, fname, 1023);
        printf("PICSimLab: Loading .pcf...\n");
        SpareParts.LoadConfig(buff);
        Window1.menu1_Modules_Spareparts_EvMenuActive(&Window1);
    } else if (strstr(fname, ".ppa")) {
        char buff[1024];
        strncpy(buff, dir, 1023);
        strncat(buff, fname, 1023);
        printf("PICSimLab: Loading .ppa...\n");
        SpareParts.LoadPinAlias(buff);
    } else {
        printf("PICSimLab: Unknow file type %s !!\n", fname);
    }
}

int CPWindow1::OnSystemCmd(const PICSimLabSystemCmd cmd, const char* Arg, void* ReturnBuff) {
    switch (cmd) {
        case PSC_FILEEXISTS:
            return lxFileExists(lxString::FromUTF8(Arg));
            break;
        case PSC_GETUSERDATADIR:
            strcpy((char*)ReturnBuff, (const char*)lxGetUserDataDir(Arg).utf8_str());
            break;
        case PSC_GETEXECUTABLEPATH:
            strcpy((char*)ReturnBuff, (const char*)lxGetExecutablePath().utf8_str());
            break;
        case PSC_GETTEMPDIR:
            strcpy((char*)ReturnBuff, (const char*)lxGetTempDir(Arg).utf8_str());
            break;
        case PSC_REMOVEDIR:
            return lxRemoveDir(Arg);
            break;
        case PSC_CREATEDIR:
            return lxCreateDir(Arg);
            break;
        case PSC_REMOVEFILE:
            return lxRemoveFile(Arg);
            break;
        case PSC_EXECUTE:
            return lxExecute(Arg);
            break;
        case PSC_ZIPDIR:
            return lxZipDir(Arg, lxString::FromUTF8((const char*)ReturnBuff));
            break;
        case PSC_UNZIPDIR:
            return lxUnzipDir(lxString::FromUTF8(Arg), (char*)ReturnBuff);
            break;
        case PSC_RENAMEFILE:
            return lxRenameFile(Arg, (const char*)ReturnBuff);
            break;
        case PSC_COPYFILE:
            return lxCopyFile(Arg, (const char*)ReturnBuff);
            break;
        case PSC_LAUNCHDEFAULTBROWSER:
            return lxLaunchDefaultBrowser(Arg);
            break;
        case PSC_LAUNCHDEFAULAPPLICATION:
            return lxLaunchDefaultApplication(Arg);
            break;
        case PSC_SHOWDIALOG:
            return Dialog_sz(lxString::FromUTF8(Arg), 400, 200);
            break;
        case PSC_BASENAME:
            strcpy((char*)ReturnBuff, (const char*)basename(lxString::FromUTF8(Arg)).utf8_str());
            break;
        case PSC_DIRNAME:
            strcpy((char*)ReturnBuff, (const char*)dirname(lxString::FromUTF8(Arg)).utf8_str());
            break;

        case PSC_MUTEXCREATE: {
            int mid = -1;
            for (int i = 0; i < MUTEX_MAX; i++) {
                if (Window1.Mutexs[i] == NULL) {
                    mid = i;
                    break;
                }
            }
            if (mid >= 0) {
                Window1.Mutexs[mid] = new lxMutex;
            } else {
                printf("PICSimLab: Error PSC_MUTEXCREATE!\n");
            }
            return mid;
        } break;
        case PSC_MUTEXDESTROY: {
            int mid = *((int*)Arg);
            if (Window1.Mutexs[mid]) {
                delete Window1.Mutexs[mid];
                Window1.Mutexs[mid] = NULL;
            }
        } break;
        case PSC_MUTEXLOCK:
            return Window1.Mutexs[*((int*)Arg)]->Lock();
        case PSC_MUTEXUNLOCK:
            return Window1.Mutexs[*((int*)Arg)]->Unlock();
            break;

        case PSC_AUDIOCHCREATE: {
            int ach = -1;
            for (int i = 0; i < MAX_AUDIO; i++) {
                if (Window1.AudioChannels[i] == NULL) {
                    ach = i;
                    break;
                }
            }
            if (ach >= 0) {
                Window1.AudioChannels[ach] = new lxaudio;
                Window1.AudioChannels[ach]->Init();
            }
            return ach;
        } break;
        case PSC_AUDIOCHDESTROY: {
            int ach = *((int*)Arg);
            if (Window1.AudioChannels[ach]) {
                Window1.AudioChannels[ach]->End();
                delete Window1.AudioChannels[ach];
                Window1.AudioChannels[ach] = NULL;
            }
        } break;
        case PSC_AUDIOCHBEEPSTART:
            Window1.AudioChannels[*((int*)Arg)]->BeepStart();
            break;
        case PSC_AUDIOCHBEEPSTARTF: {
            float* args = (float*)ReturnBuff;
            Window1.AudioChannels[*((int*)Arg)]->BeepStart(args[0], args[1], args[2]);
        } break;
        case PSC_AUDIOCHBEEPSTOP:
            Window1.AudioChannels[*((int*)Arg)]->BeepStop();
            break;
        case PSC_AUDIOCHGETSAMPLERATE:
            *((unsigned int*)ReturnBuff) = Window1.AudioChannels[*((int*)Arg)]->GetSampleRate();
            break;
        case PSC_AUDIOCHGETMAX:
            *((unsigned int*)ReturnBuff) = Window1.AudioChannels[*((int*)Arg)]->GetMax();
            break;
        case PSC_AUDIOCHSOUNDPLAY: {
            short** ptr = (short**)ReturnBuff;
            Window1.AudioChannels[*((int*)Arg)]->SoundPlay(ptr[1], *((unsigned int*)ptr[0]));
        } break;

        default:
            return 0;
            break;
    }
    return 0;
}

int CPWindow1::OnWindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action, const char* Value,
                           void* ReturnBuff) {
    CControl* ctrl = NULL;
    int wid = -1;

    if (id >= 0) {
        if (ControlName) {
            ctrl = Window1.Windows[id]->GetChildByName(ControlName);
        } else {
            ctrl = Window1.Windows[id];
        }

        if (ctrl == NULL) {
            return -1;
        }
    } else if (action == PWA_WINDOWCREATE) {  // find empty window
        for (int i = 0; i < BOARDS_MAX; i++) {
            if (Window1.Windows[i] == NULL) {
                wid = i;
                break;
            }
        }

        if (wid == -1) {
            return -1;
        }
    } else {
        return -1;
    }

    switch (action) {
        case PWA_WINDOWCREATE: {
            Window1.Windows[wid] = new CPWindow();
            ctrl = Window1.Windows[wid];
            ctrl->SetName(ControlName);  // must be the same as in xml
            ctrl->SetVisible(0);
            ((CPWindow*)ctrl)->SetCanDestroy(false);
            Application->ACreateWindow(((CPWindow*)ctrl));
            return wid;
        } break;

        case PWA_BUTTONBOARDEV:
            if (!strcmp(Value, "1")) {
                ((CButton*)ctrl)->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow1::board_ButtonEvent;
            } else {
                ((CButton*)ctrl)->EvMouseButtonRelease = NULL;
            }
            break;

        default:
            return Window1.WinCmd(ctrl, action, Value, ReturnBuff);
            break;
    }

    return 0;
}

int CPWindow1::WinCmd(CControl* ctrl, const PICSimLabWindowAction action, const char* Value, void* ReturnBuff) {
    switch (action) {
        case PWA_COMBOSETITEMS:
            ((CCombo*)ctrl)->SetItems(Value);
            break;
        case PWA_COMBOSETTEXT:
            ((CCombo*)ctrl)->SetText(Value);
            break;
        case PWA_COMBOGETTEXT:
            strcpy((char*)ReturnBuff, ((CCombo*)ctrl)->GetText().c_str());
            return strlen((char*)ReturnBuff);
            break;
        case PWA_COMBODELETEITEMS:
            ((CCombo*)ctrl)->DeleteItems();
            break;
        case PWA_COMBOADDITEM:
            ((CCombo*)ctrl)->AddItem(Value);
            break;

        case PWA_SPINDSETMAX:
            ((CSpind*)ctrl)->SetMax(std::stof(Value));
            break;
        case PWA_SPINDSETMIN:
            ((CSpind*)ctrl)->SetMin(std::stof(Value));
            break;
        case PWA_SPINDSETVALUE:
            ((CSpind*)ctrl)->SetValue(std::stof(Value));
            break;
        case PWA_SPINDGETVALUE:
            *((float*)ReturnBuff) = ((CSpind*)ctrl)->GetValue();
            break;

        case PWA_SPINSETMAX:
            ((CSpin*)ctrl)->SetMax(std::stoi(Value));
            break;
        case PWA_SPINSETMIN:
            ((CSpin*)ctrl)->SetMin(std::stoi(Value));
            break;
        case PWA_SPINSETVALUE:
            ((CSpin*)ctrl)->SetValue(std::stoi(Value));
            break;
        case PWA_SPINGETVALUE:
            *((int*)ReturnBuff) = ((CSpin*)ctrl)->GetValue();
            break;

        case PWA_TEXTCLEAR:
            ((CText*)ctrl)->Clear();
            break;
        case PWA_TEXTADDLINE:
            ((CText*)ctrl)->AddLine(lxString::FromUTF8(Value));
            break;
        case PWA_TEXTGETLINE:
            strcpy((char*)ReturnBuff, ((CText*)ctrl)->GetLine(std::stoi(Value)).utf8_str());
            return strlen((char*)ReturnBuff);
            break;
        case PWA_TEXTGETLINECOUNT:
            *((int*)ReturnBuff) = ((CText*)ctrl)->GetCountLines();
            break;
        case PWA_TEXTAPPEND:
            ((CText*)ctrl)->Append(lxString::FromUTF8(Value));
            if (((CText*)ctrl)->GetLine(((CText*)ctrl)->GetCountLines() - 1).length() > 1024) {
                ((CText*)ctrl)->Append("\n");  // TODO break line with exact size
            }

            while (((CText*)ctrl)->GetCountLines() > 1000) {
                ((CText*)ctrl)->SetCursorPos(0);
                ((CText*)ctrl)->DelLine();
            }
            break;
        case PWA_TEXTTELETYPE:
            ((CText*)ctrl)->SetFontFamily(lxFONTFAMILY_TELETYPE);
            break;

        case PWA_CHECKBOXSETCHECK:
            ((CCheckBox*)ctrl)->SetCheck(std::stoi(Value));
            break;
        case PWA_CHECKBOXGETCHECK:
            *((int*)ReturnBuff) = ((CCheckBox*)ctrl)->GetCheck();
            break;

        case PWA_EDITSETTEXT:
            ((CEdit*)ctrl)->SetText(Value);
            break;
        case PWA_EDITGETTEXT:
            strcpy((char*)ReturnBuff, ((CEdit*)ctrl)->GetText().utf8_str());
            return strlen((char*)ReturnBuff);
            break;

        case PWA_LABELSETTEXT:
            ((CLabel*)ctrl)->SetText(Value);
            break;

        case PWA_SETENABLE:
            ctrl->SetEnable(std::stoi(Value));
            break;
        case PWA_SETWIDTH:
            ctrl->SetWidth(std::stoi(Value));
            break;
        case PWA_SETHEIGHT:
            ctrl->SetHeight(std::stoi(Value));
            break;
        case PWA_SETVISIBLE:
            ctrl->SetVisible(std::stoi(Value));
            break;
        case PWA_SETX:
            ctrl->SetX(std::stoi(Value));
            break;
        case PWA_SETY:
            ctrl->SetY(std::stoi(Value));
            break;
        case PWA_SETTAG:
            ctrl->SetTag(std::stoi(Value));
            break;
        case PWA_SETCOLOR:
            ctrl->SetColor(lxColor(Value));
            break;
        case PWA_GETWIDTH:
            *((int*)ReturnBuff) = ctrl->GetWidth();
            break;
        case PWA_GETHEIGHT:
            *((int*)ReturnBuff) = ctrl->GetHeight();
            break;
        case PWA_GETX:
            *((int*)ReturnBuff) = ctrl->GetX();
            break;
        case PWA_GETY:
            *((int*)ReturnBuff) = ctrl->GetY();
            break;
        case PWA_GETTAG:
            *((int*)ReturnBuff) = ctrl->GetTag();
            break;
        case PWA_GETCOLOR:
            strcpy((char*)ReturnBuff, ctrl->GetColor().GetAsString(lxC2S_HTML_SYNTAX));
            return strlen((char*)ReturnBuff);
            break;

        case PWA_WINDOWDESTROY:
            ((CPWindow*)ctrl)->Hide();
            ((CPWindow*)ctrl)->DestroyChilds();
            ((CPWindow*)ctrl)->SetCanDestroy(true);
            ((CPWindow*)ctrl)->WDestroy();
            ctrl = NULL;
            break;
        case PWA_WINDOWLOADXML:
            ((CPWindow*)ctrl)->DestroyChilds();
            return ((CPWindow*)ctrl)->LoadXMLContextAndCreateChilds(Value);
            break;
        case PWA_WINDOWSHOW:
            ((CPWindow*)ctrl)->Show();
            break;
        case PWA_WINDOWHIDE:
            ((CPWindow*)ctrl)->Hide();
            break;
        case PWA_WINDOWSHOWEX:
            ((CPWindow*)ctrl)->Draw();
            ((CPWindow*)ctrl)->ShowExclusive();
            break;
        case PWA_WINDOWHIDEEX:
            ((CPWindow*)ctrl)->HideExclusive();
            break;
        case PWA_WINDOWHASCREATED:
            *((int*)ReturnBuff) = (((CPWindow*)ctrl)->GetWin() != NULL);
            break;

        case PWA_GETDISPLAYWIDTH:
            *((unsigned int*)ReturnBuff) = lxGetDisplayWidth(0);
            break;
        case PWA_GETDISPLAYHEIGHT:
            *((unsigned int*)ReturnBuff) = lxGetDisplayHeight(0);
            break;

        case PWA_APPPROCESSEVENTS:
            Application->ProcessEvents();
            break;

        case PWA_FILEDIALOGGETFNAME:
            strcpy((char*)ReturnBuff, ((CFileDialog*)ctrl)->GetFileName().utf8_str());
            return strlen((char*)ReturnBuff);
            break;
        case PWA_FILEDIALOGSETFNAME:
            ((CFileDialog*)ctrl)->SetFileName(lxString::FromUTF8(Value));
            break;
        case PWA_FILEDIALOGSETFILTER:
            ((CFileDialog*)ctrl)->SetFilter(Value);
            break;
        case PWA_FILEDIALOGSETTYPE:
            ((CFileDialog*)ctrl)->SetType(std::stoi(Value));
            break;
        case PWA_FILEDIALOGRUN:
            ((CFileDialog*)ctrl)->Run();
            break;
        case PWA_FILEDIALOGGETTYPE:
            *((int*)ReturnBuff) = ((CFileDialog*)ctrl)->GetType();
            break;

        case PWA_THREADGETRUNSTATE:
            *((int*)ReturnBuff) = ((CThread*)ctrl)->GetRunState();
            break;
        case PWA_THREADTESTDESTROY:
            *((int*)ReturnBuff) = ((CThread*)ctrl)->TestDestroy();
            break;
        case PWA_THREADRUN:
            ((CThread*)ctrl)->Run();
            break;
        case PWA_THREADDESTROY:
            ((CThread*)ctrl)->Destroy();
            break;

        case PWA_TIMERGETTIME:
            *((int*)ReturnBuff) = ((CTimer*)ctrl)->GetTime();
            break;

        case PWA_TOGGLEBSETCHECK:
            ((CToggleButton*)ctrl)->SetCheck(std::stoi(Value));
            break;
        case PWA_TOGGLEBGETCHECK:
            *((int*)ReturnBuff) = ((CToggleButton*)ctrl)->GetCheck();
            break;

        default:
            return -1;
            break;
    }
    return 0;
}

#ifdef __EMSCRIPTEN__

void SimRun(int run) {
    PICSimLab.SetSimulationRun(run);
}

int SimStat(void) {
    return PICSimLab.GetSimulationRun();
}
#endif
}

#ifdef FAKESJLJ
typedef unsigned _Unwind_Exception_Class __attribute__((__mode__(__DI__)));

void __gxx_personality_sj0(int version, int actions, _Unwind_Exception_Class exception_class, void* ue_header,
                           void* context) {
    std::abort();
}
#endif

// Check for update support

#ifndef NO_TOOLS

#define MEMMAX 512
// #define DEBUG

struct BufferStruct {
    char data[MEMMAX];
    size_t size;
};

static size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct BufferStruct* mem = (struct BufferStruct*)userp;

    if (mem->size + realsize + 1 > MEMMAX) {
        /* out of data! */
        printf("not enough data \n");
        return 0;
    }

    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

int CheckRemoteNewVersion(int check_for_devel) {
    CURL* curl;

    struct BufferStruct MemBuffer;

    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result)
        return -1;

    MemBuffer.data[0] = 0;
    MemBuffer.size = 0;

    lxString ca_path = PICSimLab.GetSharePath() + "github-ca.crt";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL,
                         "https://raw.githubusercontent.com/lcgamboa/picsimlab/refs/heads/master/VERSION");

        curl_easy_setopt(curl, CURLOPT_CAINFO, (const char*)ca_path.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&MemBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        result = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    /* Check for errors */
    if (result != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        printf("Cert file at:%s\n", (const char*)ca_path.c_str());
        return -1;
    }

#ifdef DEBUG
    printf("%s", MemBuffer.data);
#endif

    int maj, min, rev;

    int remote_version = -1;
    int remote_date = -1;

    char* ptr = strtok(MemBuffer.data, "\n");

    while (ptr) {
        if (ptr) {
            if (check_for_devel) {
                if (!strncmp(ptr, "VERSION=", 8)) {
                    sscanf(ptr + 8, "%i.%i.%i", &maj, &min, &rev);
                    remote_version = maj * 10000 + min * 100 + rev;
                }
                if (!strncmp(ptr, "DATE=", 5)) {
                    sscanf(ptr + 5, "%i", &remote_date);
                }
            } else {
                if (!strncmp(ptr, "STABLE_VERSION=", 15)) {
                    sscanf(ptr + 15, "%i.%i.%i", &maj, &min, &rev);
                    remote_version = maj * 10000 + min * 100 + rev;
                }
                if (!strncmp(ptr, "STABLE_DATE=", 12)) {
                    sscanf(ptr + 12, "%i", &remote_date);
                }
            }
        }
        ptr = strtok(NULL, "\n");
    }

    sscanf(_VERSION_, "%i.%i.%i", &maj, &min, &rev);
    int my_version = maj * 10000 + min * 100 + rev;
    int my_date;
    sscanf(_DATE_, "%i", &my_date);

    if ((remote_version > my_version) || (remote_date > my_date)) {
#ifdef DEBUG
        printf("PICSimLab: Remote version %i_%i is newer than PICSimLab %i_%i!\n", remote_version, remote_date,
               my_version, my_date);
#endif
        return 1;
    } else {
#ifdef DEBUG
        printf("PICSimLab: Remote version %i_%i is equal or lower than PICSimLab %i_%i!\n", remote_version, remote_date,
               my_version, my_date);
#endif
    }

    return 0;
}
#endif

void CPWindow1::menu1_Help_Check_for_Update_EvMenuActive(CControl* control) {
    int HasUpdate = CheckRemoteNewVersion(1);  // FIXME Add support to select argument from config windows
    if (HasUpdate == 1) {
        printf("PICSimLab: New version available!\n");
        if (Dialog_sz("New version available!\n Open browser for download?", 400, 200)) {
            lxLaunchDefaultBrowser("https://github.com/lcgamboa/picsimlab/releases/tag/latestbuild");
        }

    } else if (HasUpdate == 0) {
        printf("PICSimLab: No update available.\n");
        Message_sz("No update available.", 400, 200);
    } else {
        printf("PICSimLab: Error connecting to remote server.\n");
        Message_sz("Error connecting to remote server.", 400, 200);
    }
}
