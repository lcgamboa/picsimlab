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

#include "picsimlab.h"
#include "oscilloscope.h"
#include "spareparts.h"

#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "rcontrol.h"

#ifdef _USE_PICSTARTP_
extern char PROGDEVICE[100];
#endif
char SERIALDEVICE[100];

CPICSimLab PICSimLab;

CPICSimLab::CPICSimLab() {
    JUMPSTEPS = DEFAULTJS;
    NSTEP = NSTEPKT;
    NSTEPJ = NSTEP / JUMPSTEPS;
    pboard = NULL;
    mcurun = 1;
    mcupwr = 1;
    mcurst = 0;
    mcudbg = 0;
    cpustate = CPU_RUNNING;
    debug_port = 1234;
    remotec_port = 5000;
    HOME = "";
    FNAME = " ";
    OldPath = "";
    PATH = "";
    Instance = 0;
    debug_type = 0;
    debug = 0;
    Errors.clear();
    NeedReboot = 0;
    lab = DEFAULT_BOARD;
    lab_ = DEFAULT_BOARD;
    Workspacefn = "";
    status = 0;
    scale = 1.0;
    need_resize = 0;
    tgo = 0;
    plWidth = 10;
    plHeight = 10;
    use_dsr_reset = 1;
    settodestroy = 0;
    sync = 0;
    SHARE = "";
    pzwtmpdir[0] = 0;

    OnUpdateStatus = NULL;
    OnConfigure = NULL;
    OnClockSet = NULL;
    OnReadPreferences = NULL;
    OnSavePrefs = NULL;
    OnLoadHexFile = NULL;
    OnOpenLoadHexFileDialog = NULL;
    OnEndSimulation = NULL;
    OnUpdateGUI = NULL;
    OnConfigMenuGUI = NULL;
    OnCanvasCmd = NULL;
    OnWindowCmd = NULL;
}

void CPICSimLab::Init(void) {
    // check for other instances
    StartRControl();
}
/*
#ifdef _WIN_
void usleep(unsigned int usec) {
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * (__int64)usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif
*/

void CPICSimLab::SetJUMPSTEPS(int js) {
    JUMPSTEPS = js;
    if (JUMPSTEPS) {
        NSTEPJ = NSTEP / JUMPSTEPS;
    } else {
        NSTEPJ = NSTEP;
    }
}

void CPICSimLab::DeleteBoard(void) {
    if (pboard) {
        delete pboard;
        pboard = NULL;
    }
    mcurun = 1;
    mcupwr = 1;
    mcurst = 0;
}

void CPICSimLab::SetCpuState(const PICSimlabCPUState cs) {
    cpustate = cs;
}

void CPICSimLab::StartRControl(void) {
    char line[1024];
    char fname[1024];
    char* name;
    char* value;

    int lc;

    std::string status;

    mcurun = 1;
    mcupwr = 1;
    mcurst = 0;

    snprintf(fname, 1023, "%s/picsimlab.ini", (const char*)HOME.c_str());

    prefs.clear();
    if (SystemCmd(PSC_FILEEXISTS, fname)) {
        if (LoadFromFile(prefs, fname)) {
            for (lc = 0; lc < (int)prefs.size(); lc++) {
                strncpy(line, prefs.at(lc).c_str(), 1023);

                name = strtok(line, "\t= ");
                strtok(NULL, " ");
                value = strtok(NULL, "\"");
                if ((name == NULL) || (value == NULL))
                    continue;

                if (!strcmp(name, "picsimlab_remotecp")) {
                    sscanf(value, "%hu", &remotec_port);
                }

                if (!strcmp(name, "picsimlab_debugp")) {
                    sscanf(value, "%hu", &debug_port);
                }
            }
        }
    }

#ifndef __EMSCRIPTEN__

    while (rcontrol_init(remotec_port + Instance)) {
        Instance++;
        if (Instance > 100) {
            printf("PICSimLab: Instance error\n");
            exit(-1);
        }
    }
#endif
}

void CPICSimLab::Set_mcudbg(int pd) {
    mcudbg = pd;

    if (mcudbg) {
        JUMPSTEPS = 0;
    } else {
        JUMPSTEPS = DEFAULTJS;
    }
}

void CPICSimLab::SetDebugPort(unsigned short dp) {
    debug_port = dp;
}

void CPICSimLab::SetRemotecPort(unsigned short rcp) {
    remotec_port = rcp;
#ifndef __EMSCRIPTEN__
    rcontrol_end();
    rcontrol_init(remotec_port);
#endif
}

int CPICSimLab::GetDebugStatus(void) {
    return debug;
}

void CPICSimLab::SetDebugStatus(int dbs) {
    debug = dbs;
}

void CPICSimLab::UpdateStatus(const PICSimlabStatus field, const std::string msg) {
    if ((OnUpdateStatus) && (field < PS_LAST)) {
        (*OnUpdateStatus)(field, msg);
    }
}

void CPICSimLab::ConfigMenuGUI(const PICSimlabGUIMenu type) {
    if (OnConfigMenuGUI) {
        (*OnConfigMenuGUI)(type);
    }
}

int CPICSimLab::CanvasCmd(const CanvasCmd_t cmd) {
    if (PICSimLab.OnCanvasCmd) {
        return (*PICSimLab.OnCanvasCmd)(cmd);
    }
    return -1;
}

void CPICSimLab::SetClock(const float clk, const int update) {
    if ((OnClockSet)) {
        (*OnClockSet)(clk, update);
    }
    NSTEP = (int)(clk * NSTEPKT);
    NSTEPJ = NSTEP / JUMPSTEPS;
    pboard->MSetFreq(NSTEP * NSTEPKF);
}

float CPICSimLab::GetClock(void) {
    return pboard->MGetFreq() / 1000000.0;
}

void CPICSimLab::SavePrefs(std::string name, std::string value) {
    char line[1024];
    char* pname;
    char* pvalue;

    for (int lc = 0; lc < (int)prefs.size(); lc++) {
        strncpy(line, prefs.at(lc).c_str(), 1023);

        pname = strtok(line, "\t= ");
        strtok(NULL, " ");
        pvalue = strtok(NULL, "\"");

        if ((pname == NULL) || (pvalue == NULL))
            continue;

        if (std::string(pname) == name) {
            prefs[lc] = name + "\t= \"" + value + "\"";

            return;
        }
    }
    prefs.push_back(name + "\t= \"" + value + "\"");
}

void CPICSimLab::OpenLoadHexFileDialog(void) {
    if (OnOpenLoadHexFileDialog) {
        (*OnOpenLoadHexFileDialog)();
    }
}

void CPICSimLab::SetNeedReboot(int nr) {
#ifndef __EMSCRIPTEN__
    NeedReboot = nr;
#endif
}

void CPICSimLab::RegisterError(const std::string error) {
    Errors.push_back(error);
}

void CPICSimLab::EndSimulation(int saveold, const char* newpath) {
    char home[1024];
    char fname[1280];

#ifndef __EMSCRIPTEN__
    if (Workspacefn.length() > 0) {
        if (saveold) {
            const int labt = lab;
            lab = lab_;
            SaveWorkspace(Workspacefn);
            lab = labt;
        } else {
            SaveWorkspace(Workspacefn);
        }
        Workspacefn = "";
    }
#endif

    SetSimulationRun(1);

#ifndef __EMSCRIPTEN__
    rcontrol_end();
#endif

    if (OnEndSimulation) {
        (*OnEndSimulation)();
    }

    // write options
    SystemCmd(PSC_GETUSERDATADIR, "picsimlab", home);

    SystemCmd(PSC_CREATEDIR, home);

    if (Instance) {
        sprintf(fname, "%s/picsimlab_%i.ini", home, Instance);
    } else {
        sprintf(fname, "%s/picsimlab.ini", home);
    }

    SavePrefs("picsimlab_version", _VERSION_);
    SavePrefs("picsimlab_lab", boards_list[lab].name_);
    SavePrefs("picsimlab_debug", std::to_string(GetDebugStatus()));
    SavePrefs("picsimlab_debugt", std::to_string(GetDebugType()));
    SavePrefs("picsimlab_debugp", std::to_string(GetDebugPort()));
    SavePrefs("picsimlab_remotecp", std::to_string(GetRemotecPort()));
    if (OnSavePrefs) {
        (*OnSavePrefs)();
    }
    SavePrefs("picsimlab_scale", std::to_string(scale));
    SavePrefs("picsimlab_dsr_reset", std::to_string(GetUseDSRReset()));
    SavePrefs("osc_on", std::to_string(pboard->GetUseOscilloscope()));
    SavePrefs("spare_on", std::to_string(pboard->GetUseSpareParts()));
#ifndef _WIN_
    SavePrefs("picsimlab_lser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    SavePrefs("picsimlab_lprog", PROGDEVICE);
#endif
#else
    SavePrefs("picsimlab_wser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    SavePrefs("picsimlab_wprog", PROGDEVICE);
#endif
#endif

    if (PATH.size() > 0) {
        SavePrefs("picsimlab_lpath", PATH);
    } else {
        SavePrefs("picsimlab_lpath", " ");
    }
    SavePrefs("picsimlab_lfile", FNAME);

    pboard->WritePreferences();

    Oscilloscope.WritePreferences();
    SpareParts.WritePreferences();

    PrefsSaveToFile(fname);

    // write memory
    if (Instance) {
        sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[lab_].name_, (const char*)proc_.c_str(), Instance);
    } else {
        sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[lab_].name_, (const char*)proc_.c_str());
    }
    printf("PICSimLab: Saving \"%s\"\n", fname);
    pboard->MDumpMemory(fname);

    pboard->MEnd();

    if (Instance) {
        sprintf(fname, "%s/parts_%s_%i.pcf", home, boards_list[lab_].name_, Instance);
    } else {
        sprintf(fname, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
    }
    if (SpareParts.GetLoadConfigFile().length() == 0) {  // Save only if spare parts window is created
        SpareParts.SaveConfig(fname);
        SpareParts.DeleteParts();
    }
    if (Instance) {
        sprintf(fname, "%s/palias_%s_%i.ppa", home, boards_list[lab_].name_, Instance);
    } else {
        sprintf(fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
    }
    SpareParts.SavePinAlias(fname);

    scale = 1.0;

    if (GetNeedReboot()) {
        char cmd[1024];
        printf("PICSimLab: Reboot !!!\n");
        rcontrol_server_end();
        pboard->EndServers();
        DeleteBoard();
        SystemCmd(PSC_GETEXECUTABLEPATH, NULL, cmd);
        if (newpath) {
            if (strstr(newpath, ".pzw")) {
                strcat(cmd, " \"");
                strcat(cmd, newpath);
                strcat(cmd, "\"");
            } else {
                strcat(cmd, " ");
                strcat(cmd, newpath);
            }
        }
        printf("PICSimLab: Run cmd: %s\n", cmd);

        if (strlen(pzwtmpdir)) {
            SystemCmd(PSC_REMOVEDIR, pzwtmpdir);
        }

        printf("PICSimLab: End Board Simulation.\n");
#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
        fflush(stdout);
        freopen(NULLFILE, "w", stdout);
        fflush(stderr);
        freopen(NULLFILE, "w", stderr);
        char fname[1200];
        snprintf(fname, 1199, "%s/picsimlab_log%i.txt", (const char*)HOME.c_str(), Instance);
        FILE* flog;
        flog = fopen_UTF8(fname, "a");
        if (flog) {
            fprintf(flog, "PICSimLab: Finish Ok\n");
            fclose(flog);
        }
#else
        printf("PICSimLab: Finish Ok\n");
        fflush(stdout);
#endif
        SystemCmd(PSC_EXECUTE, cmd);
        exit(0);
    } else {
        printf("PICSimLab: End Board Simulation.\n");
    }

    Oscilloscope.WindowCmd(PW_MAIN, NULL, PWA_WINDOWHIDE, NULL);
    SpareParts.WindowCmd(PW_MAIN, NULL, PWA_WINDOWHIDE, NULL);

    DeleteBoard();
}

// legacy format support before 0.8.2
static const char old_board_names[6][20] = {"Breadboard", "McLab1", "K16F", "McLab2", "PICGenios", "Arduino_Uno"};

void CPICSimLab::LoadWorkspace(std::string fnpzw, const int show_readme) {
    char home[1024];
    char fzip[1280];

    if (!SystemCmd(PSC_FILEEXISTS, fnpzw.c_str())) {
        printf("PICSimLab: file %s not found!\n", (const char*)fnpzw.c_str());
        RegisterError("PICSimLab: file " + fnpzw + " not found!");
        return;
    }
    if (fnpzw.find(".pzw") == std::string::npos) {
        printf("PICSimLab: file %s is not a .pzw file!\n", (const char*)fnpzw.c_str());
        RegisterError("PICSimLab: file " + fnpzw + " is not a .pzw file!");
        return;
    }
    // write options

    if (strlen(pzwtmpdir)) {
        SystemCmd(PSC_REMOVEDIR, pzwtmpdir);
    }

    char btdir[256];
    SystemCmd(PSC_GETTEMPDIR, "PICSimLab", btdir);
    snprintf(pzwtmpdir, 1023, "%s/picsimlab-XXXXXX", btdir);
    close(mkstemp(pzwtmpdir));
    unlink(pzwtmpdir);
    SystemCmd(PSC_CREATEDIR, pzwtmpdir);

    memcpy(fzip, pzwtmpdir, 1023);
    strncat(fzip, "/", 1023);

    memcpy(home, pzwtmpdir, 1023);
    strncat(home, "/picsimlab_workspace/", 1023);

    SystemCmd(PSC_UNZIPDIR, fnpzw.c_str(), fzip);

    EndSimulation(0, fnpzw.c_str());

    SetWorkspaceFileName(fnpzw);

    snprintf(fzip, 1279, "%s/picsimlab.ini", home);
    std::vector<std::string> prefsw;
    prefsw.clear();
    int lc;
    char* value;
    char* name;
#ifdef LEGACY081
    char name_[100];
    char value_[400];
    int llab = 0;
#endif

    char line[1024];
    if (LoadFromFile(prefsw, fzip)) {
        for (lc = 0; lc < (int)prefsw.size(); lc++) {
            strncpy(line, prefsw.at(lc).c_str(), 1023);
            name = strtok(line, "\t= ");
            strtok(NULL, " ");
            value = strtok(NULL, "\"");
            if ((name == NULL) || (value == NULL))
                continue;

            if (!strcmp(name, "picsimlab_version")) {
                int maj, min, rev, ser;
                int maj_, min_, rev_, ser_;

                pzw_ver = value;

                sscanf(_VERSION_, "%i.%i.%i", &maj, &min, &rev);
                ser = maj * 10000 + min * 100 + rev;

                sscanf(value, "%i.%i.%i", &maj_, &min_, &rev_);
                ser_ = maj_ * 10000 + min_ * 100 + rev_;

                if (ser_ > ser) {
                    printf("PICSimLab: .pzw file version %i newer than PICSimLab %i!\n", ser_, ser);
                    RegisterError("The loaded workspace was made with a newer version " + pzw_ver +
                                  ".\n Please update your PICSimLab " _VERSION_ " .");
                }

                continue;
            }
#ifndef LEGACY081
            SaveP Prefs(name, value);
#else
            strncpy(name_, name, 99);
            strncpy(value_, value, 399);

            // legacy mode compatibility
            if (!strcmp(name_, "lab")) {
                char oldname[1500];

                strcpy(name_, "picsimlab_lab");
                sscanf(value_, "%i", &llab);

                strcpy(value_, old_board_names[llab]);

                snprintf(oldname, 1499, "%sparts_%02i.pcf", home, llab);

                if (SystemCmd(PSC_FILEEXISTS, oldname)) {
                    char newname[1500];
                    snprintf(newname, 1499, "%sparts_%s.pcf", home, old_board_names[llab]);
                    SystemCmd(PSC_RENAMEFILE, oldname, newname);
                }
            }
            if (!strcmp(name_, "debug")) {
                strcpy(name_, "picsimlab_debug");
            }
            if (!strcmp(name_, "position")) {
                strcpy(name_, "picsimlab_position");
            }
            if (!strcmp(name_, "lfile")) {
                strcpy(name_, "picsimlab_lfile");
            }
            if (!strcmp(name_, "clock")) {
                sprintf(name_, "%s_clock", old_board_names[llab]);
            }
            if (!strcmp(name_, "p0_proc")) {
                sprintf(name_, "%s_proc", old_board_names[0]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_00_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[0], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }
            if (!strcmp(name_, "p1_proc")) {
                sprintf(name_, "%s_proc", old_board_names[1]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_01_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[1], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }
            if (!strcmp(name_, "p2_proc")) {
                sprintf(name_, "%s_proc", old_board_names[2]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_02_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[2], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }
            if (!strcmp(name_, "p3_proc")) {
                sprintf(name_, "%s_proc", old_board_names[3]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_03_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[3], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }
            if (!strcmp(name_, "p4_proc")) {
                sprintf(name_, "%s_proc", old_board_names[4]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_04_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[4], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }
            if (!strcmp(name_, "p5_proc")) {
                sprintf(name_, "%s_proc", old_board_names[5]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_05_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[5], value);
                SystemCmd(PSC_RENAMEFILE, oldname, newname);
            }

            char* ptr;
            if ((ptr = strstr(name, "p0_"))) {
                sprintf(name_, "%s_%s", old_board_names[0], ptr + 3);
            }
            if ((ptr = strstr(name, "p1_"))) {
                sprintf(name_, "%s_%s", old_board_names[1], ptr + 3);
            }
            if ((ptr = strstr(name, "p2_"))) {
                sprintf(name_, "%s_%s", old_board_names[2], ptr + 3);
            }
            if ((ptr = strstr(name, "p3_"))) {
                sprintf(name_, "%s_%s", old_board_names[3], ptr + 3);
            }
            if ((ptr = strstr(name, "p4_"))) {
                sprintf(name_, "%s_%s", old_board_names[4], ptr + 3);
            }
            if ((ptr = strstr(name, "p5_"))) {
                sprintf(name_, "%s_%s", old_board_names[5], ptr + 3);
            }

            SavePrefs(name_, value_);
#endif
        }
    }
    PrefsSaveToFile(fzip);

    Configure(home, 0, 0, NULL, 1);

    need_resize = 1;

#ifdef CONVERTER_MODE
    fnpzw.replace(fnpzw.Length() - 4, 5, "_.pzw");
    cvt_fname = fnpzw;
#else  // CONVERTER_MODE
    if (show_readme) {
        snprintf(fzip, 1279, "%s/Readme.html", home);
        if (SystemCmd(PSC_FILEEXISTS, fzip)) {
#ifndef __EMSCRIPTEN__
#ifdef EXT_BROWSER
            SystemCmd(PSC_LAUNCHDEFAULTBROWSER, ("file://" + std::string(fzip)).c_str());
#else   // EXT_BROWSER
            Window2.html1.SetLoadFile(fzip);
            Window2.Show();
#endif  // EXT_BROWSER
#endif  //__EMSCRIPTEN__
        } else {
            snprintf(fzip, 1279, "%s/Readme.txt", home);
            if (SystemCmd(PSC_FILEEXISTS, fzip)) {
#ifndef __EMSCRIPTEN__
#ifdef EXT_BROWSER
                SystemCmd(PSC_LAUNCHDEFAULTBROWSER, ("file://" + std::string(fzip)).c_str());
#else   // EXT_BROWSER
                Window2.html1.SetLoadFile(fzip);
                Window2.Show();
#endif  // EXT_BROWSER
#endif  //__EMSCRIPTEN__
            }
        }
    }
#endif  // CONVERTER_MODE
}

void CPICSimLab::SaveWorkspace(std::string fnpzw) {
    char tmpdir[1024];
    char home[1024];
    char fname[1280];

#if !defined(__EMSCRIPTEN__) && !defined(CONVERTER_MODE)
    if (SystemCmd(PSC_FILEEXISTS, fnpzw.c_str())) {
        char bname[512];
        SystemCmd(PSC_BASENAME, fnpzw.c_str(), bname);
        if (!SystemCmd(PSC_SHOWDIALOG, (std::string("Overwriting file: ") + bname + "?").c_str()))
            return;
    }
#endif

    SetWorkspaceFileName(fnpzw);

    // write options

    SystemCmd(PSC_GETUSERDATADIR, "picsimlab", home);
    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PrefsSaveToFile(fname);

    char btdir[256];
    SystemCmd(PSC_GETTEMPDIR, "PICSimLab", btdir);
    snprintf(tmpdir, 1023, "%s/picsimlab-XXXXXX", btdir);
    close(mkstemp(tmpdir));
    unlink(tmpdir);
    SystemCmd(PSC_CREATEDIR, tmpdir);

    memcpy(home, tmpdir, 1023);
    strncat(home, "/picsimlab_workspace/", 1023);

#ifdef CONVERTER_MODE
    snprintf(fname, 1279, "rm -rf %s/*.ini", home);
    system(fname);
    snprintf(fname, 1279, "rm -rf %s/*.pcf", home);
    system(fname);
    snprintf(fname, 1279, "rm -rf %s/*.hex", home);
    system(fname);
#else
    SystemCmd(PSC_CREATEDIR, home);
#endif

    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PrefsClear();
    SavePrefs("picsimlab_version", _VERSION_);
    SavePrefs("picsimlab_lab", boards_list[lab].name_);
    SavePrefs("picsimlab_debug", std::to_string(GetDebugStatus()));
    SavePrefs("picsimlab_debugt", std::to_string(GetDebugType()));
    SavePrefs("picsimlab_debugp", std::to_string(GetDebugPort()));
    if (OnSavePrefs) {
        (*OnSavePrefs)();
    }
    SavePrefs("picsimlab_scale", std::to_string(scale));
    SavePrefs("osc_on", std::to_string(pboard->GetUseOscilloscope()));
    SavePrefs("spare_on", std::to_string(pboard->GetUseSpareParts()));
    SavePrefs("picsimlab_lfile", " ");

    pboard->WritePreferences();

    if (pboard->GetUseOscilloscope())
        Oscilloscope.WritePreferences();

    if (pboard->GetUseSpareParts()) {
        SpareParts.WritePreferences();
    }

    PrefsSaveToFile(fname);

    // write memory

    snprintf(fname, 1279, "%s/mdump_%s_%s.hex", home, boards_list[lab_].name_, (const char*)proc_.c_str());

    printf("PICSimLab: Saving \"%s\"\n", fname);
    pboard->MDumpMemory(fname);

    // write spare part config
    snprintf(fname, 1279, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
    SpareParts.SaveConfig(fname);
    sprintf(fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
    SpareParts.SavePinAlias(fname);

    SystemCmd(PSC_ZIPDIR, home, (void*)fnpzw.c_str());

    SystemCmd(PSC_REMOVEDIR, tmpdir);

    SystemCmd(PSC_GETUSERDATADIR, "picsimlab", home);
    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PrefsClear();
    PrefsLoadFromFile(fname);

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
        fnpzw.c_str());
#endif

#ifdef CONVERTER_MODE
    WDestroy();
#endif
}

void CPICSimLab::SetSimulationRun(int run) {
    if (run) {
        status &= ~ST_DI;
    } else {
        status |= ST_DI;
    }
}

int CPICSimLab::GetSimulationRun(void) {
    return (status & ST_DI) == 0;
}

void CPICSimLab::Configure(const char* home, int use_default_board, int create, const char* lfile,
                           const int disable_debug) {
    char line[1024];
    char fname[2048];
    char fname_[2048];

    char* name;
    char* value;

    int i;
    int lc;
    int load_demo = 0;

    int osc_on = 0;
    int spare_on = 0;

    std::string status;

    if (Instance && !HOME.compare(home)) {
        snprintf(fname, 1023, "%s/picsimlab_%i.ini", home, Instance);
    } else {
        snprintf(fname, 1023, "%s/picsimlab.ini", home);
    }
    SERIALDEVICE[0] = ' ';
    SERIALDEVICE[1] = 0;
#ifdef _USE_PICSTARTP_
    PROGDEVICE[0] = ' ';
    PROGDEVICE[1] = 0;
#endif
    DeleteBoard();

    PrefsClear();
    if (SystemCmd(PSC_FILEEXISTS, fname)) {
        printf("PICSimLab: Load Config from file \"%s\"\n", fname);
        if (PrefsLoadFromFile(fname)) {
            for (lc = 0; lc < (int)PrefsGetLinesCount(); lc++) {
                strncpy(line, PrefsGetLine(lc).c_str(), 1023);

                name = strtok(line, "\t= ");
                strtok(NULL, " ");
                value = strtok(NULL, "\"");
                if ((name == NULL) || (value == NULL))
                    continue;
#ifndef _WIN_
                if (!strcmp("picsimlab_lser", name))
                    strcpy(SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_
                if (!strcmp("picsimlab_lprog", name))
                    strcpy(PROGDEVICE, value);
#endif
#else
                if (!strcmp("picsimlab_wser", name))
                    strcpy(SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_
                if (!strcmp("picsimlab_wprog", name))
                    strcpy(PROGDEVICE, value);
#endif
#endif

                if (!strcmp(name, "picsimlab_lab")) {
                    if (use_default_board) {
                        SetLabs(DEFAULT_BOARD, DEFAULT_BOARD);
                    } else {
                        for (i = 0; i < BOARDS_LAST; i++) {
                            if (!strcmp(boards_list[i].name_, value)) {
                                break;
                            }
                        }
                        SetLabs(i, i);
                        if (lab == BOARDS_LAST) {
                            RegisterError(std::string("Invalid board ") + value + "!\n Using default board!");
                        }
                    }
                    SetBoard(create_board(&lab, &lab_));
                    if (pboard->GetScale() < GetScale()) {
                        SetScale(pboard->GetScale());
                    } else {
                        pboard->SetScale(GetScale());
                    }
                    SetJUMPSTEPS(DEFAULTJS);
                    SetClock(pboard->GetDefaultClock());
                }

                if (!strcmp(name, "picsimlab_debug")) {
                    int debug = 0;
#ifndef NO_DEBUG
                    sscanf(value, "%i", &debug);
                    if (disable_debug) {
                        debug = 0;
                    }
#endif
                    SetDebugStatus(debug);
                }

                if (!strcmp(name, "picsimlab_debugt")) {
                    int debug_type;
                    sscanf(value, "%i", &debug_type);
                    SetDebugType(debug_type);
                }

                if (!strcmp(name, "osc_on")) {
                    sscanf(value, "%i", &osc_on);
                }

                if (!strcmp(name, "spare_on")) {
                    sscanf(value, "%i", &spare_on);
                }

                if (!strcmp(name, "picsimlab_scale")) {
                    if (create) {
                        double s;
                        sscanf(value, "%lf", &s);
                        SetScale(s);
                        pboard->SetScale(GetScale());
                        printf("PICSimLab: Window scale %5.2f \n", GetScale());
                    }
                }

                if (!strcmp(name, "picsimlab_dsr_reset")) {
                    sscanf(value, "%i", &use_dsr_reset);
                }

                if (!strcmp(name, "picsimlab_lpath")) {
                    SetPath(std::string(value));
                }

                if (!strcmp(name, "picsimlab_lfile")) {
                    SetFNAME(std::string(value));
                }

                if (pboard) {
                    pboard->ReadPreferences(name, value);
                }

                if (OnReadPreferences) {
                    (*OnReadPreferences)(name, value, create);
                }

                Oscilloscope.ReadPreferences(name, value);
                SpareParts.ReadPreferences(name, value);
            }
        }
    }

    if (pboard == NULL) {
        printf("PICSimLab: Error open config file \"%s\"!\n", fname);

        SetLabs(DEFAULT_BOARD, DEFAULT_BOARD);  // default
        SetBoard(boards_list[lab].bcreate());
        SetJUMPSTEPS(DEFAULTJS);
        SetClock(pboard->GetDefaultClock());

#ifndef _WIN_
        strcpy(SERIALDEVICE, "/dev/tnt2");
#ifdef _USE_PICSTARTP_
        strcpy(PROGDEVICE, "/dev/tnt4");
#endif
#else
        strcpy(SERIALDEVICE, "COM6");
#ifdef _USE_PICSTARTP_
        strcpy(PROGDEVICE, "COM8");
#endif
#endif
    }

    /*
    if ((OnConfigure)) {
        (*OnConfigure)();
    }
    */

    pboard->MSetSerial(SERIALDEVICE);

    if (lfile) {
        if (SystemCmd(PSC_FILEEXISTS, lfile)) {
            strcpy(fname, lfile);
        } else {
            printf("PICSimLab: File Not found \"%s\" loading default.\n", lfile);
            RegisterError(std::string("File Not found \n\"") + lfile + "\"\n loading default.");
            if (Instance && !HOME.compare(home)) {
                sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[lab].name_,
                        (const char*)pboard->GetProcessorName().c_str(), Instance);
            } else {
                sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[lab].name_,
                        (const char*)pboard->GetProcessorName().c_str());
            }
        }
    } else {
        if (Instance && !HOME.compare(home)) {
            sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[lab].name_,
                    (const char*)pboard->GetProcessorName().c_str(), Instance);
        } else {
            sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[lab].name_,
                    (const char*)pboard->GetProcessorName().c_str());
        }
    }

    printf("PICSimLab: Using board \"%s\"\n", boards_list[lab].name);
    printf("PICSimLab: Instance number %i\n", Instance);
    printf("PICSimLab: Opening \"%s\"\n", fname);

    // change .hex to .bin
    strncpy(fname_, fname, 2048);
    fname_[strlen(fname_) - 3] = 0;
    strncat(fname_, "bin", 2047);

    if (!((SystemCmd(PSC_FILEEXISTS, fname)) || (SystemCmd(PSC_FILEEXISTS, fname_)))) {
        printf("PICSimLab: File not found! Creating new empty file. \n");
        if (!pboard->GetDefaultProcessor().compare(pboard->GetProcessorName())) {
            load_demo = 1;
        }
    }

    switch (pboard->MInit(pboard->GetProcessorName().c_str(), fname, GetNSTEP() * NSTEPKF)) {
        // case HEX_NFOUND:
        //     break;
        case HEX_CHKSUM:
            printf("PICSimLab: File checksum error!\n");
            pboard->MEraseFlash();
            break;
    }

    if ((OnConfigure)) {
        (*OnConfigure)();
    } else {
        pboard->Reset();
    }

    SetProcessorName(pboard->GetProcessorName());

#ifdef _USE_PICSTARTP_
    if (prog_init() >= 0)
        status = "PStart:  Ok ";
    else
        status = "PStart:Error";
#else
    status = "";
#endif

    UpdateStatus(PS_RUN, "Running...");

    WindowCmd(PW_MAIN, NULL, PWA_APPPROCESSEVENTS, NULL);

    Oscilloscope.SetBoard(pboard);
    Oscilloscope.SetBaseTimer();

    pboard->SetUseOscilloscope(osc_on);
    pboard->SetUseSpareParts(spare_on);

    if (Instance && !HOME.compare(home)) {
        sprintf(fname, "%s/parts_%s_%i.pcf", home, boards_list[lab].name_, Instance);
    } else {
        sprintf(fname, "%s/parts_%s.pcf", home, boards_list[lab].name_);
    }
    SpareParts.LoadConfig(fname, disable_debug);
    if (Instance && !HOME.compare(home)) {
        sprintf(fname, "%s/palias_%s_%i.ppa", home, boards_list[lab_].name_, Instance);
    } else {
        sprintf(fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
    }
    SpareParts.LoadPinAlias(fname);

    printf("PICSimLab: Debug On=%i  Type=%s Port=%i\n", debug, (debug_type) ? "GDB" : "MDB", GetDebugPort());

#ifdef NO_DEBUG
    UpdateStatus(PS_DEBUG, " ");
#else
    if (GetDebugStatus()) {
        int ret = pboard->DebugInit(GetDebugType());
        if (ret < 0) {
            UpdateStatus(PS_DEBUG, status + "Debug: Error");
        } else {
            UpdateStatus(PS_DEBUG, status + "Debug: " + pboard->GetDebugName() + ":" + std::to_string(GetDebugPort()));
        }
    } else {
        UpdateStatus(PS_DEBUG, status + "Debug: Off");
    }
#endif

#ifndef __EMSCRIPTEN__
    printf("PICSimLab: Remote Control Port %i\n", GetRemotecPort());
    rcontrol_init(GetRemotecPort() + Instance);
#endif

    if (load_demo) {
        std::string fdemo =
            PICSimLab.GetSharePath() + "boards/" + std::string(boards_list[PICSimLab.GetLab()].name) + "/demo.pzw";

        if (SystemCmd(PSC_FILEEXISTS, fdemo.c_str())) {
            printf("PICSimLab: Loading board demonstration code.\n");
            PICSimLab.LoadWorkspace(fdemo, 0);
            PICSimLab.SetWorkspaceFileName("");
        }
    }
}

double CPICSimLab::GetIdleMs(void) {
    return idle_ms;
}

void CPICSimLab::SetIdleMs(double im) {
    idle_ms = im;
}

void CPICSimLab::SetToDestroy(int reason) {
    settodestroy = reason;
}

int CPICSimLab::LoadHexFile(std::string fname) {
    int pa;
    int ret = 0;

    pa = GetMcuPwr();
    SetMcuPwr(0);
    while (PICSimLab.status & ST_TH)
        usleep(100);  // wait thread

    status |= ST_DI;
    msleep(BASETIMER);
    if (tgo)
        tgo = 1;
    while (status & (ST_TH | ST_T1)) {
        msleep(1);
        WindowCmd(PW_MAIN, NULL, PWA_APPPROCESSEVENTS, NULL);
    }

    if (GetNeedReboot()) {
        char cmd[4096];
        sprintf(cmd, " %s %s \"%s\"", boards_list[GetLab()].name_, (const char*)GetBoard()->GetProcessorName().c_str(),
                (const char*)fname.c_str());
        EndSimulation(0, cmd);
    }

    GetBoard()->MEnd();
    GetBoard()->MSetSerial(SERIALDEVICE);

    switch (GetBoard()->MInit(GetBoard()->GetProcessorName().c_str(), fname.c_str(), GetNSTEP() * NSTEPKF)) {
        case HEX_NFOUND:
            RegisterError("Hex file not found!");
            SetMcuRun(0);
            break;
        case HEX_CHKSUM:
            RegisterError("Hex file checksum error!");
            GetBoard()->MEraseFlash();
            SetMcuRun(0);
            break;
        case 0:
            SetMcuRun(1);
            break;
    }

    GetBoard()->Reset();

    if (OnLoadHexFile) {
        (*OnLoadHexFile)(fname);
    }

    ret = !GetMcuRun();

    SetMcuPwr(pa);
    status &= ~ST_DI;

#ifdef NO_DEBUG
    UpdateStatus(PS_DEBUG, " ");
#else
    if (GetDebugStatus()) {
        int ret = GetBoard()->DebugInit(GetDebugType());
        if (ret < 0) {
            UpdateStatus(PS_DEBUG, "Debug: Error");
        } else {
            UpdateStatus(PS_DEBUG, "Debug: " + GetBoard()->GetDebugName() + ":" +
                                       std::to_string(GetDebugPort() + GetInstanceNumber()));
        }
    } else {
        UpdateStatus(PS_DEBUG, "Debug: Off");
    }
#endif

    return ret;
}

void* CPICSimLab::UpdateGUI(const int id, const PICSimlabGUIType type, const PICSimlabGUIAction action,
                            const void* arg) {
    if (OnUpdateGUI) {
        return (*OnUpdateGUI)(id, type, action, arg);
    }
    return NULL;
}

int CPICSimLab::WindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action, const char* Value,
                          void* ReturnBuff) {
    if (PICSimLab.OnWindowCmd) {
        return (*PICSimLab.OnWindowCmd)(id, ControlName, action, Value, ReturnBuff);
    } else {
        printf("Error: CPICSimLab::WindowCmd missing !\n");
    }
    return -1;
}

int CPICSimLab::SystemCmd(const PICSimLabSystemCmd cmd, const char* Arg, void* ReturnBuff) {
    if (PICSimLab.OnSystemCmd) {
        return (*PICSimLab.OnSystemCmd)(cmd, Arg, ReturnBuff);
    } else {
        printf("Error: CPICSimLab::SystemCmd missing !\n");
    }
    return -1;
}
