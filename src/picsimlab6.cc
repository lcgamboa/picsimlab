/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

// Project Wizard

#include "picsimlab6.h"
#include "picsimlab1.h"
#include "picsimlab6_d.cc"
#include "picsimlab6_mplabx.h"
#include "picsimlab6_vspio.h"

#include "lib/picsimlab.h"

CPWindow6 Window6;

#define OP_ONLY_CREATE 1
#define OP_CREATE_AND_OPEN (OP_ONLY_CREATE + 2)
#define OP_OPEN_EXISTING 4

// Implementation

void CPWindow6::_EvOnCreate(CControl* control) {
    SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                  ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                  : ("PICSimLab - ")) +
             "Project Wizard");
}

void CPWindow6::_EvOnShow(CControl* control) {
    board = PICSimLab.GetBoard()->GetName();
    processor = PICSimLab.GetBoard()->GetProcessorName();

    label1.SetText("Board: " + board);
    label2.SetText("Microcontroller: " + processor);

    combo1.SetItems(PICSimLab.GetBoard()->GetSupportedIDEs());
    combo1.SetText(combo1.GetItem(0));
    combo1_EvOnComboChange(NULL);

    edit1.SetText("Untitled");
}

void CPWindow6::button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    dirdialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    operation = OP_ONLY_CREATE;
    dirdialog1.Run();
}

void CPWindow6::button2_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    dirdialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    if (!ide.compare("PlatformIO IDE for VSCode")) {
        dirdialog1.SetDirName(lxGetDocumentsDir("picsimlab") + "/PlatformIO/Projects/");
    } else if (!ide.compare("MPLAB X IDE")) {
        dirdialog1.SetDirName(lxGetHomeDir() + "/MPLABXProjects/");
    }
    operation = OP_CREATE_AND_OPEN;
    dirdialog1.Run();
}

void CPWindow6::button3_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    WDestroy();
}

void CPWindow6::button4_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    operation = OP_OPEN_EXISTING;
    if (!ide.compare("PlatformIO IDE for VSCode")) {
        dirdialog1.SetDirName(lxGetDocumentsDir("picsimlab") + "/PlatformIO/Projects/");
    } else if (!ide.compare("MPLAB X IDE")) {
        dirdialog1.SetDirName(lxGetHomeDir() + "/MPLABXProjects/");
    }
    dirdialog1.Run();
}

void CPWindow6::button5_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    char stemp[256];
    snprintf(stemp, 255, "https://lcgamboa.github.io/picsimlab_docs/%s/ProjectWizardWindow.html", _VERSION_);
    lxLaunchDefaultBrowser(stemp);
}

const char* CPWindow6::GetFrameworkList(const std::string ide) {
    std::string board = PICSimLab.GetBoard()->GetName();
    std::string processor = PICSimLab.GetBoard()->GetProcessorName();

    if (!ide.compare("PlatformIO IDE for VSCode")) {
        if (!processor.compare("ESP32") || !processor.compare("ESP32-C3")) {
            return "Arduino,IDF,";
        } else if (!processor.compare("stm32f103c8t6") || !processor.compare("stm32f103rbt6")) {
            return "Arduino,CMSIS,";

        } else if (!processor.compare("C51")) {
            return "None,";
        } else {
            return "Arduino,";
        }
    } else if (!ide.compare("MPLAB X IDE")) {
        return "XC8,";
    } else {
        return "N/A,";
    }
    return "N/A,";
}

void CPWindow6::combo1_EvOnComboChange(CControl* control) {
    ide = combo1.GetText();

    if (!ide.compare("N/A")) {
        combo1.SetEnable(0);
        combo2.SetEnable(0);
        combo3.SetEnable(0);
    } else {
        combo1.SetEnable(1);
        combo2.SetEnable(1);
        combo3.SetEnable(1);
        combo2.SetItems(GetFrameworkList(ide));
    }

    combo1.SetEnable((combo1.GetItemsCount() == 1) ? 0 : 1);

    combo2.SetText(combo2.GetItem(0));
    combo2_EvOnComboChange(NULL);
}

const char* CPWindow6::GetExampleList(const std::string ide, const std::string framework) {
    std::string board = PICSimLab.GetBoard()->GetName();
    std::string processor = PICSimLab.GetBoard()->GetProcessorName();

    if (!ide.compare("PlatformIO IDE for VSCode")) {
        if (!framework.compare("Arduino")) {
            if (!processor.compare("attiny85") || !processor.compare("STM8S103")) {
                return "Blink,";
            } else {
                return "Blink,Blink FreeRTOS,";
            }
        } else if (!framework.compare("IDF")) {
            return "Blink,";
        } else if (!framework.compare("CMSIS")) {
            return "Blink,";
        } else if (!framework.compare("None")) {
            return "Blink,";
        }
    } else if (!ide.compare("MPLAB X IDE")) {
        if (!framework.compare("XC8")) {
            if (strstr(processor.c_str(), "PIC18F")) {
                if (!board.compare("PICGenios")) {
                    return "Blink,Blink FreeRTOS,Blink PICGenios,";
                } else {
                    return "Blink,Blink FreeRTOS,";
                }
            } else {
                if (!board.compare("PICGenios")) {
                    return "Blink,Blink PICGenios,";
                } else {
                    return "Blink,";
                }
            }
        }
    }
    return "N/A,";
}

void CPWindow6::combo2_EvOnComboChange(CControl* control) {
    framework = combo2.GetText();

    combo3.SetItems("N/A,");

    if (!framework.compare("N/A")) {
        combo2.SetEnable(0);
        combo3.SetEnable(0);
        button4.SetEnable(0);
    } else {
        combo2.SetEnable(1);
        combo3.SetEnable(1);
        button4.SetEnable(1);
        combo3.SetItems(GetExampleList(ide, framework));
    }

    combo2.SetEnable((combo2.GetItemsCount() == 1) ? 0 : 1);

    combo3.SetText(combo3.GetItem(0));

    ctemplate = combo3.GetText();

    if (ctemplate.compare("N/A")) {
        combo3.SetEnable(1);
        edit1.SetEnable(1);
        button1.SetEnable(1);
        button2.SetEnable(1);
    } else {
        combo3.SetEnable(0);
        edit1.SetEnable(0);
        button1.SetEnable(0);
        button2.SetEnable(0);
    }

    combo3.SetEnable((combo3.GetItemsCount() == 1) ? 0 : 1);
}

void CPWindow6::combo3_EvOnComboChange(CControl* control) {
    ctemplate = combo3.GetText();
}

int CPWindow6::CreateProject(const std::string ide, const std::string framework, const std::string ctemplate,
                             const lxString prjdir, const int operation) {
    std::string board = PICSimLab.GetBoard()->GetName();
    std::string processor = PICSimLab.GetBoard()->GetProcessorName();

    if (!strstr(GetExampleList(ide, framework), ctemplate.c_str()) || !ctemplate.compare("N/A")) {
        PICSimLab.RegisterError("PICSimLab", (const char*)(lxString("Invalid code template: ") + ctemplate).utf8_str());
        return 1;
    }

    if (PICSimLab.SystemCmd(PSC_DIREXISTS, prjdir.utf8_str())) {
        PICSimLab.RegisterError("PICSimLab",
                                (const char*)(lxString("Project dir ") + prjdir + " already exists!").utf8_str());
        return 1;
    } else {
        if (PICSimLab.SystemCmd(PSC_CREATEDIR, prjdir.utf8_str()) == 0) {
            if (!ide.compare("PlatformIO IDE for VSCode")) {
                lxString fzip = PICSimLab.GetSharePath() + "prj_wizard/platformio.zip";
                PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.utf8_str(), (void*)((const char*)prjdir.utf8_str()));

                // board selection
                int gdb_debug = 1;
                int pio_test = 1;
                lxString pioboard = "";
                lxString pioplatform = "";
                lxString pioframework = "";
                lxString ledpin = "";
                lxString hwpin = "";
                lxString env_extra = "";
                lxString monitor_rst = "";
                lxString ftype = "";
                lxString tbreak = "";
                lxString include_extra = "";
                lxString main_extra = "";
                if (!board.compare("Arduino Uno")) {
                    pioboard = "uno";
                    pioplatform = "atmelavr @ 5.3.0";
                    pioframework = "arduino";
                    ledpin = "13";
                    hwpin = "19";
                    ftype = "hex";
                    tbreak = "setup";
                    if (!ctemplate.compare("Blink FreeRTOS")) {
                        include_extra = "#include <Arduino_FreeRTOS.h>\n";
                        env_extra = "lib_deps =\n	feilipu/FreeRTOS @ 11.1.0-3\n";
                    }
                } else if (!board.compare("Arduino Nano")) {
                    pioboard = "nanoatmega328";
                    pioplatform = "atmelavr @ 5.3.0";
                    pioframework = "arduino";
                    ledpin = "13";
                    hwpin = "17";
                    ftype = "hex";
                    tbreak = "setup";
                    if (!ctemplate.compare("Blink FreeRTOS")) {
                        include_extra = "#include <Arduino_FreeRTOS.h>\n";
                        env_extra = "lib_deps =\n	feilipu/FreeRTOS @ 11.1.0-3\n";
                    }
                } else if (!board.compare("Arduino Mega")) {
                    pioboard = "megaatmega2560";
                    pioplatform = "atmelavr @ 5.3.0";
                    pioframework = "arduino";
                    ledpin = "13";
                    hwpin = "26";
                    ftype = "hex";
                    tbreak = "setup";
                    if (!ctemplate.compare("Blink FreeRTOS")) {
                        include_extra = "#include <Arduino_FreeRTOS.h>\n";
                        env_extra = "lib_deps =\n	feilipu/FreeRTOS @ 11.1.0-3\n";
                    }
                } else if (!board.compare("Franzininho DIY")) {
                    pioboard = "attiny85";
                    pioplatform = "atmelavr @ 5.3.0";
                    pioframework = "arduino";
                    ledpin = "1";
                    hwpin = "6";
                    env_extra = "board_build.f_cpu = 16000000L\nbuild_flags = -DCLOCK_SOURCE=6\n";
                    ftype = "hex";
                    tbreak = "setup";
                } else if (!board.compare("Blue Pill")) {
                    pioboard = "bluepill_f103c8";
                    pioplatform = "ststm32 @ 19.7.1";
                    if (!framework.compare("Arduino")) {
                        pioframework = "arduino";
                        tbreak = "setup";
                        if (!ctemplate.compare("Blink FreeRTOS")) {
                            include_extra = "#include <STM32FreeRTOS.h>\n";
                            env_extra = "lib_deps =\n	stm32duino/STM32duino FreeRTOS\n";
                            main_extra = "\n  vTaskStartScheduler();\n  while(1);\n";
                        }
                    } else {
                        pioframework = "cmsis";
                        pio_test = 0;
                        tbreak = "main";
                    }
                    ledpin = "PC13";
                    hwpin = "2";
                    monitor_rst = "       monitor system_reset\n";
                    ftype = "bin";
                } else if (!board.compare("STM32 H103")) {
                    pioboard = "olimex_f103";
                    pioplatform = "ststm32 @ 19.7.1";
                    if (!framework.compare("Arduino")) {
                        pioframework = "arduino";
                        tbreak = "setup";
                        if (!ctemplate.compare("Blink FreeRTOS")) {
                            include_extra = "#include <STM32FreeRTOS.h>\n";
                            env_extra = "lib_deps =\n	stm32duino/STM32duino FreeRTOS\n";
                            main_extra = "\n  vTaskStartScheduler();\n  while(1);\n";
                        }
                    } else {
                        pioframework = "cmsis";
                        pio_test = 0;
                        tbreak = "main";
                    }
                    ledpin = "PC12";
                    hwpin = "53";
                    monitor_rst = "       monitor system_reset\n";
                    ftype = "bin";
                } else if (!board.compare("ESP32-DevKitC")) {
                    pioboard = "esp32dev";
                    pioplatform = "espressif32 @ 7.0.1";
                    if (!framework.compare("Arduino")) {
                        pioframework = "arduino";
                        tbreak = "setup";
                    } else {
                        pioframework = "espidf";
                        tbreak = "app_main";
                    }
                    ledpin = "2";
                    hwpin = "24";
                    monitor_rst = "       monitor system_reset\n";
                    env_extra = "board_build.flash_mode = dio\nboard_upload.flash_size = 4MB\n";
                    ftype = "bin";
                } else if (!board.compare("ESP32-C3-DevKitC-02")) {
                    pioboard = "esp32-c3-devkitc-02";
                    pioplatform = "espressif32 @ 6.8.1";
                    if (!framework.compare("Arduino")) {
                        pioframework = "arduino";
                        tbreak = "setup";
                    } else {
                        pioframework = "espidf";
                        tbreak = "app_main";
                    }
                    ledpin = "2";
                    hwpin = "27";
                    monitor_rst = "       monitor system_reset\n";
                    env_extra = "board_build.flash_mode = dio\nboard_upload.flash_size = 4MB\n";
                    ftype = "bin";
                } else if (!board.compare("uCboard")) {
                    if (!processor.compare("C51")) {
                        pioboard = "Generic8051";
                        pioplatform = "intel_mcs51 @ 2.2.0";
                        pioframework = "";
                        ledpin = "P3_2";
                        hwpin = "12";
                        tbreak = "main";
                    } else if (!processor.compare("STM8S103")) {
                        pioboard = "stm8sblue";
                        pioplatform = "ststm8 @ 2.1.0";
                        pioframework = "arduino";
                        ledpin = "4";
                        hwpin = "12";
                        env_extra = "board_build.f_cpu = 4000000L\n";
                        tbreak = "setup";
                    }
                    gdb_debug = 0;
                    pio_test = 0;
                    ftype = "hex";
                } else {
                    PICSimLab.RegisterError("PICSimLab", (const char*)("Not supported board: " + board).c_str());
                    PICSimLab.SystemCmd(PSC_REMOVEDIR, (const char*)prjdir.utf8_str());
                    WDestroy();
                    return 1;
                }

                // main
                if (!framework.compare("Arduino")) {
                    FILE* fmain = fopen_UTF8((prjdir + "src/main.cpp").utf8_str(), "w");
                    if (fmain == NULL) {
                        PICSimLab.RegisterError(
                            "PICSimLab",
                            (const char*)(lxString("File ") + prjdir + "src/main.cpp can't be open!").utf8_str());
                        return 1;
                    }
                    if (!ctemplate.compare("Blink FreeRTOS")) {
                        fprintf(fmain, blink_RTOS_code, (const char*)include_extra.c_str(), (const char*)ledpin.c_str(),
                                (const char*)main_extra.c_str());
                    } else {
                        fprintf(fmain, blink_code, (const char*)ledpin.c_str());
                    }
                    fclose(fmain);
                    if (!processor.compare("STM8S103")) {
                        PICSimLab.SystemCmd(PSC_RENAMEFILE, (const char*)(prjdir + "src/main.cpp").utf8_str(),
                                            (void*)((const char*)(prjdir + "src/main.c").utf8_str()));
                        PICSimLab.SystemCmd(PSC_REMOVEFILE, (const char*)(prjdir + "test/test_main.cpp").utf8_str());
                    }
                } else if (!framework.compare("IDF")) {
                    PICSimLab.SystemCmd(PSC_RENAMEFILE, (prjdir + (const char*)"test/test_main.cpp").utf8_str(),
                                        (void*)((const char*)(prjdir + "test/test_main.c").utf8_str()));
                    PICSimLab.SystemCmd(PSC_RENAMEFILE, (const char*)(prjdir + "src/main.cpp").utf8_str(),
                                        (void*)((const char*)(prjdir + "src/main.c").utf8_str()));
                    FILE* fmain = fopen_UTF8((prjdir + "src/main.c").utf8_str(), "w");
                    if (fmain == NULL) {
                        PICSimLab.RegisterError(
                            "PICSimLab",
                            (const char*)(lxString("File ") + prjdir + "src/main.c can't be open!").utf8_str());
                        return 1;
                    }
                    fprintf(fmain, blink_idf_code, (const char*)ledpin.c_str());
                    fclose(fmain);

                    FILE* fsdkcfg = fopen_UTF8((prjdir + "sdkconfig.defaults").utf8_str(), "w");
                    if (fsdkcfg == NULL) {
                        PICSimLab.RegisterError(
                            "PICSimLab",
                            (const char*)(lxString("File ") + prjdir + "sdkconfig.defaults can't be open!").utf8_str());
                        return 1;
                    }
                    fprintf(fsdkcfg, "CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y\n");
                    fclose(fsdkcfg);
                } else if (!framework.compare("CMSIS")) {
                    PICSimLab.SystemCmd(PSC_RENAMEFILE, (const char*)(prjdir + "src/main.cpp").utf8_str(),
                                        (void*)((const char*)(prjdir + "src/main.c").utf8_str()));
                    FILE* fmain = fopen_UTF8((prjdir + "src/main.c").utf8_str(), "w");
                    if (fmain == NULL) {
                        PICSimLab.RegisterError(
                            "PICSimLab",
                            (const char*)(lxString("File ") + prjdir + "src/main.c can't be open!").utf8_str());
                        return 1;
                    }
                    fprintf(fmain, blink_cmsis_code, (const char*)ledpin.c_str());
                    fclose(fmain);

                    PICSimLab.SystemCmd(PSC_REMOVEFILE, (const char*)(prjdir + "test/test_main.cpp").utf8_str());
                } else if (!framework.compare("None")) {
                    PICSimLab.SystemCmd(PSC_RENAMEFILE, (const char*)(prjdir + "src/main.cpp").utf8_str(),
                                        (void*)((const char*)(prjdir + "src/main.c").utf8_str()));
                    FILE* fmain = fopen_UTF8((prjdir + "src/main.c").utf8_str(), "w");
                    if (fmain == NULL) {
                        PICSimLab.RegisterError(
                            "PICSimLab",
                            (const char*)(lxString("File ") + prjdir + "src/main.c can't be open!").utf8_str());
                        return 1;
                    }
                    fprintf(fmain, blink_c51_code, (const char*)ledpin.c_str());
                    fclose(fmain);

                    PICSimLab.SystemCmd(PSC_REMOVEFILE, (const char*)(prjdir + "test/test_main.cpp").utf8_str());
                }

                // platformio.ini
                FILE* fpio = fopen_UTF8((prjdir + "platformio.ini").utf8_str(), "w");
                if (fpio == NULL) {
                    PICSimLab.RegisterError(
                        "PICSimLab",
                        (const char*)(lxString("File ") + prjdir + "platformio.ini can't be open!").utf8_str());
                    return 1;
                }
                fprintf(fpio, platformio_ini, (const char*)pioboard.c_str(), (const char*)pioplatform.c_str(),
                        (const char*)pioboard.c_str(), (const char*)pioframework.c_str(),
                        (const char*)env_extra.c_str(), (const char*)pioboard.c_str(), (const char*)ftype.c_str());
                if (gdb_debug) {
                    fprintf(fpio, platformio_ini_dbg, (const char*)tbreak.c_str(), (const char*)monitor_rst.c_str(),
                            (const char*)monitor_rst.c_str());
                } else {
                    fprintf(fpio, "debug_tool = custom  #no debug support\n");
                }

                fclose(fpio);

                // test
                if (pio_test) {
                    FILE* ftest = fopen_UTF8((prjdir + "test/test_custom_runner.py").utf8_str(), "w");
                    if (ftest == NULL) {
                        PICSimLab.RegisterError("PICSimLab", (const char*)(lxString("File ") + prjdir +
                                                                           "test/test_custom_runner.py can't be open!")
                                                                 .utf8_str());
                        return 1;
                    }
                    fprintf(ftest, blink_test, (const char*)hwpin.c_str());
                    fclose(ftest);
                } else {
                    PICSimLab.SystemCmd(PSC_REMOVEFILE,
                                        (const char*)(prjdir + "test/test_custom_runner.py").utf8_str());
                }

                PICSimLab.GetBoard()->SetPWActiveProject((const char*)prjdir.utf8_str());
                PICSimLab.GetBoard()->SetPWProjectType((const char*)ide.c_str());
                Window1.menu1_Code_Open_Active_Project.SetEnable(1);
                Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(1);

                if (operation == OP_CREATE_AND_OPEN) {
                    OpenProject(prjdir, ide);
                }
            } else if (!ide.compare("MPLAB X IDE")) {
                if (!ctemplate.compare("Blink FreeRTOS")) {
                    lxString fzip = PICSimLab.GetSharePath() + "prj_wizard/mplabx_freertos.zip";
                    PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.utf8_str(), (void*)((const char*)prjdir.utf8_str()));
                } else if (!ctemplate.compare("Blink PICGenios")) {
                    lxString fzip = PICSimLab.GetSharePath() + "prj_wizard/mplabx_picgenios.zip";
                    PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.utf8_str(), (void*)((const char*)prjdir.utf8_str()));
                } else {
                    lxString fzip = PICSimLab.GetSharePath() + "prj_wizard/mplabx.zip";
                    PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.utf8_str(), (void*)((const char*)prjdir.utf8_str()));
                }
                char prj_name[512];

                if (strlen(strrchr(prjdir.utf8_str(), '/')) > 2) {
                    strncpy(prj_name, strrchr(prjdir.utf8_str(), '/'), 511);
                } else {
                    char stmp[512];
                    strncpy(stmp, prjdir.utf8_str(), 511);
                    stmp[strlen(stmp) - 1] = 0;
                    strncpy(prj_name, strrchr(stmp, '/') + 1, 511);
                }

                // board selection
                lxString mplabx_cfg = "";
                lxString mplabx_freq = "";
                lxString mplabx_tris = "";
                lxString mplabx_pin = "";

                lxString cfg_headers = "";
                lxString cfg_sources = "";
                lxString cfg_root_lst = "";
                lxString cfg_include_dirs = "";
                lxString cfg_stack_type = "compiled";
                if (!board.compare("PICGenios")) {
                    if (strstr(processor.c_str(), "PIC18F")) {
                        if (!processor.compare("PIC18F45K50")) {
                            mplabx_cfg = "#pragma config WDTEN = OFF";
                            mplabx_tris = "ANSELDbits.ANSD3 = 0;\n    TRISDbits.TRISD0";
                        } else if (!processor.compare("PIC18F47K40")) {
                            mplabx_cfg = "#pragma config WDTE = OFF";
                            mplabx_tris = "ANSELDbits.ANSELD0 = 0;\n    TRISDbits.TRISD0";
                        } else {
                            mplabx_cfg = "#pragma config WDT = OFF";
                            mplabx_tris = "ADCON1 |= 0x0F;\n    TRISDbits.TRISD0";
                        }
                        mplabx_pin = "LATDbits.LD0";
                    } else {
                        mplabx_cfg = "#pragma config WDTE = OFF";
                        if (!processor.compare("PIC16F1789") || !processor.compare("PIC16F1939")) {
                            mplabx_tris = "ANSELDbits.ANSD0 = 0;\n    TRISDbits.TRISD0";
                        } else {
                            mplabx_tris = "ADCON1 |= 0x0F;\n    TRISDbits.TRISD0";
                        }
                        mplabx_pin = "PORTDbits.RD0";
                    }
                    mplabx_freq = "8000000L";
                } else if (!board.compare("McLab2")) {
                    if (strstr(processor.c_str(), "PIC18F")) {
                        if (!processor.compare("PIC18F45K50")) {
                            mplabx_cfg = "#pragma config WDTEN = OFF";
                            mplabx_tris = "ANSELBbits.ANSB3 = 0;\n    TRISBbits.TRISB3";
                        } else if (!processor.compare("PIC18F47K40")) {
                            mplabx_cfg = "#pragma config WDTE = OFF";
                            mplabx_tris = "ANSELBbits.ANSELB3 = 0;\n    TRISBbits.TRISB3";
                        } else {
                            mplabx_cfg = "#pragma config WDT = OFF";
                            mplabx_tris = "ADCON1 |= 0x0F;\n    TRISBbits.TRISB3";
                        }
                        mplabx_pin = "LATBbits.LB3";
                    } else {
                        mplabx_cfg = "#pragma config WDTE = OFF";
                        if (!processor.compare("PIC16F1789") || !processor.compare("PIC16F1939")) {
                            mplabx_tris = "ANSELBbits.ANSB3 = 0;\n    TRISBbits.TRISB3";
                        } else {
                            mplabx_tris = "ADCON1 |= 0x0F;\n    TRISBbits.TRISB3";
                        }
                        mplabx_pin = "PORTBbits.RB3";
                    }
                    mplabx_freq = "8000000L";
                } else if (!board.compare("McLab1")) {
                    mplabx_cfg = "#pragma config WDTE = OFF";
                    mplabx_tris = "TRISBbits.TRISB0";
                    mplabx_pin = "PORTBbits.RB0";
                    mplabx_freq = "4000000L";
                } else if (!board.compare("K16F")) {
                    mplabx_cfg = "#pragma config WDTE = OFF";
                    mplabx_tris = "TRISAbits.TRISA1";
                    mplabx_pin = "PORTAbits.RA1";
                    mplabx_freq = "4000000L";
                } else if (!board.compare("Curiosity")) {
                    mplabx_cfg = "#pragma config WDTE = OFF";
                    mplabx_tris = "TRISCbits.TRISC5";
                    mplabx_pin = "PORTCbits.RC5";
                    mplabx_freq = "8000000L";
                } else if (!board.compare("Curiosity HPC")) {
                    mplabx_cfg = "#pragma config WDTE = OFF";
                    mplabx_tris = "ANSELAbits.ANSELA7 = 0;\nTRISAbits.TRISA7";
                    mplabx_pin = "PORTAbits.RA7";
                    mplabx_freq = "8000000L";
                } else if (!board.compare("Xpress")) {
                    mplabx_cfg = "#pragma config WDTE = OFF";
                    mplabx_tris = "ANSELAbits.ANSA0 = 0;\nTRISAbits.TRISA0";
                    mplabx_pin = "PORTAbits.RA0";
                    mplabx_freq = "8000000L";
                } else if (!board.compare("PQDB")) {
                    mplabx_cfg = "#pragma config WDT = OFF";
                    mplabx_tris = "ADCON1 |= 0x0F;\nTRISAbits.TRISA5";
                    mplabx_pin = "LATAbits.LA5";
                    mplabx_freq = "8000000L";
                } else if (!board.compare("X")) {
                    if (strstr(processor.c_str(), "PIC18F")) {
                        mplabx_cfg = "#pragma config WDT = OFF";
                        mplabx_tris = "ADCON1 |= 0x0F;\n    TRISBbits.TRISB0";
                        mplabx_pin = "LATBbits.LB0";
                    } else {
                        mplabx_cfg = "#pragma config WDTE = OFF";
                        mplabx_tris = "TRISBbits.TRISB0";
                        mplabx_pin = "PORTBbits.RB0";
                    }
                    mplabx_freq = "8000000L";
                } else if (!board.compare("Breadboard")) {
                    if (strstr(processor.c_str(), "PIC")) {
                        if (strstr(processor.c_str(), "PIC18F")) {
                            if (!processor.compare("PIC18F45K50") || !processor.compare("PIC18F26K80") ||
                                !processor.compare("PIC18F46J50") || !processor.compare("PIC18F67J94")) {
                                mplabx_cfg = "#pragma config WDTEN = OFF\n#pragma config XINST = OFF";
                                mplabx_tris = "TRISCbits.TRISC0";
                            } else if (!processor.compare("PIC18F47K40") || !processor.compare("PIC18F24Q10") ||
                                       !processor.compare("PIC18F27K40")) {
                                mplabx_cfg = "#pragma config WDTE = OFF\n#pragma config XINST = OFF";
                                mplabx_tris = "ANSELCbits.ANSELC0 = 0;\nTRISCbits.TRISC0";
                            } else if (!processor.compare("PIC18F452")) {
                                mplabx_cfg = "#pragma config WDT = OFF";
                                mplabx_tris = "TRISCbits.TRISC0";
                            } else {
                                mplabx_cfg = "#pragma config WDT = OFF\n#pragma config XINST = OFF";
                                mplabx_tris = "TRISCbits.TRISC0";
                            }

                            mplabx_pin = "LATCbits.LC0";
                        } else {  // PIC16F
                            mplabx_cfg = "#pragma config WDTE = OFF";
                            if (!processor.compare("PIC16F1827") || !processor.compare("PIC16F1847") ||
                                !processor.compare("PIC16F628A") || !processor.compare("PIC16F648A") ||
                                !processor.compare("PIC16F84A") || !processor.compare("PIC16F819")) {
                                mplabx_tris = "TRISBbits.TRISB0";
                                mplabx_pin = "PORTBbits.RB0";

                            } else if (!processor.compare("PIC16F1619") || !processor.compare("PIC16F1829") ||
                                       !processor.compare("PIC16F18324")) {
                                mplabx_tris = "ANSELCbits.ANSC0 = 0;\nTRISCbits.TRISC0";
                                mplabx_pin = "PORTCbits.RC0";
                            } else {
                                mplabx_tris = "TRISCbits.TRISC0";
                                mplabx_pin = "PORTCbits.RC0";
                            }
                        }
                    } else {
                        // avr blink has not configuration yet
                    }
                    mplabx_freq = "8000000L";
                } else {
                    PICSimLab.RegisterError("PICSimLab", (const char*)("Not supported board: " + board).c_str());
                    PICSimLab.SystemCmd(PSC_REMOVEDIR, (const char*)prjdir.utf8_str());
                    WDestroy();
                    return 1;
                }

                // main
                FILE* fmain = fopen_UTF8((prjdir + "main.c").utf8_str(), "w");
                if (fmain == NULL) {
                    PICSimLab.RegisterError(
                        "PICSimLab", (const char*)(lxString("File ") + prjdir + "main.c can't be open!").utf8_str());
                    return 1;
                }

                if (!ctemplate.compare("Blink FreeRTOS")) {
                    fprintf(fmain, blink_mplabx_freertos, (const char*)mplabx_cfg.c_str(),
                            (const char*)mplabx_freq.c_str(), (const char*)mplabx_tris.c_str(),
                            (const char*)mplabx_pin.c_str(), (const char*)mplabx_pin.c_str());
                } else if (!ctemplate.compare("Blink PICGenios")) {
                    fprintf(fmain, blink_mplabx_picgenios);
                } else {
                    if (strstr(processor.c_str(), "PIC")) {
                        fprintf(fmain, blink_mplabx, (const char*)mplabx_cfg.c_str(), (const char*)mplabx_freq.c_str(),
                                (const char*)mplabx_tris.c_str(), (const char*)mplabx_pin.c_str(),
                                (const char*)mplabx_pin.c_str());
                    } else {
                        fprintf(fmain, blink_mplabx_avr);
                    }
                }
                fclose(fmain);

                // project
                FILE* fproj = fopen_UTF8((prjdir + "nbproject/project.xml").utf8_str(), "w");
                if (fproj == NULL) {
                    PICSimLab.RegisterError(
                        "PICSimLab",
                        (const char*)(lxString("File ") + prjdir + "nbproject/project.xml can't be open!").utf8_str());
                    return 1;
                }
                fprintf(fproj, project_xml, prj_name, (unsigned long)time(NULL));
                fclose(fproj);

                // configurations
                FILE* fconf = fopen_UTF8((prjdir + "nbproject/configurations.xml").utf8_str(), "w");
                if (fconf == NULL) {
                    PICSimLab.RegisterError("PICSimLab", (const char*)(lxString("File ") + prjdir +
                                                                       "nbproject/configurations.xml can't be open!")
                                                             .utf8_str());
                    return 1;
                }
                if (!ctemplate.compare("Blink FreeRTOS")) {
                    cfg_headers = "      <itemPath>FreeRTOSConfig.h</itemPath>\n";
                    cfg_sources =
                        "      <itemPath>FreeRTOS/Source/tasks.c</itemPath>\n"
                        "      <itemPath>FreeRTOS/Source/queue.c</itemPath>\n"
                        "      <itemPath>portable/port.c</itemPath>\n"
                        "      <itemPath>FreeRTOS/Source/list.c</itemPath>\n"
                        "      <itemPath>FreeRTOS/Source/portable/MemMang/heap_4.c</itemPath>\n";
                    cfg_root_lst = "    <Elem>FreeRTOS/Source</Elem>\n    <Elem>portable</Elem>\n";
                    cfg_include_dirs = "FreeRTOS/Source/include;.;portable";
                    cfg_stack_type = "reentrant";
                } else if (!ctemplate.compare("Blink PICGenios")) {
                    cfg_headers =
                        "      <itemPath>lib/adc.h</itemPath>\n"
                        "      <itemPath>lib/config.h</itemPath>\n"
                        "      <itemPath>lib/display7s.h</itemPath>\n"
                        "      <itemPath>lib/eeprom.h</itemPath>\n"
                        "      <itemPath>lib/eeprom_ext.h</itemPath>\n"
                        "      <itemPath>lib/itoa.h</itemPath>\n"
                        "      <itemPath>lib/lcd.h</itemPath>\n"
                        "      <itemPath>lib/serial.h</itemPath>\n"
                        "      <itemPath>lib/rtc.h</itemPath>\n"
                        "      <itemPath>lib/pwm.h</itemPath>\n"
                        "      <itemPath>lib/keyboard.h</itemPath>\n"
                        "      <itemPath>lib/config_1789.h</itemPath>\n"
                        "      <itemPath>lib/config_1939.h</itemPath>\n"
                        "      <itemPath>lib/config_452.h</itemPath>\n"
                        "      <itemPath>lib/config_4520.h</itemPath>\n"
                        "      <itemPath>lib/config_4550.h</itemPath>\n"
                        "      <itemPath>lib/config_4580.h</itemPath>\n"
                        "      <itemPath>lib/config_45K50.h</itemPath>\n"
                        "      <itemPath>lib/config_4620.h</itemPath>\n"
                        "      <itemPath>lib/config_47K40.h</itemPath>\n"
                        "      <itemPath>lib/config_777.h</itemPath>\n"
                        "      <itemPath>lib/config_877A.h</itemPath>\n"
                        "      <itemPath>lib/config_887.h</itemPath>\n"
                        "      <itemPath>lib/sw_i2c.h</itemPath>\n"
                        "      <itemPath>lib/sw_spi.h</itemPath>\n"
                        "      <itemPath>lib/spi.h</itemPath>\n"
                        "      <itemPath>lib/lcd_4.h</itemPath>\n";
                    cfg_sources =
                        "      <itemPath>lib/serial.c</itemPath>\n"
                        "      <itemPath>lib/rtc_r.c</itemPath>\n"
                        "      <itemPath>lib/pwm.c</itemPath>\n"
                        "      <itemPath>lib/adc.c</itemPath>\n"
                        "      <itemPath>lib/config.c</itemPath>\n"
                        "      <itemPath>lib/display7s.c</itemPath>\n"
                        "      <itemPath>lib/eeprom.c</itemPath>\n"
                        "      <itemPath>lib/eeprom_ext.c</itemPath>\n"
                        "      <itemPath>lib/lcd.c</itemPath>\n"
                        "      <itemPath>lib/itoa.c</itemPath>\n"
                        "      <itemPath>lib/keyboard.c</itemPath>\n"
                        "      <itemPath>lib/lcd_4.c</itemPath>\n"
                        "      <itemPath>lib/sw_i2c.c</itemPath>\n";
                    cfg_root_lst = "    <Elem>lib</Elem>\n";
                    cfg_include_dirs = "lib";
                }

                fprintf(fconf, configurations_xml, (const char*)cfg_headers.c_str(), (const char*)cfg_sources.c_str(),
                        (const char*)cfg_root_lst.c_str(), (const char*)processor.c_str(),
                        (const char*)cfg_include_dirs.c_str(), (const char*)cfg_stack_type.c_str());
                fclose(fconf);

                PICSimLab.GetBoard()->SetPWActiveProject((const char*)prjdir.utf8_str());
                PICSimLab.GetBoard()->SetPWProjectType((const char*)ide.c_str());
                Window1.menu1_Code_Open_Active_Project.SetEnable(1);
                Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(1);

                if (operation == OP_CREATE_AND_OPEN) {
                    OpenProject(prjdir, ide);
                }

            } else {
                PICSimLab.RegisterError("PICSimLab",
                                        (const char*)(lxString("IDE of type [") + ide + "] not supported!").utf8_str());
                WDestroy();
                return 1;
            }
        } else {
            PICSimLab.RegisterError("PICSimLab",
                                    (const char*)(lxString("Project dir ") + prjdir + " can't be created!").utf8_str());
            return 1;
        }
    }
    return 0;
}

void CPWindow6::dirdialog1_EvOnClose(int retId) {
    if (retId) {
        if (operation & OP_ONLY_CREATE) {
            lxString prjdir = dirdialog1.GetDirName();
            prjdir += lxString("/") + edit1.GetText() + "/";
            CreateProject(ide, framework, ctemplate, prjdir, operation);
        } else if (operation == OP_OPEN_EXISTING) {
            lxString prjdir = dirdialog1.GetDirName();
            PICSimLab.GetBoard()->SetPWActiveProject((const char*)prjdir.utf8_str());
            PICSimLab.GetBoard()->SetPWProjectType((const char*)ide.c_str());
            Window1.menu1_Code_Open_Active_Project.SetEnable(1);
            Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(1);
            OpenProject(prjdir, ide);
        }
    }
    WDestroy();
}

void CPWindow6::filedialog1_EvOnClose(const int retId) {
    if (retId) {
        lxString type = PICSimLab.GetBoard()->GetPWProjectType();
        if (!type.compare("PlatformIO IDE for VSCode")) {
            PICSimLab.SetPWVscodePath((const char*)filedialog1.GetFileName().utf8_str());
        } else if (!type.compare("MPLAB X IDE")) {
            PICSimLab.SetPWMplabxPath((const char*)filedialog1.GetFileName().utf8_str());
        } else {
            PICSimLab.RegisterError("PICSimLab", "IDE type not supported!");
            return;
        }
        OpenProject(PICSimLab.GetBoard()->GetPWActiveProject(), PICSimLab.GetBoard()->GetPWProjectType());
    }
}

int CPWindow6::OpenProject(lxString path, lxString type) {
    if (!PICSimLab.SystemCmd(PSC_DIREXISTS, path.utf8_str())) {
        PICSimLab.RegisterError("PICSimLab", (const char*)("Project dir not found!\n" + path).utf8_str());
        PICSimLab.GetBoard()->SetPWActiveProject(" ");
        Window1.menu1_Code_Open_Active_Project.SetEnable(0);
        Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(0);
        return 1;
    }

    if (!type.compare("PlatformIO IDE for VSCode")) {
        char vscode_path[1024];

        strncpy(vscode_path, PICSimLab.GetPWVscodePath().c_str(), 1023);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, vscode_path)) {
#ifdef _WIN_

            lxString vscode_user_path = "";
            strncpy(vscode_path, (const char*)lxGetUserDataDir("picsimlab"), 1023);
            printf("%s\n", (const char*)vscode_path);
            char* ptr = strstr(vscode_path, "AppData\\");
            if (ptr) {
                ptr[8] = 0;
                vscode_user_path = (lxString(vscode_path) + "Local\\Programs\\Microsoft VS Code\\Code.exe").utf8_str();
            }

            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, (const char*)vscode_user_path.utf8_str())) {
                strncpy(vscode_path, vscode_user_path.utf8_str(), 1023);
            } else if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "C:\\Program Files\\Microsoft VS Code\\Code.exe")) {
                strncpy(vscode_path, "C:\\Program Files\\Microsoft VS Code\\Code.exe", 1023);
            } else {
                if (Dialog_sz("VS code executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetFileName(vscode_path);
                    filedialog1.SetFilter(lxT("All Files (*.exe)|*.exe"));
                    filedialog1.Run();
                }
                return 1;
            }
#else

            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "/usr/bin/code")) {
                strncpy(vscode_path, "/usr/bin/code", 1023);
            } else if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "/snap/bin/code")) {
                strncpy(vscode_path, "/snap/bin/code", 1023);
            } else {
                if (Dialog_sz("VS code executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetFileName(vscode_path);
                    filedialog1.SetFilter(lxT("All Files (*)|*"));
                    filedialog1.Run();
                }
                return 1;
            }

#endif
            PICSimLab.SetPWVscodePath(vscode_path);
        }

        printf("PICSimLab: Open project [%s]\n", (const char*)(lxString(vscode_path) + " \"" + path + "\"").utf8_str());
        lxExecute((lxString(vscode_path) + " \"" + path + "\""));

        return 0;
    } else if (!type.compare("MPLAB X IDE")) {
        char mplabx_path[1024];

        strncpy(mplabx_path, PICSimLab.GetPWMplabxPath().c_str(), 1023);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, mplabx_path)) {
#ifdef _WIN_
            if (PICSimLab.SystemCmd(PSC_FILEEXISTS,
                                    "C:/Program Files/Microchip/MPLABX/v6.20/mplab_platform/bin/mplab_ide64.exe")) {
                strncpy(mplabx_path, "C:/Program Files/Microchip/MPLABX/v6.20/mplab_platform/bin/mplab_ide64.exe",
                        1023);
            } else {
                if (Dialog_sz("MPLAB X IDE executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetDir("C:/Program Files/Microchip/MPLABX/");
                    filedialog1.SetFilter(lxT("All Files (*.exe)|*.exe"));
                    filedialog1.Run();
                }
                return 1;
            }
#else
            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "/opt/microchip/mplabx/v6.20/mplab_platform/bin/mplab_ide")) {
                strncpy(mplabx_path, "/opt/microchip/mplabx/v6.20/mplab_platform/bin/mplab_ide", 1023);
            } else {
                if (Dialog_sz("MPLAB X IDE executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetDir("/opt/microchip/mplabx/");
                    filedialog1.SetFilter(lxT("All Files (*)|*"));
                    filedialog1.Run();
                }
                return 1;
            }

#endif
            PICSimLab.SetPWMplabxPath(mplabx_path);
        }

        printf("PICSimLab: Open project [%s]\n",
               (const char*)(lxString(mplabx_path) + " --open \"" + path + "\"").utf8_str());
        lxExecute((lxString(mplabx_path) + " --open \"" + path + "\""));

        return 0;
    } else {
        PICSimLab.RegisterError("PICSimLab", "Project type not supported!");
        Window1.menu1_Code_Open_Active_Project.SetEnable(0);
        Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(0);
        Window1.menu1_Code_Project_Wizard.SetEnable(0);
    }
    return -1;
}

int CPWindow6::OpenProjectDir(lxString path) {
    if (!PICSimLab.SystemCmd(PSC_DIREXISTS, path.utf8_str())) {
        PICSimLab.RegisterError("PICSimLab", (const char*)("Project dir not found!\n" + path).utf8_str());
        PICSimLab.GetBoard()->SetPWActiveProject(" ");
        Window1.menu1_Code_Open_Active_Project.SetEnable(0);
        Window1.menu1_Code_Open_Active_Project_Dir.SetEnable(0);
        return 1;
    }

#ifdef _WIN_
    std::string cmd = "explorer \"";
    cmd += (path + "\"").utf8_str();
    std::replace(cmd.begin(), cmd.end(), '/', '\\');
#else
    std::string cmd = "xdg-open \"";
    cmd += (path + "\"").utf8_str();
#endif

    lxExecute(cmd);
    printf("PICSimLab: Execute: %s\n", cmd.c_str());

    return 0;
}