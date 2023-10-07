/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "lib/picsimlab.h"

#include "picsimlab1.h"
#include "picsimlab1_d.cc"

CPWindow1 Window1;

// Implementation

#include "picsimlab2.h"
#include "picsimlab3.h"
#include "picsimlab4.h"
#include "picsimlab5.h"

#include "lib/oscilloscope.h"
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

#ifdef _USE_PICSTARTP_
// picstart plus
int prog_init(void);
int prog_loop(_pic* pic);
int prog_end(void);
#endif

#ifdef CONVERTER_MODE
static lxString cvt_fname;
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
    if (PICSimLab.status.st[0] & (ST_T1 | ST_DI))
        return;

    PICSimLab.SetSync(1);
    PICSimLab.status.st[0] |= ST_T1;

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
    PICSimLab.cpu_mutex->Lock();
    PICSimLab.cpu_cond->Signal();
    PICSimLab.cpu_mutex->Unlock();
#endif

    if (PICSimLab.tgo > 3) {
        if (timer1.GetTime() < 330) {
            timer1.SetTime(timer1.GetTime() + 5);
        }
        PICSimLab.tgo = 1;
    }

    DrawBoard();

    PICSimLab.status.st[0] &= ~ST_T1;
}

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
                lxGetLocalFile(PICSimLab.GetSharePath() + lxT("boards/") + PICSimLab.GetBoard()->GetPictureFileName()),
                PICSimLab.GetScale(), PICSimLab.GetScale());
        }

        if (PICSimLab.GetBoard()) {
            PICSimLab.GetBoard()->SetScale(PICSimLab.GetScale());
            PICSimLab.GetBoard()->EvOnShow();
            PICSimLab.GetBoard()->Draw(&draw1);
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
        PICSimLab.GetBoard()->Draw(&draw1);
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

            PICSimLab.status.st[1] |= ST_TH;
            PICSimLab.GetBoard()->Run_CPU();
            if (PICSimLab.GetDebugStatus())
                PICSimLab.GetBoard()->DebugLoop();
            PICSimLab.tgo--;
            PICSimLab.status.st[1] &= ~ST_TH;

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
            PICSimLab.cpu_mutex->Lock();
            PICSimLab.cpu_cond->Wait();
            PICSimLab.cpu_mutex->Unlock();
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
    PICSimLab.GetBoard()->EvThreadRun(thread3);
}

void CPWindow1::timer2_EvOnTime(CControl* control) {
    // avoid run again before terminate previous
    if (PICSimLab.status.st[0] & (ST_T2 | ST_DI))
        return;

    PICSimLab.status.st[0] |= ST_T2;
    if (PICSimLab.GetBoard() != NULL) {
        PICSimLab.GetBoard()->RefreshStatus();

        switch (PICSimLab.GetCpuState()) {
            case CPU_RUNNING:
                statusbar1.SetField(0, lxT("Running..."));
                break;
            case CPU_STEPPING:
                statusbar1.SetField(0, lxT("Stepping..."));
                break;
            case CPU_HALTED:
                statusbar1.SetField(0, lxT("Halted!"));
                break;
            case CPU_BREAKPOINT:
                statusbar1.SetField(0, lxT("BreakPoint!"));
                break;
            case CPU_POWER_OFF:
                statusbar1.SetField(0, lxT("Power Off!"));
                break;
            case CPU_ERROR:
                statusbar1.SetField(0, lxT("Error!"));
                break;
        }
    }

    label2.SetText(lxString().Format("Spd: %3.2fx", ((float)BASETIMER) / timer1.GetTime()));

    if (PICSimLab.GetErrorCount()) {
#ifndef __EMSCRIPTEN__
        Message_sz(PICSimLab.GetError(0), 600, 240);
#else
        printf("Error: %s\n", PICSimLab.GetError(0).c_str());
#endif
        PICSimLab.DeleteError(0);
    }
    PICSimLab.status.st[0] &= ~ST_T2;

#ifdef CONVERTER_MODE
    if (cvt_fname.Length() > 3) {
        SaveWorkspace(cvt_fname);
    }
#endif

    if (PICSimLab.GetToDestroy()) {
        WDestroy();
    }

    if (PICSimLab.GetNeedClkUpdate()) {
        PICSimLab.SetClock(PICSimLab.GetClock());
    }
}

void CPWindow1::draw1_EvMouseMove(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseMove(button, x, y, state);
}

void CPWindow1::draw1_EvMouseButtonPress(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseButtonPress(button, x, y, state);
}

void CPWindow1::draw1_EvMouseButtonRelease(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / PICSimLab.GetScale();
    y = y / PICSimLab.GetScale();

    PICSimLab.GetBoard()->EvMouseButtonRelease(button, x, y, state);
}

void CPWindow1::draw1_EvKeyboardPress(CControl* control, const uint key, const uint hkey, const uint mask) {
    PICSimLab.GetBoard()->EvKeyPress(key, mask);
}

void CPWindow1::draw1_EvKeyboardRelease(CControl* control, const uint key, const uint hkey, const uint mask) {
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
static void set_signal_handler(void){};
#endif

void CPWindow1::_EvOnCreate(CControl* control) {
    char home[1024];
    lxFileName fn;
    lxFileName fn_spare;
    char fname[1200];
    char fname_error[1200];
    int close_error = 0;

    set_signal_handler();

    PICSimLab.menu_EvBoard = EVMENUACTIVE & CPWindow1::menu1_EvBoard;
    PICSimLab.menu_EvMicrocontroller = EVMENUACTIVE & CPWindow1::menu1_EvMicrocontroller;
    PICSimLab.board_Event = EVONCOMBOCHANGE & CPWindow1::board_Event;
    PICSimLab.board_ButtonEvent = EVMOUSEBUTTONRELEASE & CPWindow1::board_ButtonEvent;
    PICSimLab.Init(this);

    Oscilloscope.Init(&Window4);

    SpareParts.PropButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    SpareParts.PropComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;
    SpareParts.PropSpinChange = EVONCHANGESPIN & CPWindow5::PropSpinChange;
    SpareParts.PartEvent = EVONCOMBOCHANGE & CPWindow5::PartEvent;
    SpareParts.PartButtonEvent = EVMOUSEBUTTONRELEASE & CPWindow5::PartButtonEvent;
    SpareParts.PartKeyEvent = EVKEYBOARDPRESS & CPWindow5::PartKeyEvent;
    SpareParts.Init(&Window5);

    PICSimLab.SetWorkspaceFileName("");

    strncpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str(), 1023);

    PICSimLab.SetHomePath(home);

    PICSimLab.SetPath(lxGetCwd());

#ifndef _SHARE_
#error Define the _SHARE_ path is necessary
#endif

    if (lxString(_SHARE_).Contains("http")) {
        PICSimLab.SetSharePath(lxString(_SHARE_));
    } else {
        PICSimLab.SetSharePath(dirname(lxGetExecutablePath()) + lxT("/") + lxString(_SHARE_));
    }
    fn.Assign(PICSimLab.GetSharePath());
    fn.MakeAbsolute();
    PICSimLab.SetSharePath(fn.GetFullPath() + "/");

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

    PICSimLab.SetLibPath(dirname(lxGetExecutablePath()) + lxT("/") + lxString(_LIB_));
    fn.Assign(PICSimLab.GetLibPath());
    fn.MakeAbsolute();
    PICSimLab.SetLibPath(fn.GetFullPath() + "/");

#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
    snprintf(fname, 1199, "%s/picsimlab_log%i.txt", home, PICSimLab.GetInstanceNumber());
    if (lxFileExists(fname)) {
        FILE* flog = fopen(fname, "r");
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
                lxLaunchDefaultApplication(fname_error);
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

#endif

    printf("PICSimLab: Version \"%s %s %s %s\"\n", _VERSION_, _DATE_, _ARCH_, _PKG_);

    printf("PICSimLab: Command Line: ");
    for (int i = 0; i < Application->Aargc; i++) {
        printf("%s ", Application->Aargv[i]);
    }
    printf("\n");

    fflush(stdout);

    if (close_error) {
        printf(
            "PICSimLab: Error closing PICSimLab in last time! \nUsing default mode.\n Erro log file: %s\n If the "
            "problem persists, please consider opening an issue on github..",
            fname_error);
        PICSimLab.Configure(home, 1, 1);
        PICSimLab.RegisterError(
            "Error closing PICSimLab in last time!\n Using default mode.\n Error log file: " + lxString(fname_error) +
            "\n If the problem persists, please consider opening an issue on github.\n ");
    } else if (Application->Aargc == 2) {  // only .pzw file
        fn.Assign(Application->Aargv[1]);
        fn.MakeAbsolute();
        // load options
        PICSimLab.Configure(home, 1, 1);

        // check if it is a demonstration

        lxString fns = fn.GetFullPath();

        lxFileName fn_dir;
        fn_dir.Assign(PICSimLab.GetSharePath() + "boards/");
        fn_dir.MakeAbsolute();

        if (fns.Contains(fn_dir.GetFullPath()) && fns.Contains("demo.pzw")) {
            PICSimLab.LoadWorkspace(fn.GetFullPath(), 0);
            PICSimLab.SetWorkspaceFileName("");
        } else {
            PICSimLab.LoadWorkspace(fn.GetFullPath());
        }

    } else if ((Application->Aargc >= 3) && (Application->Aargc <= 5)) {
        // arguments: Board Processor File.hex(.bin) file.pcf

        if (Application->Aargc >= 4) {
            fn.Assign(Application->Aargv[3]);
            fn.MakeAbsolute();
        }
        if (Application->Aargc == 5) {
            fn_spare.Assign(Application->Aargv[4]);
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
            snprintf(fname, 1199, "%s/picsimlab.ini", home);
            PICSimLab.PrefsClear();
            if (lxFileExists(fname)) {
                if (PICSimLab.PrefsLoadFromFile(fname)) {
                    PICSimLab.SavePrefs(lxT("picsimlab_lab"), boards_list[PICSimLab.GetLab()].name_);
                    PICSimLab.SavePrefs(lxString(boards_list[PICSimLab.GetLab()].name_) + lxT("_proc"),
                                        Application->Aargv[2]);
                    if (Application->Aargc == 5) {
                        PICSimLab.SavePrefs(lxT("spare_on"), lxT("1"));
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
            PICSimLab.Configure(home, 0, 1, (const char*)fn.GetFullPath().c_str());
            if (Application->Aargc == 5) {
                SpareParts.LoadConfig(fn_spare.GetFullPath());
            }
        } else {
            // load options
            PICSimLab.Configure(home, 0, 1);
        }
    } else {  // no arguments
        // load options
        PICSimLab.Configure(home, 0, 1);
    }
    label1.SetText(PICSimLab.GetBoard()->GetClkLabel());
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
    flog = fopen(fname, "w");
    if (flog) {
        // copy
        ftmp = fopen(tmpname, "r");
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

    while (PICSimLab.status.st[1] & ST_TH)
        usleep(100);  // wait thread

    if (retId && (filedialog1.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
        PICSimLab.SetPath(filedialog1.GetDir());
        PICSimLab.SetFNAME(filedialog1.GetFileName());
        PICSimLab.LoadHexFile(filedialog1.GetFileName());
        menu1_File_ReloadLast.SetEnable(1);
    }

    if (retId && (filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
        PICSimLab.GetBoard()->MDumpMemory(filedialog1.GetFileName());
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
    // lxLaunchDefaultBrowser(lxT("file://")+PICSimLab.GetSharePath() + lxT ("docs/picsimlab.html"));
    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/index.html"), lxT(_VERSION_));
    lxLaunchDefaultBrowser(stemp);
#else
    Window2.html1.SetLoadFile(PICSimLab.GetSharePath() + lxT("docs/picsimlab.html"));
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

    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/%s.html"), lxT(_VERSION_), bname);
    lxLaunchDefaultBrowser(stemp);
}

void CPWindow1::menu1_Help_About_Board_EvMenuActive(CControl* control) {
    Message_sz(lxT("Board ") + lxString(boards_list[PICSimLab.GetLab()].name) + lxT("\nDeveloped by ") +
                   PICSimLab.GetBoard()->GetAboutInfo(),
               400, 200);
}

void CPWindow1::menu1_Help_About_PICSimLab_EvMenuActive(CControl* control) {
    lxString stemp;
    stemp.Printf(lxT("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s %s"), lxT(_VERSION_),
                 lxT(_DATE_), lxT(_ARCH_), lxT(_PKG_));
    Message_sz(stemp, 400, 200);
}

void CPWindow1::menu1_Help_Examples_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER_EXAMPLES
    // lxLaunchDefaultBrowser(lxT("file://")+PICSimLab.GetSharePath() + lxT ("docs/picsimlab.html"));
    lxLaunchDefaultBrowser(lxT("https://lcgamboa.github.io/picsimlab_examples/board_" +
                               lxString(boards_list[PICSimLab.GetLab()].name_) + ".html#board_" +
                               lxString(boards_list[PICSimLab.GetLab()].name_) + lxT("_") +
                               PICSimLab.GetBoard()->GetProcessorName()));
    // SetToDestroy();
#else
    OldPath = filedialog2.GetDir();

    filedialog2.SetDir(PICSimLab.GetSharePath() + lxT("/docs/hex/board_") + lxString(boards_list[lab].name_) +
                       lxT("/") + PICSimLab.GetBoard()->GetProcessorName() + lxT("/"));

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

void CPWindow1::_EvOnDropFile(CControl* control, const lxString fname) {
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
    PICSimLab.GetBoard()->board_Event(control);
}

void CPWindow1::board_ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) {
    PICSimLab.GetBoard()->board_ButtonEvent(control, button, x, y, state);
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
        PICSimLab.SetFNAME(lxT(" "));
        PICSimLab.EndSimulation(1);
        PICSimLab.Configure(PICSimLab.GetHomePath());
        PICSimLab.SetNeedResize(1);
        label1.SetText(PICSimLab.GetBoard()->GetClkLabel());
    }
}

// change microcontroller

void CPWindow1::menu1_EvMicrocontroller(CControl* control) {
    if (!PICSimLab.GetErrorCount()) {
        PICSimLab.SetProcessorName(PICSimLab.GetBoard()->GetProcessorName());
        PICSimLab.GetBoard()->SetProcessorName(((CItemMenu*)control)->GetText());

        SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                      ? (lxT("PICSimLab[") + itoa(PICSimLab.GetInstanceNumber()) + lxT("] - "))
                      : (lxT("PICSimLab - "))) +
                 lxString(boards_list[PICSimLab.GetLab()].name) + lxT(" - ") +
                 PICSimLab.GetBoard()->GetProcessorName());

        PICSimLab.SetFNAME(lxT(" "));
        PICSimLab.EndSimulation();
        PICSimLab.Configure(PICSimLab.GetHomePath());
        PICSimLab.SetNeedResize(1);
        SpareParts.SetUseAlias(0);
    }
}

void CPWindow1::togglebutton1_EvOnToggleButton(CControl* control) {
#ifdef NO_DEBUG
    statusbar1.SetField(1, lxT(" "));
#else
    int osc_on = PICSimLab.GetBoard()->GetUseOscilloscope();
    int spare_on = PICSimLab.GetBoard()->GetUseSpareParts();

    PICSimLab.SetDebugStatus(togglebutton1.GetCheck(), 0);

    PICSimLab.EndSimulation();
    PICSimLab.Configure(PICSimLab.GetHomePath());

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

void CPWindow1::menu1_File_LoadWorkspace_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog2.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    filedialog2.Run();
#endif
}

void CPWindow1::menu1_File_LoadBoardDemo_EvMenuActive(CControl* control) {
    lxString fdemo =
        PICSimLab.GetSharePath() + "boards/" + lxString(boards_list[PICSimLab.GetLab()].name) + lxT("/demo.pzw");

    if (lxFileExists(fdemo)) {
        PICSimLab.LoadWorkspace(fdemo);
        PICSimLab.SetWorkspaceFileName("");
    } else {
        PICSimLab.RegisterError("Demo for board " + lxString(boards_list[PICSimLab.GetLab()].name) + " not found!");
        printf("PICSimLab: Demo for board  %s not found! (%s)\n", boards_list[PICSimLab.GetLab()].name,
               (const char*)fdemo.c_str());
    }
}

void CPWindow1::filedialog2_EvOnClose(int retId) {
    if (retId && (filedialog2.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
        PICSimLab.LoadWorkspace(filedialog2.GetFileName());
        if (PICSimLab.GetOldPath().size() > 1) {
            filedialog2.SetDir(PICSimLab.GetOldPath());
            PICSimLab.SetOldPath("");
        }
    }

    if (retId && (filedialog2.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
        PICSimLab.SaveWorkspace(filedialog2.GetFileName());
    }
}

void CPWindow1::menu1_Tools_SerialTerm_EvMenuActive(CControl* control) {
    char stfname[1024];
    snprintf(stfname, 1024, "%s/open_w_cutecom_or_gtkterm.sterm", (const char*)lxGetTempDir("PICSimLab").c_str());

    if (!lxFileExists(stfname)) {
        // create one dumb file to associate whit serial terminal
        FILE* fout;
        fout = fopen(stfname, "w");
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
    lxExecute(PICSimLab.GetSharePath() + lxT("/../srtank.exe"));
#else

    lxExecute(dirname(lxGetExecutablePath()) + "/srtank");
#endif
}

void CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive(CControl* control) {
#ifdef _WIN_
    lxExecute(PICSimLab.GetSharePath() + lxT("/../espmsim.exe"));
#else
    lxExecute(dirname(lxGetExecutablePath()) + "/espmsim");
#endif
}

void CPWindow1::menu1_Tools_ArduinoBootloader_EvMenuActive(CControl* control) {
    PICSimLab.LoadHexFile(PICSimLab.GetSharePath() + "bootloaders/arduino_" + PICSimLab.GetBoard()->GetProcessorName() +
                          ".hex");
}

void CPWindow1::menu1_Tools_MPLABXDebuggerPlugin_EvMenuActive(CControl* control) {
    lxLaunchDefaultBrowser(lxT("https://github.com/lcgamboa/picsimlab_md/releases"));
}

void CPWindow1::menu1_Tools_PinViewer_EvMenuActive(CControl* control) {
#ifdef _WIN_
    lxExecute(PICSimLab.GetSharePath() + lxT("/../PinViewer.exe " + itoa(PICSimLab.GetRemotecPort())));
#else
    lxExecute(dirname(lxGetExecutablePath()) + "/PinViewer " + itoa(PICSimLab.GetRemotecPort()));
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
