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

#include "picsimlab.h"
#include "oscilloscope.h"
#include "spareparts.h"

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
    Window = NULL;
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
    Errors.Clear();
    NeedReboot = 0;
    lab = DEFAULT_BOARD;
    lab_ = DEFAULT_BOARD;
    Workspacefn = "";
    status.status = 0;
    scale = 1.0;
    need_resize = 0;
    tgo = 0;
    plWidth = 10;
    plHeight = 10;
    need_clkupdate = 0;
    use_dsr_reset = 1;
    settodestroy = 0;
    sync = 0;

#ifndef _NOTHREAD
    cpu_mutex = NULL;
    cpu_cond = NULL;
#endif

    menu_EvBoard = NULL;
    menu_EvMicrocontroller = NULL;
    board_Event = NULL;
    board_ButtonEvent = NULL;
}

void CPICSimLab::Init(CWindow* w) {
    Window = w;
    statusbar = (CStatusbar*)Window->GetChildByName("statusbar1");

    // check for other instances
    StartRControl();

    // board menu
    for (int i = 0; i < BOARDS_LAST; i++) {
        MBoard[i].SetFOwner(Window);
        MBoard[i].SetName(itoa(i));
        MBoard[i].SetText(boards_list[i].name);
        MBoard[i].EvMenuActive = menu_EvBoard;
        Window->GetChildByName("menu1")->GetChildByName("menu1_Board")->CreateChild(&MBoard[i]);
        //((CPMenu*)Window->GetChildByName("menu1_Board"))->CreateChild(&MBoard[i]);
    }
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

void CPICSimLab::SetCpuState(const unsigned char cs) {
    cpustate = cs;
}

void CPICSimLab::StartRControl(void) {
    char line[1024];
    char fname[1024];
    char* name;
    char* value;

    int lc;

    lxString status;

    mcurun = 1;
    mcupwr = 1;
    mcurst = 0;

    snprintf(fname, 1023, "%s/picsimlab.ini", (const char*)HOME.c_str());

    prefs.Clear();
    if (lxFileExists(fname)) {
        if (prefs.LoadFromFile(fname)) {
            for (lc = 0; lc < (int)prefs.GetLinesCount(); lc++) {
                strncpy(line, prefs.GetLine(lc).c_str(), 1023);

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
    if (debug) {
        CToggleButton* togglebutton = (CToggleButton*)Window->GetChildByName("togglebutton1");
        togglebutton->SetCheck(0);
        (Window->*(togglebutton->EvOnToggleButton))(NULL);
    }
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

void CPICSimLab::SetDebugStatus(int dbs, int updatebtn) {
    if (debug != dbs) {
        debug = dbs;
        if (updatebtn) {
            CToggleButton* togglebutton = (CToggleButton*)Window->GetChildByName("togglebutton1");
            (Window->*(togglebutton->EvOnToggleButton))(NULL);
        }
    }
}

void CPICSimLab::SetClock(const float clk, const int update) {
    CCombo* combo = (CCombo*)Window->GetChildByName("combo1");

    if (update) {
        if (clk < 1) {
            combo->SetText(lxString().Format("%2.1f", clk));
        } else {
            combo->SetText(lxString().Format("%2.0f", clk));
        }
        need_clkupdate = 0;
    } else {
        need_clkupdate = 1;
    }

    NSTEP = (int)(clk * NSTEPKT);
    NSTEPJ = NSTEP / JUMPSTEPS;
    pboard->MSetFreq(NSTEP * NSTEPKF);
}

float CPICSimLab::GetClock(void) {
    return pboard->MGetFreq() / 1000000.0;
}

void CPICSimLab::SavePrefs(lxString name, lxString value) {
    char line[1024];
    char* pname;
    char* pvalue;

    for (int lc = 0; lc < (int)prefs.GetLinesCount(); lc++) {
        strncpy(line, prefs.GetLine(lc).c_str(), 1023);

        pname = strtok(line, "\t= ");
        strtok(NULL, " ");
        pvalue = strtok(NULL, "\"");

        if ((pname == NULL) || (pvalue == NULL))
            continue;

        if (lxString(pname) == name) {
            prefs.SetLine(name + lxT("\t= \"") + value + lxT("\""), lc);

            return;
        }
    }
    prefs.AddLine(name + lxT("\t= \"") + value + lxT("\""));
}

void CPICSimLab::OpenLoadHexFileDialog(void) {
    CItemMenu* imenu = (CItemMenu*)Window->GetChildByName("menu1_File_LoadHex");
    (Window->*(imenu->EvMenuActive))(NULL);
}

void CPICSimLab::SetNeedReboot(int nr) {
#ifndef __EMSCRIPTEN__
    NeedReboot = nr;
#endif
}

void CPICSimLab::RegisterError(const lxString error) {
    Errors.AddLine(error);
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
    ((CTimer*)Window->GetChildByName("timer1"))->SetRunState(0);
    ((CTimer*)Window->GetChildByName("timer2"))->SetRunState(0);
    msleep(100);
    while (status.status) {
        msleep(1);
        Application->ProcessEvents();
    }
    tgo = 100000;
#ifndef _NOTHREAD
    cpu_mutex->Lock();
    cpu_cond->Signal();
    cpu_mutex->Unlock();
#endif
    ((CThread*)Window->GetChildByName("thread1"))->Destroy();
    tgo = 0;

#ifndef __EMSCRIPTEN__
    ((CThread*)Window->GetChildByName("thread2"))->Destroy();
#endif

    // write options
    strcpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str());

    lxCreateDir(home);

    if (Instance) {
        sprintf(fname, "%s/picsimlab_%i.ini", home, Instance);
    } else {
        sprintf(fname, "%s/picsimlab.ini", home);
    }

    SavePrefs(lxT("picsimlab_version"), _VERSION_);
    SavePrefs(lxT("picsimlab_lab"), boards_list[lab].name_);
    SavePrefs(lxT("picsimlab_debug"), itoa(GetDebugStatus()));
    SavePrefs(lxT("picsimlab_debugt"), itoa(GetDebugType()));
    SavePrefs(lxT("picsimlab_debugp"), itoa(GetDebugPort()));
    SavePrefs(lxT("picsimlab_remotecp"), itoa(GetRemotecPort()));
    SavePrefs(lxT("picsimlab_position"), itoa(Window->GetX()) + lxT(",") + itoa(Window->GetY()));
    SavePrefs(lxT("picsimlab_scale"), ftoa(scale));
    SavePrefs(lxT("picsimlab_dsr_reset"), itoa(GetUseDSRReset()));
    SavePrefs(lxT("osc_on"), itoa(pboard->GetUseOscilloscope()));
    SavePrefs(lxT("spare_on"), itoa(pboard->GetUseSpareParts()));
#ifndef _WIN_
    SavePrefs(lxT("picsimlab_lser"), SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    SavePrefs(lxT("picsimlab_lprog"), PROGDEVICE);
#endif
#else
    SavePrefs("picsimlab_wser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    SavePrefs("picsimlab_wprog", PROGDEVICE);
#endif
#endif

    if (PATH.size() > 0) {
        SavePrefs(lxT("picsimlab_lpath"), PATH);
    } else {
        SavePrefs(lxT("picsimlab_lpath"), " ");
    }
    SavePrefs(lxT("picsimlab_lfile"), FNAME);

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

#ifndef __EMSCRIPTEN__
    if (((CThread*)Window->GetChildByName("thread3"))->GetRunState()) {
        ((CThread*)Window->GetChildByName("thread3"))->Destroy();
    }
#endif

    if (Instance) {
        sprintf(fname, "%s/parts_%s_%i.pcf", home, boards_list[lab_].name_, Instance);
    } else {
        sprintf(fname, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
    }
    SpareParts.SaveConfig(fname);
    SpareParts.DeleteParts();
    if (Instance) {
        sprintf(fname, "%s/palias_%s_%i.ppa", home, boards_list[lab_].name_, Instance);
    } else {
        sprintf(fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
    }
    SpareParts.SavePinAlias(fname);

    // refresh window position to window reopen in same position
    Window->GetX();
    Window->GetY();

    scale = 1.0;

#ifndef _NOTHREAD
    delete cpu_cond;
    delete cpu_mutex;
    cpu_cond = NULL;
    cpu_mutex = NULL;
#endif

    if (GetNeedReboot()) {
        char cmd[1024];
        printf("PICSimLab: Reboot !!!\n");
        rcontrol_server_end();
        pboard->EndServers();
        DeleteBoard();
        strcpy(cmd, lxGetExecutablePath().c_str());
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

        printf("PICSimLab: End Board Simulation.\n");
#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
        fflush(stdout);
        freopen(NULLFILE, "w", stdout);
        fflush(stderr);
        freopen(NULLFILE, "w", stderr);
        char fname[1200];
        snprintf(fname, 1199, "%s/picsimlab_log%i.txt", (const char*)HOME.c_str(), Instance);
        FILE* flog;
        flog = fopen(fname, "a");
        if (flog) {
            fprintf(flog, "PICSimLab: Finish Ok\n");
            fclose(flog);
        }
#else
        printf("PICSimLab: Finish Ok\n");
        fflush(stdout);
#endif
        lxExecute(cmd);
        exit(0);
    } else {
        printf("PICSimLab: End Board Simulation.\n");
    }

    Oscilloscope.GetWindow()->Hide();
    SpareParts.GetWindow()->Hide();
    DeleteBoard();
}

// legacy format support before 0.8.2
static const char old_board_names[6][20] = {"Breadboard", "McLab1", "K16F", "McLab2", "PICGenios", "Arduino_Uno"};

void CPICSimLab::LoadWorkspace(lxString fnpzw, const int show_readme) {
    char home[1024];
    char fzip[1280];

    if (!lxFileExists(fnpzw)) {
        printf("PICSimLab: file %s not found!\n", (const char*)fnpzw.c_str());
        RegisterError("PICSimLab: file " + fnpzw + " not found!");
        return;
    }
    if (!fnpzw.Contains(".pzw")) {
        printf("PICSimLab: file %s is not a .pzw file!\n", (const char*)fnpzw.c_str());
        RegisterError("PICSimLab: file " + fnpzw + " is not a .pzw file!");
        return;
    }
    // write options
    strncpy(fzip, (char*)lxGetTempDir(lxT("picsimlab")).char_str(), 1023);
    strncat(fzip, "/", 1023);

    strncpy(home, (char*)lxGetTempDir(lxT("picsimlab")).char_str(), 1023);
    strncat(home, "/picsimlab_workspace/", 1023);

    lxRemoveDir(home);

    lxUnzipDir(fnpzw, fzip);

    EndSimulation(0, fnpzw.c_str());

    SetWorkspaceFileName(fnpzw);

    snprintf(fzip, 1279, "%s/picsimlab.ini", home);
    lxStringList prefsw;
    prefsw.Clear();
    int lc;
    char* value;
    char* name;
#ifdef LEGACY081
    char name_[100];
    char value_[400];
    int llab = 0;
#endif

    char line[1024];
    if (prefsw.LoadFromFile(fzip)) {
        for (lc = 0; lc < (int)prefsw.GetLinesCount(); lc++) {
            strncpy(line, prefsw.GetLine(lc).c_str(), 1023);
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

                if (lxFileExists(oldname)) {
                    char newname[1500];
                    snprintf(newname, 1499, "%sparts_%s.pcf", home, old_board_names[llab]);
                    lxRenameFile(oldname, newname);
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
                lxRenameFile(oldname, newname);
            }
            if (!strcmp(name_, "p1_proc")) {
                sprintf(name_, "%s_proc", old_board_names[1]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_01_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[1], value);
                lxRenameFile(oldname, newname);
            }
            if (!strcmp(name_, "p2_proc")) {
                sprintf(name_, "%s_proc", old_board_names[2]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_02_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[2], value);
                lxRenameFile(oldname, newname);
            }
            if (!strcmp(name_, "p3_proc")) {
                sprintf(name_, "%s_proc", old_board_names[3]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_03_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[3], value);
                lxRenameFile(oldname, newname);
            }
            if (!strcmp(name_, "p4_proc")) {
                sprintf(name_, "%s_proc", old_board_names[4]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_04_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[4], value);
                lxRenameFile(oldname, newname);
            }
            if (!strcmp(name_, "p5_proc")) {
                sprintf(name_, "%s_proc", old_board_names[5]);

                char oldname[1500];
                char newname[1500];
                snprintf(oldname, 1499, "%smdump_05_%s.hex", home, value);
                snprintf(newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[5], value);
                lxRenameFile(oldname, newname);
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
        if (lxFileExists(fzip)) {
#ifndef __EMSCRIPTEN__
#ifdef EXT_BROWSER
            lxLaunchDefaultBrowser(lxT("file://") + lxString(fzip));
#else   // EXT_BROWSER
            Window2.html1.SetLoadFile(fzip);
            Window2.Show();
#endif  // EXT_BROWSER
#endif  //__EMSCRIPTEN__
        } else {
            snprintf(fzip, 1279, "%s/Readme.txt", home);
            if (lxFileExists(fzip)) {
#ifndef __EMSCRIPTEN__
#ifdef EXT_BROWSER
                lxLaunchDefaultBrowser(lxT("file://") + lxString(fzip));
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

void CPICSimLab::SaveWorkspace(lxString fnpzw) {
    char home[1024];
    char fname[1280];

#if !defined(__EMSCRIPTEN__) && !defined(CONVERTER_MODE)
    if (lxFileExists(fnpzw)) {
        if (!Dialog_sz(lxString("Overwriting file: ") + basename(fnpzw) + "?", 400, 200))
            return;
    }
#endif

    SetWorkspaceFileName(fnpzw);

    // write options

    strncpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str(), 1023);
    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PrefsSaveToFile(fname);

    strncpy(home, (char*)lxGetTempDir(lxT("picsimlab")).char_str(), 1023);
    strncat(home, "/picsimlab_workspace/", 1023);

#ifdef CONVERTER_MODE
    snprintf(fname, 1279, "rm -rf %s/*.ini", home);
    system(fname);
    snprintf(fname, 1279, "rm -rf %s/*.pcf", home);
    system(fname);
    snprintf(fname, 1279, "rm -rf %s/*.hex", home);
    system(fname);
#else
    lxRemoveDir(home);
    lxCreateDir(home);
#endif

    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PrefsClear();
    SavePrefs(lxT("picsimlab_version"), _VERSION_);
    SavePrefs(lxT("picsimlab_lab"), boards_list[lab].name_);
    SavePrefs(lxT("picsimlab_debug"), itoa(GetDebugStatus()));
    SavePrefs(lxT("picsimlab_debugt"), itoa(GetDebugType()));
    SavePrefs(lxT("picsimlab_debugp"), itoa(GetDebugPort()));
    SavePrefs(lxT("picsimlab_position"), itoa(Window->GetX()) + lxT(",") + itoa(Window->GetY()));
    SavePrefs(lxT("picsimlab_scale"), ftoa(scale));
    SavePrefs(lxT("osc_on"), itoa(pboard->GetUseOscilloscope()));
    SavePrefs(lxT("spare_on"), itoa(pboard->GetUseSpareParts()));
    SavePrefs(lxT("picsimlab_lfile"), lxT(" "));

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

    lxZipDir(home, fnpzw);

    lxRemoveDir(home);

    strncpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str(), 1023);
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
        status.st[0] &= ~ST_DI;
    } else {
        status.st[0] |= ST_DI;
    }
}

int CPICSimLab::GetSimulationRun(void) {
    return (status.st[0] & ST_DI) == 0;
}

void CPICSimLab::Configure(const char* home, int use_default_board, int create, const char* lfile,
                           const int disable_debug) {
    char line[1024];
    char fname[2048];
    char fname_[2048];

    char* name;
    char* value;

    int i, j;
    int lc;
    int load_demo = 0;

    int osc_on = 0;
    int spare_on = 0;

    lxString status;

#ifndef _NOTHREAD
    if (cpu_mutex == NULL) {
        cpu_mutex = new lxMutex();
        cpu_cond = new lxCondition(*cpu_mutex);
    }
#endif

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
    if (lxFileExists(fname)) {
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
                            RegisterError(lxString("Invalid board ") + value + "!\n Using default board!");
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
                    ((CToggleButton*)Window->GetChildByName("togglebutton1"))->SetCheck(debug);
#endif
                    SetDebugStatus(debug, 0);
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

                if (!strcmp(name, "picsimlab_position")) {
                    sscanf(value, "%i,%i", &i, &j);
                    Window->SetX(i);
                    Window->SetY(j);
                    printf("PICSimLab: Window position x=%i y=%i\n", i, j);
                }

                if (!strcmp(name, "picsimlab_scale")) {
                    if (create) {
                        double s;
                        sscanf(value, "%lf", &s);
                        SetScale(s);
                        ((CDraw*)Window->GetChildByName("draw1"))->SetWidth(plWidth * GetScale());
                        Window->SetWidth(185 + plWidth * GetScale());
                        ((CDraw*)Window->GetChildByName("draw1"))->SetHeight(plHeight * GetScale());
                        Window->SetHeight(90 + plHeight * GetScale());
                        pboard->SetScale(GetScale());
                        printf("PICSimLab: Window scale %5.2f \n", GetScale());
                    }
                }

                if (!strcmp(name, "picsimlab_dsr_reset")) {
                    sscanf(value, "%i", &use_dsr_reset);
                }

                if (!strcmp(name, "picsimlab_lpath")) {
                    SetPath(lxString(value, lxConvUTF8));
                }

                if (!strcmp(name, "picsimlab_lfile")) {
                    SetFNAME(lxString(value, lxConvUTF8));
                    if (GetFNAME().length() > 1)
                        Window->GetChildByName("menu1")
                            ->GetChildByName("menu1_File")
                            ->GetChildByName("menu1_File_ReloadLast")
                            ->SetEnable(1);
                    else
                        Window->GetChildByName("menu1")
                            ->GetChildByName("menu1_File")
                            ->GetChildByName("menu1_File_ReloadLast")
                            ->SetEnable(0);
                }

                if (pboard != NULL)
                    pboard->ReadPreferences(name, value);

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
        strcpy(SERIALDEVICE, "com6");
#ifdef _USE_PICSTARTP_
        strcpy(PROGDEVICE, "com8");
#endif
#endif
    }

    ((CPMenu*)Window->GetChildByName("menu1")->GetChildByName("menu1_Microcontroller"))->DestroyChilds();
    lxString sdev = pboard->GetSupportedDevices();
    int f;
    int dc = 0;
    while (sdev.size() > 0) {
        f = sdev.find(lxT(","));
        if (f < 0)
            break;
        MMicro[dc].SetFOwner(Window);
        MMicro[dc].SetName("Micro_" + itoa(dc + 1));
        MMicro[dc].SetText(sdev.substr(0, f));
        MMicro[dc].EvMenuActive = menu_EvMicrocontroller;
        ((CPMenu*)Window->GetChildByName("menu1")->GetChildByName("menu1_Microcontroller"))->CreateChild(&MMicro[dc]);
        MMicro[dc].SetVisible(true);
        sdev = sdev.substr(f + 1, sdev.size() - f - 1);
        dc++;

        if (dc >= MAX_MIC) {
            printf("PICSimLab: microcontroller menu only support %i entries!\n", MAX_MIC);
            exit(-1);
        }
    }

    ((CFileDialog*)Window->GetChildByName("filedialog1"))->SetDir(GetPath());

    ((CDraw*)Window->GetChildByName("draw1"))
        ->SetImgFileName(lxGetLocalFile(GetSharePath() + lxT("boards/") + pboard->GetPictureFileName()), GetScale(),
                         GetScale());

    pboard->MSetSerial(SERIALDEVICE);

    if (lfile) {
        if (lxFileExists(lfile)) {
            strcpy(fname, lfile);
        } else {
            printf("PICSimLab: File Not found \"%s\" loading default.\n", lfile);
            RegisterError(lxString("File Not found \n\"") + lfile + "\"\n loading default.");
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

    if (!((lxFileExists(fname)) || (lxFileExists(fname_)))) {
        printf("PICSimLab: File not found! Creating new empty file. \n");
        if (!pboard->GetDefaultProcessor().compare(pboard->GetProcessorName())) {
            load_demo = 1;
        }
    }

    switch (pboard->MInit(pboard->GetProcessorName(), fname, GetNSTEP() * NSTEPKF)) {
        // case HEX_NFOUND:
        //     break;
        case HEX_CHKSUM:
            printf("PICSimLab: File checksum error!\n");
            pboard->MEraseFlash();
            break;
    }

    pboard->Reset();

    SetProcessorName(pboard->GetProcessorName());

    Window->SetTitle(((Instance > 0) ? (lxT("PICSimLab[") + itoa(Instance) + lxT("] - ")) : (lxT("PICSimLab - "))) +
                     lxString(boards_list[lab].name) + lxT(" - ") + pboard->GetProcessorName());

#ifdef _USE_PICSTARTP_
    if (prog_init() >= 0)
        status = lxT("PStart:  Ok ");
    else
        status = lxT("PStart:Error");
#else
    status = lxT("");
#endif

    statusbar->SetField(0, lxT("Running..."));

    ((CThread*)Window->GetChildByName("thread1"))->Run();  // parallel thread
#ifndef __EMSCRIPTEN__
    // FIXME remote control disabled
    ((CThread*)Window->GetChildByName("thread2"))->Run();  // parallel thread
#endif
    ((CTimer*)Window->GetChildByName("timer1"))->SetRunState(1);
    ((CTimer*)Window->GetChildByName("timer2"))->SetRunState(1);

    Application->ProcessEvents();

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
    statusbar->SetField(1, lxT(" "));
#else
    if (GetDebugStatus()) {
        int ret = pboard->DebugInit(GetDebugType());
        if (ret < 0) {
            statusbar->SetField(1, status + lxT("Debug: Error"));
        } else {
            statusbar->SetField(1, status + lxT("Debug: ") + pboard->GetDebugName() + ":" + itoa(GetDebugPort()));
        }
    } else {
        statusbar->SetField(1, status + lxT("Debug: Off"));
    }
#endif

#ifndef NO_TOOLS
    if ((!pboard->GetProcessorName().Cmp("atmega328p")) || (!pboard->GetProcessorName().Cmp("atmega2560"))) {
        Window->GetChildByName("menu1")
            ->GetChildByName("menu1_Tools")
            ->GetChildByName("menu1_Tools_ArduinoBootloader")
            ->SetEnable(true);
    } else {
        Window->GetChildByName("menu1")
            ->GetChildByName("menu1_Tools")
            ->GetChildByName("menu1_Tools_ArduinoBootloader")
            ->SetEnable(false);
    }
#endif

#ifndef __EMSCRIPTEN__
    printf("PICSimLab: Remote Control Port %i\n", GetRemotecPort());
    rcontrol_init(GetRemotecPort() + Instance);
#endif

    if (load_demo) {
        lxString fdemo =
            PICSimLab.GetSharePath() + "boards/" + lxString(boards_list[PICSimLab.GetLab()].name) + lxT("/demo.pzw");

        if (lxFileExists(fdemo)) {
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

void CPICSimLab::SetToDestroy(void) {
    settodestroy = 1;
}

int CPICSimLab::LoadHexFile(lxString fname) {
    int pa;
    int ret = 0;

    pa = GetMcuPwr();
    SetMcuPwr(0);

    // timer1.SetRunState (0);
    status.st[0] |= ST_DI;
    msleep(((CTimer*)Window->GetChildByName("timer1"))->GetTime());
    if (tgo)
        tgo = 1;
    while (status.status & 0x0401) {
        msleep(1);
        Application->ProcessEvents();
    }

    if (GetNeedReboot()) {
        char cmd[4096];
        sprintf(cmd, " %s %s \"%s\"", boards_list[GetLab()].name_, (const char*)GetBoard()->GetProcessorName().c_str(),
                (const char*)fname.char_str());
        EndSimulation(0, cmd);
    }

    GetBoard()->MEnd();
    GetBoard()->MSetSerial(SERIALDEVICE);

    switch (GetBoard()->MInit(GetBoard()->GetProcessorName(), fname.char_str(), GetNSTEP() * NSTEPKF)) {
        case HEX_NFOUND:
            RegisterError(lxT("Hex file not found!"));
            SetMcuRun(0);
            break;
        case HEX_CHKSUM:
            RegisterError(lxT("Hex file checksum error!"));
            GetBoard()->MEraseFlash();
            SetMcuRun(0);
            break;
        case 0:
            SetMcuRun(1);
            break;
    }

    GetBoard()->Reset();

    if (GetMcuRun())
        Window->SetTitle(((GetInstanceNumber() > 0) ? (lxT("PICSimLab[") + itoa(GetInstanceNumber()) + lxT("] - "))
                                                    : (lxT("PICSimLab - "))) +
                         lxString(boards_list[GetLab()].name) + lxT(" - ") + GetBoard()->GetProcessorName() +
                         lxT(" - ") + basename(((CFileDialog*)Window->GetChildByName("filedialog1"))->GetFileName()));
    else
        Window->SetTitle(((GetInstanceNumber() > 0) ? (lxT("PICSimLab[") + itoa(GetInstanceNumber()) + lxT("] - "))
                                                    : (lxT("PICSimLab - "))) +
                         lxString(boards_list[GetLab()].name) + lxT(" - ") + GetBoard()->GetProcessorName());

    ret = !GetMcuRun();

    SetMcuPwr(pa);
    // timer1.SetRunState (1);
    status.st[0] &= ~ST_DI;

#ifdef NO_DEBUG
    statusbar->SetField(1, lxT(" "));
#else
    if (GetDebugStatus()) {
        int ret = GetBoard()->DebugInit(GetDebugType());
        if (ret < 0) {
            statusbar->SetField(1, lxT("Debug: Error"));
        } else {
            statusbar->SetField(
                1, lxT("Debug: ") + GetBoard()->GetDebugName() + ":" + itoa(GetDebugPort() + GetInstanceNumber()));
        }
    } else {
        statusbar->SetField(1, lxT("Debug: Off"));
    }
#endif

    return ret;
}
