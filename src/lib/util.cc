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

#include "util.h"
#include <string.h>

#ifdef _WIN_
#include <windows.h>
#endif

std::string FloatStrFormat(const char* str, const float value) {
    char stemp[256];
    snprintf(stemp, 256, str, value);
    return std::string(stemp);
}

static int FGetLine(FILE* file, std::string& str) {
    char line[5000];
    line[0] = 0;

    if (file) {
        fgets(line, 5000, file);

        if (line[0] != 0) {
            line[strlen(line) - 1] = '\0';
        } else
            line[0] = '\0';

        str = line;

        if (!feof(file))
            return true;
        else
            return false;
    } else {
        printf("fatal error in function fgetline!\n");
        exit(-1);
    }
}

int LoadFromFile(std::vector<std::string>& strlist, const char* fname) {
    FILE* fin;
    std::string line;
    fin = fopen_UTF8(fname, "r");

    if (fin) {
        strlist.clear();
        while (FGetLine(fin, line)) {
            strlist.push_back(line);
        }
        fclose(fin);
        return 1;
    }
    return 0;
}

int SaveToFile(std::vector<std::string> strlist, const char* fname) {
    FILE* fout;
    fout = fopen_UTF8(fname, "w");

    if (fout) {
        for (unsigned int i = 0; i < strlist.size(); i++) {
            fprintf(fout, "%s\n", strlist[i].c_str());
        }
        fclose(fout);
        return 1;
    }
    return 0;
}

FILE* fopen_UTF8(const char* fname, const char* mode) {
#ifndef _WIN_
    return fopen(fname, mode);
#else
    int newSize;

    std::string filePath(fname);
    std::wstring filePathW;
    filePathW.resize(filePath.size());
    newSize = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), filePath.length(),
                                  const_cast<wchar_t*>(filePathW.c_str()), filePath.length());
    filePathW.resize(newSize);

    std::string fileMode(mode);
    std::wstring fileModeW;
    fileModeW.resize(fileMode.size());
    newSize = MultiByteToWideChar(CP_UTF8, 0, fileMode.c_str(), fileMode.length(),
                                  const_cast<wchar_t*>(fileModeW.c_str()), fileMode.length());
    fileModeW.resize(newSize);

    return _wfopen(filePathW.c_str(), fileModeW.c_str());
#endif
}

std::string GetLocalFile(const std::string file) {
    /*
    #ifndef __WXMSW__
        if (file.Contains("http")) {
            lxString appname = lowercase(basename(Application->Aargv[0]));

            lxString name = file.substr(file.find(".com/") + 4, file.length());

            lxString local;

            local.Printf("%s/%s_local/%s", (const char*)lxGetTempDir(appname).c_str(), (const char*)appname.c_str(),
                         (const char*)name.c_str());

            if (lxSystemCmd(PSC_FILEEXISTS,local)) {
                return local;
            }

            SystemCmd(PSC_CREATEDIR,dirname(local));

            lxString cmd = "cd \"" + dirname(local) + "\"; wget --inet4-only \"" + file + "\"";

            system((const char*)cmd.c_str());

            return local;
        }
    #endif
    */
    // local file
    return file;
}