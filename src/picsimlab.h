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

#ifndef PICSIMLAB
#define PICSIMLAB

#define TIMER 0.1        // timer period in s
#define CPUTUSEMAX 0.1   // max period cpu use in s
#define NSTEPKT 25000.0  // TIMER constant 1MHz/(4.0*timer_freq)
#define NSTEPKF 40.0     // Freq constant 4.0*timer_freq
#define DEFAULTJS 100    // IO refresh rate

extern char SERIALDEVICE[100];

#define MAX_MIC 140

#include <lxrad.h>
#include "boards/board.h"
#include "boards/boards_defs.h"

enum { CPU_RUNNING, CPU_STEPPING, CPU_HALTED, CPU_BREAKPOINT, CPU_ERROR, CPU_POWER_OFF };

class CPICSimLab {
public:
    CPICSimLab();

    void Init(CWindow* w);

    CWindow* GetWindow(void) { return Window; };

    /**
     * @brief  Get the file path of resources
     */
    lxString GetSharePath(void) { return share; };
    void SetSharePath(lxString spath) { share = spath; };

    lxString GetLibPath(void) { return libpath; };
    void SetLibPath(lxString lpath) { libpath = lpath; };

    lxString GetHomePath(void) { return HOME; };
    void SetHomePath(lxString home) { HOME = home; };

    lxString GetPath(void) { return PATH; };
    void SetPath(lxString path) { PATH = path; };

    lxString GetFNAME(void) { return FNAME; };
    void SetFNAME(lxString fname) { FNAME = fname; };

    lxString GetOldPath(void) { return OldPath; };
    void SetOldPath(lxString op) { OldPath = op; };

    lxString GetProcessorName(void) { return proc_; };
    void SetProcessorName(lxString pn) { proc_ = pn; };

    long int GetNSTEP(void) { return NSTEP; };
    void SetNSTEP(long int ns) { NSTEP = ns; };

    /**
     * @brief  Get the number of steps in 100ms of simulation
     */
    long int GetNSTEPJ(void) { return NSTEPJ; };
    void SetNSTEPJ(long int nsj) { NSTEPJ = nsj; };

    /**
     * @brief  Get the number of steps to be skipped in board update
     */
    int GetJUMPSTEPS(void) { return JUMPSTEPS; };
    void SetJUMPSTEPS(int js);

    /**
     * @brief  Return a pointer to board object
     */
    board* GetBoard(void) { return pboard; };
    void SetBoard(board* b) { pboard = b; };
    void DeleteBoard(void);

    CStatusbar* GetStatusBar(void) { return statusbar; };

    /**
     * @brief  Return actual power status of microcontroller ON/OFF
     */
    int GetMcuPwr(void) { return mcupwr; };

    /**
     * @brief  Retunr if microcontroller reset pin is enabled
     */
    int GetMcuRst(void) { return mcurst; };

    int GetMcuDbg(void) { return mcudbg; };

    /**
     * @brief  Set the power status of microcontroller ON/OFF
     */
    void SetMcuPwr(int pp) {
        mcupwr = pp;
        if (mcupwr)
            SetCpuState(CPU_RUNNING);
        else
            SetCpuState(CPU_POWER_OFF);
    };

    void SetCpuState(const unsigned char cs);

    /**
     * @brief  Set mcu rst flag (inform simulator about mcu reset state)
     */
    void SetMcuRst(int pr) { mcurst = pr; };

    char GetCpuState(void) { return cpustate; };

    void Set_mcudbg(int pd);

    void SetMcuRun(int mr) { mcurun = mr; };
    int GetMcuRun(void) { return mcurst; };

    /**
     * @brief  Return the selected debugger port
     */
    unsigned short GetDebugPort(void) { return debug_port + Instance; };

    /**
     * @brief  Set debug TCP port
     */
    void SetDebugPort(unsigned short dp);

    /**
     * @brief  Return the selected remote control port
     */
    unsigned short GetRemotecPort(void) { return remotec_port + Instance; };

    /**
     * @brief  Set remote control TCP port
     */
    void SetRemotecPort(unsigned short rcp);

    /**
     * @brief  Return the program instance number
     */
    int GetInstanceNumber(void) { return Instance; };

    /**
     * @brief  Return the selected debugger type
     */
    int GetDebugType(void) { return debug_type; };

    /**
     * @brief  Set debug type (MDB or GDB)
     */
    void SetDebugType(int dt) { debug_type = dt; };

    /**
     * @brief  Get debug status (On/Off)
     */
    int GetDebugStatus(void);

    /**
     * @brief  Set debug status flag
     */
    void SetDebugStatus(int dbs, int updatebtn = 1);

    void SetClock(const float clk, const int update = 1);
    float GetClock(void);

    int GetNeedClkUpdate(void) { return need_clkupdate; };

    /**
     * @brief  Save the preferences
     */
    void SavePrefs(lxString name, lxString value);

    void PrefsClear(void) { prefs.Clear(); };
    int PrefsSaveToFile(lxString fname) { return prefs.SaveToFile(fname); };
    int PrefsLoadFromFile(lxString fname) { return prefs.LoadFromFile(fname); };
    unsigned int PrefsGetLinesCount(void) { return prefs.GetLinesCount(); };
    lxString PrefsGetLine(int ln) { return prefs.GetLine(ln); };

    void OpenLoadHexFileDialog(void);

    void SetNeedReboot(int nr = 1);
    int GetNeedReboot(void) { return NeedReboot; };

    void RegisterError(const lxString error);
    int GetErrorCount(void) { return Errors.GetLinesCount(); };

    lxString GetError(int en) { return Errors.GetLine(en); };

    void DeleteError(int en) { Errors.DelLine(en); };

    void EndSimulation(int saveold = 0, const char* newpath = NULL);

    void SetWorkspaceFileName(const lxString fname) { Workspacefn = fname; };

    void SetLabs(const int lb, const int lb_) {
        lab = lb;
        lab_ = lb_;
    };

    int GetLab(void) { return lab; };
    int GetLab_(void) { return lab_; };

    void LoadWorkspace(lxString fnpzw);
    void SaveWorkspace(lxString fnpzw);

    void SetSimulationRun(int run);
    int GetSimulationRun(void);

    double GetScale(void) { return scale; };
    void SetScale(double s) { scale = s; };

    int GetNeedResize(void) { return need_resize; };
    void SetNeedResize(int nr) { need_resize = nr; };

    void Configure(const char* home, int use_default_board = 0, int create = 0, const char* lfile = NULL);

    void SetplWidth(int pw) { plWidth = pw; };

    void SetplHeight(int ph) { plHeight = ph; };

    double GetIdleMs(void);
    void SetIdleMs(double im);

#ifndef _NOTHREAD
    lxCondition* cpu_cond;
    lxMutex* cpu_mutex;
#endif
    union {
        char st[2];
        unsigned short int status;
    } status;

    int tgo;

    int osc_on;
    int spare_on;
    int plWidth;
    int plHeight;

    CItemMenu MBoard[BOARDS_MAX];
    CItemMenu MMicro[MAX_MIC];

    void (CControl::*menu_EvBoard)(CControl* control);
    void (CControl::*menu_EvMicrocontroller)(CControl* control);
    void (CControl::*board_Event)(CControl* control);
    void (CControl::*board_ButtonEvent)(CControl* control, const uint button, const uint x, const uint y,
                                        const uint mask);

private:
    void StartRControl(void);
    CWindow* Window;
    board* pboard;
    CStatusbar* statusbar;
    int lab;
    int lab_;
    lxString share;
    lxString libpath;
    long int NSTEP;
    long int NSTEPJ;
    int JUMPSTEPS;
    int mcurun;
    int mcupwr;
    int mcurst;
    int mcudbg;
    char cpustate;
    unsigned short debug_port;
    unsigned short remotec_port;
    lxString HOME;
    lxString PATH;
    lxString FNAME;
    lxString OldPath;
    lxString proc_;
    lxString pzw_ver;
    int Instance;
    int debug_type;
    int debug;
    int need_resize;
    int need_clkupdate;
    lxStringList prefs;
    int NeedReboot;
    lxStringList Errors;
    lxString Workspacefn;
    double scale;
    double idle_ms;
};

extern CPICSimLab PICSimLab;

#ifdef _WIN_
#define msleep(x) Sleep(x)
// extern void usleep(unsigned int usec);
#include <unistd.h>
#else
#define msleep(x) usleep(x * 1000)
#endif

#define ST_T1 0x01
#define ST_T2 0x02
#define ST_TH 0x04
#define ST_DI 0x80  // DISABLE

#ifdef _WIN_
#define NULLFILE "\\\\.\\NUL"
#else
#define NULLFILE "/dev/null"
#endif

#endif  // PICSIMLAB
