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

#include "picsimlab.h"
#include "oscilloscope.h"
#include "spareparts.h"

#include "devices/rcontrol.h"

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
    lab = 1;
    lab_ = 1;
    Workspacefn = "";
    status.status = 0;
    scale = 1.0;
    need_resize = 0;
    tgo = 0;
    osc_on = 0;
    spare_on = 0;
    plWidth = 10;
    plHeight = 10;
    need_clkupdate = 0;

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

void CPICSimLab::Set_debug_port(unsigned short dp) {
    debug_port = dp;
    if (debug) {
        CToggleButton* togglebutton = (CToggleButton*)Window->GetChildByName("togglebutton1");
        togglebutton->SetCheck(0);
        (Window->*(togglebutton->EvOnToggleButton))(NULL);
    }
}

void CPICSimLab::Set_remotec_port(unsigned short rcp) {
    remotec_port = rcp;
#ifndef __EMSCRIPTEN__
    rcontrol_end();
    rcontrol_init(remotec_port);
#endif
}

int CPICSimLab::Get_debug_status(void) {
    return debug;
}

void CPICSimLab::Set_debug_status(int dbs, int updatebtn) {
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

void CPICSimLab::saveprefs(lxString name, lxString value) {
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
    Oscilloscope.GetWindow()->Hide();
    SpareParts.GetWindow()->Hide();
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

    if (PICSimLab.GetInstanceNumber()) {
        sprintf(fname, "%s/picsimlab_%i.ini", home, PICSimLab.GetInstanceNumber());
    } else {
        sprintf(fname, "%s/picsimlab.ini", home);
    }

    PICSimLab.saveprefs(lxT("picsimlab_version"), _VERSION_);
    PICSimLab.saveprefs(lxT("picsimlab_lab"), boards_list[lab].name_);
    PICSimLab.saveprefs(lxT("picsimlab_debug"), itoa(PICSimLab.Get_debug_status()));
    PICSimLab.saveprefs(lxT("picsimlab_debugt"), itoa(PICSimLab.Get_debug_type()));
    PICSimLab.saveprefs(lxT("picsimlab_debugp"), itoa(PICSimLab.Get_debug_port()));
    PICSimLab.saveprefs(lxT("picsimlab_remotecp"), itoa(PICSimLab.Get_remotec_port()));
    PICSimLab.saveprefs(lxT("picsimlab_position"),
                        itoa(PICSimLab.GetWindow()->GetX()) + lxT(",") + itoa(PICSimLab.GetWindow()->GetY()));
    PICSimLab.saveprefs(lxT("picsimlab_scale"), ftoa(scale));
    PICSimLab.saveprefs(lxT("osc_on"), itoa(PICSimLab.GetBoard()->GetUseOscilloscope()));
    PICSimLab.saveprefs(lxT("spare_on"), itoa(PICSimLab.GetBoard()->GetUseSpareParts()));
#ifndef _WIN_
    PICSimLab.saveprefs(lxT("picsimlab_lser"), SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    PICSimLab.saveprefs(lxT("picsimlab_lprog"), PROGDEVICE);
#endif
#else
    PICSimLab.saveprefs("picsimlab_wser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_
    PICSimLab.saveprefs("picsimlab_wprog", PROGDEVICE);
#endif
#endif

    if (PATH.size() > 0) {
        PICSimLab.saveprefs(lxT("picsimlab_lpath"), PATH);
    } else {
        PICSimLab.saveprefs(lxT("picsimlab_lpath"), " ");
    }
    PICSimLab.saveprefs(lxT("picsimlab_lfile"), FNAME);

    PICSimLab.GetBoard()->WritePreferences();

    Oscilloscope.WritePreferences();
    SpareParts.WritePreferences();

    PICSimLab.PrefsSaveToFile(fname);

    // write memory
    if (PICSimLab.GetInstanceNumber()) {
        sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[lab_].name_, (const char*)proc_.c_str(),
                PICSimLab.GetInstanceNumber());
    } else {
        sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[lab_].name_, (const char*)proc_.c_str());
    }
    printf("PICSimLab: Saving \"%s\"\n", fname);
    PICSimLab.GetBoard()->MDumpMemory(fname);

    PICSimLab.GetBoard()->MEnd();

#ifndef __EMSCRIPTEN__
    if (((CThread*)Window->GetChildByName("thread3"))->GetRunState()) {
        ((CThread*)Window->GetChildByName("thread3"))->Destroy();
    }
#endif

    if (PICSimLab.GetInstanceNumber()) {
        sprintf(fname, "%s/parts_%s_%i.pcf", home, boards_list[lab_].name_, PICSimLab.GetInstanceNumber());
    } else {
        sprintf(fname, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
    }
    SpareParts.SaveConfig(fname);
    SpareParts.DeleteParts();
    if (PICSimLab.GetInstanceNumber()) {
        sprintf(fname, "%s/palias_%s_%i.ppa", home, boards_list[lab_].name_, PICSimLab.GetInstanceNumber());
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

    if (PICSimLab.GetNeedReboot()) {
        char cmd[1024];
        printf("PICSimLab: Reboot !!!\n");
        rcontrol_server_end();
        PICSimLab.GetBoard()->EndServers();
        PICSimLab.DeleteBoard();
        strcpy(cmd, lxGetExecutablePath().c_str());
        if (newpath) {
            strcat(cmd, " ");
            strcat(cmd, newpath);
        }
        printf("PICSimLab: %s\n", cmd);

        printf("PICSimLab: End Board Simulation.\n");
#if !defined(__EMSCRIPTEN__) && !defined(_CONSOLE_LOG_)
        fflush(stdout);
        freopen(NULLFILE, "w", stdout);
        fflush(stderr);
        freopen(NULLFILE, "w", stderr);
        char fname[1200];
        snprintf(fname, 1199, "%s/picsimlab_log%i.txt", (const char*)PICSimLab.GetHomePath().c_str(),
                 PICSimLab.GetInstanceNumber());
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

    PICSimLab.DeleteBoard();
}

// legacy format support before 0.8.2
static const char old_board_names[6][20] = {"Breadboard", "McLab1", "K16F", "McLab2", "PICGenios", "Arduino_Uno"};

void CPICSimLab::LoadWorkspace(lxString fnpzw) {
    char home[1024];
    char fzip[1280];

    if (!lxFileExists(fnpzw)) {
        printf("PICSimLab: file %s not found!\n", (const char*)fnpzw.c_str());
        PICSimLab.RegisterError("PICSimLab: file " + fnpzw + " not found!");
        return;
    }
    if (!fnpzw.Contains(".pzw")) {
        printf("PICSimLab: file %s is not a .pzw file!\n", (const char*)fnpzw.c_str());
        PICSimLab.RegisterError("PICSimLab: file " + fnpzw + " is not a .pzw file!");
        return;
    }
    // write options
    strncpy(fzip, (char*)lxGetTempDir(lxT("picsimlab")).char_str(), 1023);
    strncat(fzip, "/", 1023);

    strncpy(home, (char*)lxGetTempDir(lxT("picsimlab")).char_str(), 1023);
    strncat(home, "/picsimlab_workspace/", 1023);

    lxRemoveDir(home);

    lxUnzipDir(fnpzw, fzip);

    PICSimLab.EndSimulation(0, fnpzw.c_str());

    PICSimLab.SetWorkspaceFileName(fnpzw);

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
                    PICSimLab.RegisterError("The loaded workspace was made with a newer version " + pzw_ver +
                                            ".\n Please update your PICSimLab " _VERSION_ " .");
                }

                continue;
            }
#ifndef LEGACY081
            saveprefs(name, value);
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

            PICSimLab.saveprefs(name_, value_);
#endif
        }
    }
    PICSimLab.PrefsSaveToFile(fzip);

    Configure(home);

    need_resize = 1;

#ifdef CONVERTER_MODE
    fnpzw.replace(fnpzw.Length() - 4, 5, "_.pzw");
    cvt_fname = fnpzw;
#else
    snprintf(fzip, 1279, "%s/Readme.html", home);
    if (lxFileExists(fzip)) {
#ifdef EXT_BROWSER
#ifndef __EMSCRIPTEN__
        lxLaunchDefaultBrowser(lxT("file://") + lxString(fzip));
#endif
#else
        Window2.html1.SetLoadFile(fzip);
        Window2.Show();
#endif
    } else {
        snprintf(fzip, 1279, "%s/Readme.txt", home);
        if (lxFileExists(fzip)) {
#ifdef EXT_BROWSER
#ifndef __EMSCRIPTEN__
            lxLaunchDefaultBrowser(lxT("file://") + lxString(fzip));
#endif
#else
            Window2.html1.SetLoadFile(fzip);
            Window2.Show();
#endif
        }
    }
#endif
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

    PICSimLab.SetWorkspaceFileName(fnpzw);

    // write options

    strncpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str(), 1023);
    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PICSimLab.PrefsSaveToFile(fname);

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
    PICSimLab.PrefsClear();
    PICSimLab.saveprefs(lxT("picsimlab_version"), _VERSION_);
    PICSimLab.saveprefs(lxT("picsimlab_lab"), boards_list[PICSimLab.GetLab()].name_);
    PICSimLab.saveprefs(lxT("picsimlab_debug"), itoa(PICSimLab.Get_debug_status()));
    PICSimLab.saveprefs(lxT("picsimlab_debugt"), itoa(PICSimLab.Get_debug_type()));
    PICSimLab.saveprefs(lxT("picsimlab_debugp"), itoa(PICSimLab.Get_debug_port()));
    PICSimLab.saveprefs(lxT("picsimlab_position"),
                        itoa(PICSimLab.GetWindow()->GetX()) + lxT(",") + itoa(PICSimLab.GetWindow()->GetY()));
    PICSimLab.saveprefs(lxT("picsimlab_scale"), ftoa(scale));
    PICSimLab.saveprefs(lxT("osc_on"), itoa(PICSimLab.GetBoard()->GetUseOscilloscope()));
    PICSimLab.saveprefs(lxT("spare_on"), itoa(PICSimLab.GetBoard()->GetUseSpareParts()));
    PICSimLab.saveprefs(lxT("picsimlab_lfile"), lxT(" "));

    PICSimLab.GetBoard()->WritePreferences();

    if (PICSimLab.GetBoard()->GetUseOscilloscope())
        Oscilloscope.WritePreferences();

    if (PICSimLab.GetBoard()->GetUseSpareParts()) {
        SpareParts.WritePreferences();
    }

    PICSimLab.PrefsSaveToFile(fname);

    // write memory

    snprintf(fname, 1279, "%s/mdump_%s_%s.hex", home, boards_list[PICSimLab.GetLab_()].name_,
             (const char*)proc_.c_str());

    printf("PICSimLab: Saving \"%s\"\n", fname);
    PICSimLab.GetBoard()->MDumpMemory(fname);

    // write spare part config
    snprintf(fname, 1279, "%s/parts_%s.pcf", home, boards_list[PICSimLab.GetLab_()].name_);
    SpareParts.SaveConfig(fname);
    sprintf(fname, "%s/palias_%s.ppa", home, boards_list[PICSimLab.GetLab_()].name_);
    SpareParts.SavePinAlias(fname);

    lxZipDir(home, fnpzw);

    lxRemoveDir(home);

    strncpy(home, (char*)lxGetUserDataDir(lxT("picsimlab")).char_str(), 1023);
    snprintf(fname, 1279, "%s/picsimlab.ini", home);
    PICSimLab.PrefsClear();
    PICSimLab.PrefsLoadFromFile(fname);

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
        PICSimLab.status.st[0] &= ~ST_DI;
    } else {
        PICSimLab.status.st[0] |= ST_DI;
    }
}

int CPICSimLab::GetSimulationRun(void) {
    return (PICSimLab.status.st[0] & ST_DI) == 0;
}

void CPICSimLab::Configure(const char* home, int use_default_board, int create, const char* lfile) {
    char line[1024];
    char fname[1024];

    char* name;
    char* value;

    int i, j;
    int lc;

    lxString status;

#ifndef _NOTHREAD
    if (PICSimLab.cpu_mutex == NULL) {
        PICSimLab.cpu_mutex = new lxMutex();
        PICSimLab.cpu_cond = new lxCondition(*PICSimLab.cpu_mutex);
    }
#endif

    if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
        snprintf(fname, 1023, "%s/picsimlab_%i.ini", home, PICSimLab.GetInstanceNumber());
    } else {
        snprintf(fname, 1023, "%s/picsimlab.ini", home);
    }
    SERIALDEVICE[0] = ' ';
    SERIALDEVICE[1] = 0;
#ifdef _USE_PICSTARTP_
    PROGDEVICE[0] = ' ';
    PROGDEVICE[1] = 0;
#endif
    PICSimLab.DeleteBoard();

    PICSimLab.PrefsClear();
    if (lxFileExists(fname)) {
        printf("PICSimLab: Load Config from file \"%s\"\n", fname);
        if (PICSimLab.PrefsLoadFromFile(fname)) {
            for (lc = 0; lc < (int)PICSimLab.PrefsGetLinesCount(); lc++) {
                strncpy(line, PICSimLab.PrefsGetLine(lc).c_str(), 1023);

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
                        PICSimLab.SetLabs(0, 0);
                    } else {
                        for (i = 0; i < BOARDS_LAST; i++) {
                            if (!strcmp(boards_list[i].name_, value)) {
                                break;
                            }
                        }
                        PICSimLab.SetLabs(i, i);
                        if (PICSimLab.GetLab() == BOARDS_LAST) {
                            PICSimLab.RegisterError(lxString("Invalid board ") + value + "!\n Using default board!");
                        }
                    }
                    printf("PICSimLab: Open board %s\n", boards_list[PICSimLab.GetLab()].name);
                    int lab = PICSimLab.GetLab();
                    int lab_ = PICSimLab.GetLab_();
                    PICSimLab.SetBoard(create_board(&lab, &lab_));
                    PICSimLab.SetLabs(lab, lab_);
                    if (PICSimLab.GetBoard()->GetScale() < PICSimLab.GetScale()) {
                        PICSimLab.SetScale(PICSimLab.GetBoard()->GetScale());
                    } else {
                        PICSimLab.GetBoard()->SetScale(PICSimLab.GetScale());
                    }
                    PICSimLab.SetJUMPSTEPS(DEFAULTJS);
                    PICSimLab.SetClock(2.0);  // Default clock
                }

                if (!strcmp(name, "picsimlab_debug")) {
                    int debug = 0;
#ifndef NO_DEBUG
                    sscanf(value, "%i", &debug);
                    ((CToggleButton*)Window->GetChildByName("togglebutton1"))->SetCheck(debug);
#endif
                    printf("PICSimLab: Debug On = %i\n", debug);
                    PICSimLab.Set_debug_status(debug, 0);
                }

                if (!strcmp(name, "picsimlab_debugt")) {
                    int debug_type;
                    sscanf(value, "%i", &debug_type);
                    PICSimLab.Set_debug_type(debug_type);
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
                        PICSimLab.SetScale(s);
                        ((CDraw*)Window->GetChildByName("draw1"))->SetWidth(plWidth * PICSimLab.GetScale());
                        Window->SetWidth(185 + plWidth * PICSimLab.GetScale());
                        ((CDraw*)Window->GetChildByName("draw1"))->SetHeight(plHeight * PICSimLab.GetScale());
                        Window->SetHeight(90 + plHeight * PICSimLab.GetScale());
                        PICSimLab.GetBoard()->SetScale(PICSimLab.GetScale());
                        printf("PICSimLab: Window scale %5.2f \n", PICSimLab.GetScale());
                    }
                }

                if (!strcmp(name, "picsimlab_lpath")) {
                    PICSimLab.SetPath(lxString(value, lxConvUTF8));
                }

                if (!strcmp(name, "picsimlab_lfile")) {
                    PICSimLab.SetFNAME(lxString(value, lxConvUTF8));
                    if (PICSimLab.GetFNAME().length() > 1)
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

                if (PICSimLab.GetBoard() != NULL)
                    PICSimLab.GetBoard()->ReadPreferences(name, value);

                Oscilloscope.ReadPreferences(name, value);
                SpareParts.ReadPreferences(name, value);
            }
        }
    }

    if (PICSimLab.GetBoard() == NULL) {
        printf("PICSimLab: Error open config file \"%s\"!\n", fname);

        PICSimLab.SetLabs(0, 0);  // default
        PICSimLab.SetBoard(boards_list[0].bcreate());

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
    lxString sdev = PICSimLab.GetBoard()->GetSupportedDevices();
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

    ((CFileDialog*)Window->GetChildByName("filedialog1"))->SetDir(PICSimLab.GetPath());

    ((CDraw*)Window->GetChildByName("draw1"))
        ->SetImgFileName(
            lxGetLocalFile(PICSimLab.GetSharePath() + lxT("boards/") + PICSimLab.GetBoard()->GetPictureFileName()),
            PICSimLab.GetScale(), PICSimLab.GetScale());

    PICSimLab.GetBoard()->MSetSerial(SERIALDEVICE);

    if (lfile) {
        if (lxFileExists(lfile)) {
            strcpy(fname, lfile);
        } else {
            printf("PICSimLab: File Not found \"%s\" loading default.\n", lfile);
            PICSimLab.RegisterError(lxString("File Not found \n\"") + lfile + "\"\n loading default.");
            if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
                sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[PICSimLab.GetLab()].name_,
                        (const char*)PICSimLab.GetBoard()->GetProcessorName().c_str(), PICSimLab.GetInstanceNumber());
            } else {
                sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[PICSimLab.GetLab()].name_,
                        (const char*)PICSimLab.GetBoard()->GetProcessorName().c_str());
            }
        }
    } else {
        if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
            sprintf(fname, "%s/mdump_%s_%s_%i.hex", home, boards_list[PICSimLab.GetLab()].name_,
                    (const char*)PICSimLab.GetBoard()->GetProcessorName().c_str(), PICSimLab.GetInstanceNumber());
        } else {
            sprintf(fname, "%s/mdump_%s_%s.hex", home, boards_list[PICSimLab.GetLab()].name_,
                    (const char*)PICSimLab.GetBoard()->GetProcessorName().c_str());
        }
    }
    printf("PICSimLab: Opening \"%s\"\n", fname);
    switch (
        PICSimLab.GetBoard()->MInit(PICSimLab.GetBoard()->GetProcessorName(), fname, PICSimLab.GetNSTEP() * NSTEPKF)) {
        case HEX_NFOUND:
            printf("File not found!\n");
            break;
        case HEX_CHKSUM:
            printf("File checksum error!\n");
            PICSimLab.GetBoard()->MEraseFlash();
            break;
    }

    PICSimLab.GetBoard()->Reset();

    PICSimLab.SetProcessorName(PICSimLab.GetBoard()->GetProcessorName());

    Window->SetTitle(
        ((PICSimLab.GetInstanceNumber() > 0) ? (lxT("PICSimLab[") + itoa(PICSimLab.GetInstanceNumber()) + lxT("] - "))
                                             : (lxT("PICSimLab - "))) +
        lxString(boards_list[PICSimLab.GetLab()].name) + lxT(" - ") + PICSimLab.GetBoard()->GetProcessorName());

#ifdef _USE_PICSTARTP_
    if (prog_init() >= 0)
        status = lxT("PStart:  Ok ");
    else
        status = lxT("PStart:Error");
#else
    status = lxT("");
#endif

#ifdef NO_DEBUG
    statusbar1.SetField(1, lxT(" "));
#else
    if (PICSimLab.Get_debug_status()) {
        int ret = PICSimLab.GetBoard()->DebugInit(PICSimLab.Get_debug_type());
        if (ret < 0) {
            statusbar->SetField(1, status + lxT("Debug: Error"));
        } else {
            statusbar->SetField(1, status + lxT("Debug: ") + PICSimLab.GetBoard()->GetDebugName() + ":" +
                                       itoa(PICSimLab.Get_debug_port() + PICSimLab.GetInstanceNumber()));
        }
    } else {
        statusbar->SetField(1, status + lxT("Debug: Off"));
    }
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

    Oscilloscope.SetBoard(PICSimLab.GetBoard());
    Oscilloscope.SetBaseTimer();

    if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
        sprintf(fname, "%s/parts_%s_%i.pcf", home, boards_list[PICSimLab.GetLab()].name_,
                PICSimLab.GetInstanceNumber());
    } else {
        sprintf(fname, "%s/parts_%s.pcf", home, boards_list[PICSimLab.GetLab()].name_);
    }
    SpareParts.LoadConfig(fname);
    if (PICSimLab.GetInstanceNumber() && !PICSimLab.GetHomePath().compare(home)) {
        sprintf(fname, "%s/palias_%s_%i.ppa", home, boards_list[PICSimLab.GetLab_()].name_,
                PICSimLab.GetInstanceNumber());
    } else {
        sprintf(fname, "%s/palias_%s.ppa", home, boards_list[PICSimLab.GetLab_()].name_);
    }
    SpareParts.LoadPinAlias(fname);

    if ((!PICSimLab.GetBoard()->GetProcessorName().Cmp("atmega328p")) ||
        (!PICSimLab.GetBoard()->GetProcessorName().Cmp("atmega2560"))) {
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

#ifndef __EMSCRIPTEN__
    rcontrol_init(PICSimLab.Get_remotec_port() + PICSimLab.GetInstanceNumber());
#endif
}

double CPICSimLab::GetIdleMs(void) {
    return idle_ms;
}

void CPICSimLab::SetIdleMs(double im) {
    idle_ms = im;
}
