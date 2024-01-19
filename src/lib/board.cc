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

#include "board.h"
#include "picsimlab.h"

int ioupdated = 0;

board::board(void) {
    ioupdated = 1;
    inputc = 0;
    outputc = 0;
    use_oscope = 0;
    use_spare = 0;
    Proc = "";
    p_RST = 1;
    Scale = PICSimLab.GetScale();
    InstCounter = 0;
    TimersCount = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        Timers[i].Arg = NULL;
        Timers[i].Callback = NULL;
        Timers[i].Enabled = 0;
        Timers[i].Reload = 0;
        Timers[i].Timer = 0;
        Timers[i].Tout = 0;
    }
    for (int i = 0; i < MAX_IDS; i++) {
        input_ids[i] = &input[i];
        output_ids[i] = &output[i];
    }
}

board::~board(void) {}

void board::ReadMaps(void) {
    inputc = 0;
    outputc = 0;
    ReadInputMap((const char*)lxGetLocalFile(PICSimLab.GetSharePath() + "boards/" + GetMapFile()).c_str());
    ReadOutputMap((const char*)lxGetLocalFile(PICSimLab.GetSharePath() + "boards/" + GetMapFile()).c_str());

    for (int i = 0; i < inputc; i++) {
        input_ids[GetInputId(input[i].name)] = &input[i];
    }

    for (int i = 0; i < outputc; i++) {
        output_ids[GetOutputId(output[i].name)] = &output[i];
    }
}

void board::ReadInputMap(std::string fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;
    char* value;

    int board_w = 0;
    int board_h = 0;

    int x1, y1, x2, y2, r;

    fin = fopen_UTF8(fname.c_str(), "r");

    if (fin) {
        while (fgets(line, 256, fin)) {
            it = strtok(line, "< =\"");
            if (!strcmp("img", it)) {
                do {
                    name = strtok(NULL, "< =\"");
                    value = strtok(NULL, "<=\"");

                    if (!strcmp("width", name)) {
                        sscanf(value, "%i", &board_w);
                        PICSimLab.SetplWidth(board_w);

                        unsigned int ww = 185 + board_w * PICSimLab.GetScale();
                        if (ww > lxGetDisplayWidth(0)) {
                            float scalex = ((lxGetDisplayWidth(0) - 185) * 1.0) / board_w;
                            if (PICSimLab.GetWindow()) {
                                ((CDraw*)PICSimLab.GetWindow()->GetChildByName("draw1"))->SetWidth(board_w * scalex);
                                PICSimLab.GetWindow()->SetWidth(lxGetDisplayWidth(0));
                            }
                            if (scalex < Scale) {
                                Scale = scalex;
                            }
                        } else {
                            if (PICSimLab.GetWindow()) {
                                ((CDraw*)PICSimLab.GetWindow()->GetChildByName("draw1"))
                                    ->SetWidth(board_w * PICSimLab.GetScale());
                                PICSimLab.GetWindow()->SetWidth(ww);
                            }
                        }
                    }

                    if (!strcmp("height", name)) {
                        sscanf(value, "%i", &board_h);
                        PICSimLab.SetplHeight(board_h);

                        unsigned int wh = 90 + board_h * PICSimLab.GetScale();

                        if (wh > lxGetDisplayHeight(0)) {
                            float scaley = ((lxGetDisplayHeight(0) - 90) * 1.0) / board_h;

                            if (PICSimLab.GetWindow()) {
                                ((CDraw*)PICSimLab.GetWindow()->GetChildByName("draw1"))->SetHeight(board_h * scaley);
                                PICSimLab.GetWindow()->SetHeight(lxGetDisplayHeight(0));
                                PICSimLab.GetWindow()->SetWidth(185 + board_w * scaley);
                            }
                            if (scaley < Scale) {
                                Scale = scaley;
                            }
                        } else {
                            if (PICSimLab.GetWindow()) {
                                ((CDraw*)PICSimLab.GetWindow()->GetChildByName("draw1"))
                                    ->SetHeight(board_h * PICSimLab.GetScale());
                                PICSimLab.GetWindow()->SetHeight(wh);
                            }
                        }
                    }

                } while (value != NULL);

                //        printf("%s %s %s\n",name,shape,coords);

            } else if (!strcmp("area", it)) {
                strtok(NULL, "< =\"");
                shape = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                coords = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                name = strtok(NULL, "< =\"");

                //        printf("%s %s %s\n",name,shape,coords);
                if (((name[0] == 'I') || (name[0] == 'B')) && (name[1] == '_')) {
                    if (strcmp("rect", shape) == 0) {
                        sscanf(coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
                        //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);

                        input[inputc].x1 = x1;
                        input[inputc].y1 = y1;
                        input[inputc].x2 = x2;
                        input[inputc].y2 = y2;
                    } else {
                        sscanf(coords, "%i,%i,%i\n", &x1, &y1, &r);
                        //          printf("circle=%i,%i,%i\n",x1,y1,r);
                        input[inputc].x1 = x1 - r;
                        input[inputc].y1 = y1 - r;
                        input[inputc].x2 = x1 + r;
                        input[inputc].y2 = y1 + r;
                    }
                    strcpy(input[inputc].name, name + 2);
                    input[inputc].id = GetInputId(input[inputc].name);
                    input[inputc].cx = ((input[inputc].x2 - input[inputc].x1) / 2.0) + input[inputc].x1;
                    input[inputc].cy = ((input[inputc].y2 - input[inputc].y1) / 2.0) + input[inputc].y1;
                    input[inputc].status = NULL;
                    input[inputc].update = NULL;
                    input[inputc].value = 0;
                    input[inputc].value_s = 0;
                    input[inputc].value_f = 0;
                    inputc++;
                    if (inputc >= MAX_IDS) {
                        printf("PICSimLab: Error! inputc greater than MAX_IDS \n");
                        exit(-1);
                    }
                }
            }
        }
        fclose(fin);
    } else {
        printf("PICSimLab: Error open input.map \"%s\"!\n", (const char*)fname.c_str());
        PICSimLab.RegisterError("Error open input.map:\n" + fname);
    }
}

void board::ReadOutputMap(std::string fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;

    int x1, y1, x2, y2, r;

    fin = fopen_UTF8(fname.c_str(), "r");

    if (fin) {
        while (fgets(line, 256, fin)) {
            it = strtok(line, "< =\"");
            if (!strcmp("area", it)) {
                strtok(NULL, "< =\"");
                shape = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                coords = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                name = strtok(NULL, "< =\"");

                //        printf("%s %s %s\n",name,shape,coords);
                if (((name[0] == 'O') || (name[0] == 'B')) && (name[1] == '_')) {
                    if (!strcmp("rect", shape)) {
                        sscanf(coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
                        //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);
                        output[outputc].x1 = x1;
                        output[outputc].y1 = y1;
                        output[outputc].x2 = x2;
                        output[outputc].y2 = y2;
                        output[outputc].r = 0;
                        strcpy(output[outputc].name, name + 2);
                        output[outputc].id = GetOutputId(output[outputc].name);
                        output[outputc].cx = ((output[outputc].x2 - output[outputc].x1) / 2.0) + output[outputc].x1;
                        output[outputc].cy = ((output[outputc].y2 - output[outputc].y1) / 2.0) + output[outputc].y1;
                        output[outputc].status = NULL;
                        output[outputc].value = 0;
                        output[outputc].value_s = 0;
                        output[outputc].value_f = 0;
                        outputc++;
                    } else {
                        sscanf(coords, "%i,%i,%i\n", &x1, &y1, &r);
                        //          printf("circle=%i,%i,%i\n",x1,y1,r);
                        output[outputc].x1 = x1;
                        output[outputc].y1 = y1;
                        output[outputc].x2 = 0;
                        output[outputc].y2 = 0;
                        output[outputc].r = r;
                        strcpy(output[outputc].name, name + 2);
                        output[outputc].id = GetOutputId(output[outputc].name);
                        output[outputc].cx = output[outputc].x1;
                        output[outputc].cy = output[outputc].y1;
                        output[outputc].status = NULL;
                        output[outputc].value = 0;
                        output[outputc].value_s = 0;
                        output[outputc].value_f = 0;
                        outputc++;
                    }
                    if (outputc >= MAX_IDS) {
                        printf("PICSimLab: Error! outputc greater than MAX_IDS \n");
                        exit(-1);
                    }
                }
            }
        }

        fclose(fin);
    } else {
        printf("PICSimLab: Error open output.map \"%s\"!\n", (const char*)fname.c_str());
        PICSimLab.RegisterError("Error open output.map:\n" + fname);
    }
}

void board::RefreshStatus(void) {
    PICSimLab.UpdateStatus(PS_SERIAL, "");
}

void board::SetUseOscilloscope(int uo) {
    use_oscope = uo;
}

void board::SetUseSpareParts(int sp) {
    use_spare = sp;
}

int board::GetUseOscilloscope(void) {
    return use_oscope;
}

int board::GetUseSpareParts(void) {
    return use_spare;
}

void board::SetProcessorName(std::string proc) {
    Proc = proc;
}

std::string board::GetProcessorName(void) {
    return Proc;
}

int board::GetInputCount(void) {
    return inputc;
}

input_t* board::GetInput(int n) {
    if (n < inputc) {
        return &input[n];
    }
    return NULL;
}

int board::GetOutputCount(void) {
    return outputc;
}

output_t* board::GetOutput(int n) {
    if (n < outputc) {
        return &output[n];
    }
    return NULL;
}

unsigned char board::CalcAngle(int in, int x, int y) {
    int dx = input[in].cx - x;
    int dy = y - input[in].cy;
    double angle = 0;

    if ((dx >= 0) && (dy >= 0)) {
        angle = atan2(dx, dy) * 180 / M_PI;
    } else if ((dx >= 0) && (dy < 0)) {
        angle = 180 - (atan2(dx, -dy) * 180 / M_PI);
    } else if ((dx < 0) && (dy < 0)) {
        angle = (atan2(-dx, -dy) * 180 / M_PI) + 180;
    } else if ((dx < 0) && (dy >= 0)) {
        angle = 360 - (atan2(-dx, dy) * 180 / M_PI);
    }

    if (angle > 340)
        angle = 340;
    if (angle < 20)
        angle = 20;

    return (199 * (angle - 20) / 320.0);
}

void board::SetScale(double scale) {
    Scale = scale;
}

double board::GetScale(void) {
    return Scale;
}

void board::EvOnShow(void) {
    for (int i = 0; i < outputc; i++) {
        output[i].update = 1;
    }
}

std::string board::GetPictureFileName(void) {
    return GetName() + "/board.svg";
}

std::string board::GetMapFile(void) {
    return GetName() + "/board.map";
}

void board::StartThread(void) {
#ifndef __EMSCRIPTEN__
    CThread* thread3 = ((CThread*)PICSimLab.GetWindow()->GetChildByName("thread3"));
    if (!thread3->GetRunState()) {
        thread3->Run();
    }
#endif
}

void board::StopThread(void) {
#ifndef __EMSCRIPTEN__
    CThread* thread3 = ((CThread*)PICSimLab.GetWindow()->GetChildByName("thread3"));
    if (thread3->GetRunState()) {
        thread3->Destroy();
    }
#endif
}

void board::InstCounterInc(void) {
    InstCounter++;
    for (int t = 0; t < TimersCount; t++) {
        if (TimersList[t]->Enabled) {
            TimersList[t]->Timer--;
            if (!TimersList[t]->Timer) {
                (*TimersList[t]->Callback)(TimersList[t]->Arg);
                TimersList[t]->Timer = TimersList[t]->Reload;
            }
        }
    }
}

int board::TimerRegister_us(const double micros, void (*Callback)(void* arg), void* arg) {
    if (TimersCount < MAX_TIMERS) {
        int timern = 0;
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (Timers[i].Callback == NULL) {
                timern = i + 1;
                break;
            }
        }
        TimerChange_us(timern, micros);
        Timers[timern - 1].Callback = Callback;
        Timers[timern - 1].Arg = arg;
        Timers[timern - 1].Enabled = 1;
        TimersList[TimersCount] = &Timers[timern - 1];
        TimersCount++;
        return timern;
    }
    return -1;
}

int board::TimerRegister_ms(const double miles, void (*Callback)(void* arg), void* arg) {
    if (TimersCount < MAX_TIMERS) {
        int timern = 0;
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (Timers[i].Callback == NULL) {
                timern = i + 1;
                break;
            }
        }
        TimerChange_ms(timern, miles);
        Timers[timern - 1].Callback = Callback;
        Timers[timern - 1].Arg = arg;
        Timers[timern - 1].Enabled = 1;
        TimersList[TimersCount] = &Timers[timern - 1];
        TimersCount++;
        return timern;
    }
    return -1;
}

int board::TimerUnregister(const int timer) {
    if (timer <= MAX_TIMERS) {
        Timers[timer - 1].Callback = NULL;  // free timer

        int tltimer = 0;
        for (int t = 0; t < TimersCount; t++) {
            if (TimersList[t] == &Timers[timer - 1]) {
                tltimer = t;
            }
        }

        for (int t = tltimer; t < (TimersCount - 1); t++) {
            TimersList[t] = TimersList[t + 1];
        }
        TimersCount--;
        TimersList[TimersCount] = NULL;
        return 0;
    }
    return -1;
}

int board::TimerChange_us(const int timer, const double micros) {
    if (timer <= MAX_TIMERS) {
        Timers[timer - 1].Reload = micros * 1e-6 * MGetInstClockFreq();
        if (Timers[timer - 1].Reload <= 0) {
            Timers[timer - 1].Reload = 1;
        }
        Timers[timer - 1].Timer = Timers[timer - 1].Reload;
        Timers[timer - 1].Tout = micros;
        return 0;
    }
    return -1;
}

int board::TimerChange_ms(const int timer, const double miles) {
    if (timer <= MAX_TIMERS) {
        Timers[timer - 1].Reload = miles * 1e-3 * MGetInstClockFreq();
        if (Timers[timer - 1].Reload <= 0) {
            Timers[timer - 1].Reload = 1;
        }
        Timers[timer - 1].Timer = Timers[timer - 1].Reload;
        Timers[timer - 1].Tout = miles * 1e3;
        return 0;
    }
    return -1;
}

int board::TimerSetState(const int timer, const int enabled) {
    if (timer <= MAX_TIMERS) {
        Timers[timer - 1].Enabled = enabled;
        if (enabled) {
            Timers[timer - 1].Timer = Timers[timer - 1].Reload;
        }
        return 0;
    }
    return -1;
}

uint64_t board::TimerGet_ns(const int timer) {
    if (timer <= MAX_TIMERS) {
        return (Timers[timer - 1].Reload * 1e9) / MGetInstClockFreq();
    }
    return -1;
}

uint32_t board::GetInstCounter_us(const uint32_t start) {
    return ((InstCounter - start) * 1e6) / MGetInstClockFreq();
}

uint32_t board::GetInstCounter_ms(const uint32_t start) {
    return ((InstCounter - start) * 1e3) / MGetInstClockFreq();
}

void board::TimerUpdateFrequency(float freq) {
    for (int t = 0; t < TimersCount; t++) {
        TimersList[t]->Reload = TimersList[t]->Tout * 1e-6 * MGetInstClockFreq();
        if (TimersList[t]->Reload <= 0) {
            TimersList[t]->Reload = 1;
        }
        TimersList[t]->Timer = TimersList[t]->Reload;
    }
}

// BOARDS_DEFS

board_desc boards_list[BOARDS_MAX];

int BOARDS_LAST = 0;

// boards object creation

board* create_board(int* lab, int* lab_) {
    board* pboard = NULL;

    if ((*lab >= 0) && (*lab < BOARDS_LAST)) {
        pboard = boards_list[*lab].bcreate();
    } else {
        mprint("PICSimLab: Invalid board! Using default!\n");
        *lab = 0;   // default
        *lab_ = 0;  // default
        pboard = boards_list[0].bcreate();
    }

    return pboard;
}

void board_register(const char* name, board_create_func bcreate) {
    int in;

    if (BOARDS_LAST == BOARDS_MAX) {
        printf("Number of boards greater than BOARDS_MAX!\n");
        exit(-1);
    }

    // insert in ascendent order
    in = BOARDS_LAST;
    for (int i = BOARDS_LAST; i > 0; i--) {
        if (strcmp(name, boards_list[i - 1].name) > 0) {
            break;
        } else {
            in = i - 1;
            memcpy((void*)&boards_list[i], (void*)&boards_list[i - 1], sizeof(board_desc));
        }
    }

    // insert new
    boards_list[in].bcreate = bcreate;
    strncpy(boards_list[in].name, name, 30);

    for (unsigned int i = 0; i <= strlen(name); i++) {
        if ((name[i] != ' ') && (name[i] != '-')) {
            boards_list[in].name_[i] = name[i];
        } else {
            boards_list[in].name_[i] = '_';
        }
    }

    BOARDS_LAST++;
}