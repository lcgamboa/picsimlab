/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes

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

#ifndef SPAREPARTS
#define SPAREPARTS

#include "parts/part.h"
#include "parts/parts_defs.h"

#define IOINIT 110

class CSpareParts {
public:
    CSpareParts();

    void Init(CWindow* win);

    void UpdateAll(const int force = 0);
    int GetCount(void) { return partsc; };
    part* GetPart(const int partn);
    void DeleteParts(void);
    void ResetPullupBus(unsigned char pin);
    void SetPullupBus(unsigned char pin, unsigned char value);
    unsigned char GetPullupBus(unsigned char pin);

    /**
     * @brief  Execute the process code of spare parts N times (where N is the number of steps in 100ms)
     */
    void Process(void);

    /**
     * @brief  Execute the pre process code of spare parts one time per 100ms
     */
    void PreProcess(void);

    /**
     * @brief  Execute the post process code of spare parts one time per 100ms
     */
    void PostProcess(void);

    /**
     * @brief  Return the name of all pins
     */
    lxString GetPinsNames(void);

    /**
     * @brief  Return the name of one pin
     */
    lxString GetPinName(unsigned char pin);

    const picpin* GetPinsValues(void);
    void SetPin(unsigned char pin, unsigned char value);
    void SetAPin(unsigned char pin, float value);
    void SetPinDOV(unsigned char pin, unsigned char ovalue);
    void SetPinDir(unsigned char pin, unsigned char dir);
    void WritePin(unsigned char pin, unsigned char value);
    void WritePinA(unsigned char pin, unsigned char avalue);
    void WritePinOA(unsigned char pin, unsigned short oavalue);
    unsigned char RegisterIOpin(lxString pname, unsigned char pin = 0, unsigned char dir = PD_OUT);
    unsigned char UnregisterIOpin(unsigned char pin);
    part* AddPart(const char* partname, const int x, const int y, const float scale);
    void DeletePart(const int partn);
    void SetUseAlias(const int use) { useAlias = use; };
    unsigned char GetUseAlias(void) { return useAlias; };
    bool SavePinAlias(lxString fname);
    bool LoadPinAlias(lxString fname, unsigned char show_error_msg = 0);
    bool LoadConfig(lxString fname);
    void ClearPinAlias(void);
    lxString GetAliasFname(void) { return alias_fname; };
    float GetScale(void) { return scale; };
    void SetScale(float s) { scale = s; };
    CWindow* GetWindow(void) { return Window; }
    CFileDialog* GetFileDialog(void) { return filedialog; }
    void Reset(void);

    void (CControl::*PropButtonRelease)(CControl* control, const uint button, const uint x, const uint y,
                                        const uint mask);
    void (CControl::*PropComboChange)(CControl* control);

    void (CControl::*PartButtonEvent)(CControl* control, const uint button, const uint x, const uint y,
                                      const uint mask);
    void (CControl::*PartEvent)(CControl* control);

    void (CControl::*PartKeyEvent)(CControl* control, const uint keyval, const uint hardware_keycode, const uint state);

    void Setfdtype(int value);

    int Getfdtype(void) { return fdtype; };

    void ReadPreferences(char* name, char* value);
    void WritePreferences(void);

    bool SaveConfig(lxString fname);

    lxString GetLoadConfigFile(void) { return LoadConfigFile; };

    void SetfdOldFilename(const lxString ofn);

    lxString GetOldFilename(void) { return oldfname; };

private:
    float scale;
    board* pboard;
    CWindow* Window;
    CFileDialog* filedialog;
    lxString PinNames[256];
    lxString PinAlias[256];
    lxString alias_fname;
    lxString LoadConfigFile;
    picpin* Pins;
    unsigned char PinsCount;
    unsigned char useAlias;
    int partsc;
    part* parts[MAX_PARTS];
    int partsc_aup;              // aways update list
    part* parts_aup[MAX_PARTS];  // aways update list
    unsigned char pullup_bus[IOINIT];
    int pullup_bus_count;
    unsigned char pullup_bus_ptr[IOINIT];
    int fdtype;
    lxString oldfname;
};

extern CSpareParts SpareParts;

#endif  // SPAREPARTS