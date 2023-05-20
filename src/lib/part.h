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

#ifndef PART_H
#define PART_H

#include "board.h"

/**
 * @brief PCWProp
 *
 * Part Configure Window proprierties
 */

enum { PCW_END, PCW_COMBO, PCW_LABEL, PCW_SPIND, PCW_EDIT, PCW_SPIN, PCW_TEXT, PCW_DCOMBO };

typedef struct {
    int pcw_type;
    char label[20];
} PCWProp;

/**
 * @brief PART class
 *
 * class definition of parts used in picsimlab spare parts window.
 */
class part {
public:
    /**
     * @brief  Return the name of part
     */
    lxString GetName(void);

    /**
     * @brief  Return the help url of part
     */
    virtual lxString GetHelpURL(void);

    /**
     * @brief  Return the about information of part
     */
    virtual lxString GetAboutInfo(void) = 0;

    /**
     * @brief  Called ever 100ms to draw part
     */
    void Draw(void);

    /**
     * @brief  Called to draw every output
     */
    virtual void DrawOutput(const unsigned int index) = 0;

    /**
     * @brief  Called every start of CPU process
     */
    virtual void PreProcess(void){};

    /**
     * @brief  Called every CPU step
     */
    virtual void Process(void){};

    /**
     * @brief  Called every end of CPU process
     */
    virtual void PostProcess(void){};

    /**
     * @brief  Return the filename of part picture
     */
    virtual lxString GetPictureFileName(void);

    /**
     * @brief  Return the filename of part picture map
     */
    virtual lxString GetMapFile(void);

    /**
     * @brief  Return the filename of properties window XML file
     */
    virtual lxString GetPropertiesWindowFile(void);

    /**
     * @brief Get part input count
     */
    int GetInputCount(void);

    /**
     * @brief Get part input
     */
    input_t* GetInput(int n);

    /**
     * @brief Get part output count
     */
    int GetOutputCount(void);

    /**
     * @brief Get part input
     */
    output_t* GetOutput(int n);

    /**
     * @brief  Reset part status
     */
    virtual void Reset(void){};

    /**
     * @brief  Stop part simulation
     */
    virtual void Stop(void){};

    /**
     * @brief  Event handler on the part
     */
    void EvMouseButtonPress(uint button, uint x, uint y, uint state);

    /**
     * @brief  Event handler on the part
     */
    virtual void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state){};

    /**
     * @brief  Event handler on the part
     */
    void EvMouseButtonRelease(uint button, uint x, uint y, uint state);

    /**
     * @brief  Event handler on the part
     */
    virtual void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state){};

    /**
     * @brief  Event handler on the part
     */
    void EvMouseMove(uint button, uint x, uint y, uint state);

    /**
     * @brief  Event handler on the part
     */
    virtual void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state){};

    /**
     * @brief  Event handler on the part
     */
    virtual void EvKeyPress(uint key, uint mask){};

    /**
     * @brief  Event handler on the part
     */
    virtual void EvKeyRelease(uint key, uint mask){};

    /**
     * @brief  Called to save part preferences in configuration file
     */
    virtual lxString WritePreferences(void) = 0;

    /**
     * @brief  Called whe configuration file load  preferences
     */
    virtual void ReadPreferences(lxString value) = 0;

    /**
     * @brief  return the input ids numbers of names used in input map
     */
    virtual unsigned short GetInputId(char* name) = 0;

    /**
     * @brief  return the output ids numbers of names used in output map
     */
    virtual unsigned short GetOutputId(char* name) = 0;

    /**
     * @brief  Called to configure the properties window
     */
    virtual void ConfigurePropertiesWindow(CPWindow* WProp) = 0;

    /**
     * @brief  Called when properties window close
     */
    virtual void ReadPropertiesWindow(CPWindow* WProp) = 0;

    /**
     * @brief  Used by properties window combos
     */
    virtual void ComboChange(CPWindow* WProp, CCombo* control, lxString value){};

    /**
     * @brief  Used by properties window spin
     */
    virtual void SpinChange(CPWindow* WProp, CSpin* control, int value){};

    /**
     * @brief  Used by windows of parts
     */
    virtual void ButtonEvent(CControl* control, uint button, uint x, uint y, uint state){};

    /**
     * @brief  Used by windows of parts
     */
    virtual void KeyEvent(CControl* control, uint keysym, uint ukeysym, uint state){};

    /**
     * @brief  Used by windows of parts
     */
    virtual void Event(CControl* control){};

    /**
     * @brief  Used by properties window filedialogs
     */
    virtual void filedialog_EvOnClose(int retId){};

    /**
     * @brief  Called once on part creation
     */
    part(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int fsize = 8);

    /**
     * @brief  Called once on part initialization
     */
    void Init(void);

    /**
     * @brief  Called once on part after initialization
     */
    virtual void PostInit(void){};

    /**
     * @brief  Called once on part destruction
     */
    virtual ~part(void){};

    /**
     * @brief  Return the Bitmap of part
     */
    lxBitmap* GetBitmap(void) { return Bitmap; };

    /**
     * @brief  Return X position of part
     */
    int GetX(void) { return X; };

    /**
     * @brief  Return Y position of part
     */
    int GetY(void) { return Y; };

    /**
     * @brief  Set X position of part
     */
    void SetX(int x) { X = x; };

    /**
     * @brief  Set Y position of part
     */
    void SetY(int y) { Y = y; };

    /**
     * @brief  Return width of part
     */
    unsigned int GetWidth(void) { return Width; };

    /**
     * @brief  Return height of part
     */
    unsigned int GetHeight(void) { return Height; };

    /**
     * @brief  Return if point x,y is inside of part
     */
    int PointInside(int x, int y);

    /**
     * @brief  Return coords x y rotated by orientation
     */
    void RotateCoords(unsigned int* x, unsigned int* y);

    /**
     * @brief  Load Part Image
     */
    virtual void LoadImage(void);

    /**
     * @brief  Return the orientation to draw
     */
    int GetOrientation(void);

    /**
     * @brief  Set the orientation to draw
     */
    virtual void SetOrientation(int orientation);

    /**
     * @brief  Return the orientation to draw
     */
    float GetScale(void);

    /**
     * @brief  Set the orientation to draw
     */
    virtual void SetScale(double scale);

    /**
     * @brief  Set the part ID, don't be called by user
     */
    virtual void SetId(int _id) { id = _id; };

    /**
     * @brief  Get the part ID
     */
    int GetId(void) { return id; };

    /**
     * @brief  Return if part need Draw update
     */
    unsigned char GetUpdate(void);

    /**
     * @brief  Set part to Draw update
     */
    void SetUpdate(unsigned char upd);

    /**
     * @brief  Return if part need to be update every clock cycle
     */
    int GetAlwaysUpdate(void);

    /**
     * @brief  Set if part need to be update every clock cycle
     */
    void SetAlwaysUpdate(int sau);

    const int GetPCWCount(void);
    const PCWProp* GetPCWProperties(void);

    const int GetPinCount(void) { return PinCount; };
    unsigned char* GetPins(void) { return Pins; };
    const int GetPinCtrlCount(void) { return PinCtrlCount; };
    const unsigned char* GetPinsCtrl(void) { return PinsCtrl; };

protected:
    /**
     * @brief Register remote control variables
     */
    virtual void RegisterRemoteControl(void){};

    int id;                         ///< part ID
    input_t input[MAX_IDS];         ///< input map elements
    input_t* input_ids[MAX_IDS];    ///< input map elements by id order
    output_t output[MAX_IDS];       ///< output map elements
    output_t* output_ids[MAX_IDS];  ///< output map elements by id order
    int inputc;                     ///< input map elements counter
    int outputc;                    ///< output map elements counter
    unsigned int Height;            ///< Height of part
    unsigned int Width;             ///< Width of part
    int X;                          ///< X position of part
    int Y;                          ///< Y position of part
    lxBitmap* Bitmap;               ///< Internal Bitmap
    CCanvas canvas;                 ///< Internal Canvas to draw in bitmap
    unsigned int refresh;           ///< redraw is needed
    int Orientation;                ///< orientation to draw part
    double Scale;                   ///< scale to draw part
    unsigned int Update;            ///< part need draw Update
    int always_update;              ///< part need to be update every clock cycle
    lxString Type;
    lxFont font;
    int PinCount;
    int PinCtrlCount;
    unsigned char* Pins;
    unsigned char* PinsCtrl;
    board* pboard;

    /**
     * @brief  read maps
     */
    void ReadMaps(void);

    /**
     * @brief  Return if point x,y is inside of input
     */
    int PointInside(int x, int y, input_t input);

    void DrawSlider(const output_t* output, const unsigned char pos, const lxString val, const lxFont font);
    void DrawPotentiometer(const output_t* output, const unsigned char pos, const lxString val, const lxFont font);
    void SetPCWProperties(const PCWProp* pcwprop);
    void SetPCWComboWithPinNames(CPWindow* WProp, const char* combo_name, const unsigned char pin);
    unsigned char GetPWCComboSelectedPin(CPWindow* WProp, const char* combo_name);

private:
    const PCWProp* PCWProperties;
    int PCWCount;
    lxString Name;

    /**
     * @brief  Read the Input Map
     */
    void ReadInputMap(lxString fname);

    /**
     * @brief  Read the Output Map
     */
    void ReadOutputMap(lxString fname);
};

#endif /* PART_H */

#ifndef PARTS_DEFS_H
#define PARTS_DEFS_H

#define MAX_PARTS 100

extern int NUM_PARTS;

#define part_init(name, function, menu)                                              \
    static part* function##_create(unsigned int x, unsigned int y, board* pboard_) { \
        part* p = new function(x, y, name, menu, pboard_);                           \
        p->Init();                                                                   \
        return p;                                                                    \
    };                                                                               \
    static void __attribute__((constructor)) function##_init(void);                  \
    static void function##_init(void) {                                              \
        part_register(name, function##_create, menu);                                \
    }

typedef part* (*part_create_func)(unsigned int x, unsigned int y, board* pboard_);

void part_register(const char* name, part_create_func pcreate, const char* menu);

part* create_part(lxString name, unsigned int x, unsigned int y, board* pboard_);

typedef struct {
    char name[30];
    part_create_func pcreate;
    char menu[30];
} part_desc;

extern part_desc parts_list[MAX_PARTS];

#endif /* PARTS_DEFS_H */