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

#ifndef BOARD_H
#define BOARD_H

#include <picsim/picsim.h>
#include <stdint.h>

#include <stdlib.h>
#include <string.h>
#include <string>

#define INCOMPLETE                                                      \
    printf("Incomplete: %s -> %s :%i\n", __func__, __FILE__, __LINE__); \
    exit(-1);

enum { ARCH_P16, ARCH_P16E, ARCH_P18, ARCH_AVR8, ARCH_STM32, ARCH_STM8, ARCH_C51, ARCH_Z80, ARCH_UNKNOWN };

/**
 * @brief input map struct
 *
 */
typedef struct {
    float x1;               ///< x1 position
    float x2;               ///< x2 position
    float y1;               ///< y1 position
    float y2;               ///< y2 position
    float cx;               ///< center x position
    float cy;               ///< center y position
    char name[10];          ///< region name
    unsigned short id;      ///< region ID
    void* status;           ///< rcontrol status
    unsigned char* update;  ///< output need draw update
    union {
        unsigned char value;  ///< updated value
        short value_s;        ///< updated value short
        float value_f;        ///< updated value float
    };
} input_t;

/**
 * @brief output map struct
 *
 */
typedef struct {
    float x1;              ///< x1 position
    float x2;              ///< x2 position
    float y1;              ///< y1 position
    float y2;              ///< y2 position
    float cx;              ///< center x position
    float cy;              ///< center y position
    float r;               ///< radius
    char name[10];         ///< region name
    unsigned short id;     ///< region ID
    void* status;          ///< rcontrol status
    unsigned char update;  ///< need draw update
    union {
        unsigned char value;  ///< updated value
        short value_s;        ///< updated value short
        float value_f;        ///< updated value float
    };
} output_t;

#define MAX_TIMERS 256

#define MAX_IDS 128

#define INVALID_ID (MAX_IDS - 1)

#define IOINIT 110         // IOINIT is the max CPU pins number
#define MAX_PIN_COUNT 256  // MAX_PIN_COUNT- IOINIT is the max spare parts extra pins number

/**
 * @brief internal timer struct
 *
 */
typedef struct {
    uint32_t Timer;
    uint32_t Reload;
    void* Arg;
    void (*Callback)(void* arg);
    int Enabled;
    double Tout;  // in us
} Timers_t;

/**
 * @brief Board class
 *
 * class definition of boards used in picsimlab.
 */
class board {
public:
    /**
     * @brief Called ever 100ms to draw board
     */
    virtual void Draw(void) = 0;

    /**
     * @brief Paralle thread called ever 100ms to run cpu code
     */
    virtual void Run_CPU(void) = 0;

    /**
     * @brief  Return a list of supported microcontrollers
     */
    virtual std::string GetSupportedDevices(void) = 0;

    /**
     * @brief Return the filename of board picture
     */
    virtual std::string GetPictureFileName(void);

    /**
     * @brief  Return the filename of board picture map
     */
    virtual std::string GetMapFile(void);

    /**
     * @brief  Reset board status
     */
    virtual void Reset(void) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {};

    /**
     * @brief  Event on the board
     */
    virtual void EvKeyPress(unsigned int key, unsigned int mask) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvKeyRelease(unsigned int key, unsigned int mask) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvOnShow(void);

    /**
     * @brief  Event on the board
     */
    virtual void EvThreadRun(void) {};

    /**
     * @brief  Called ever 1s to refresh status
     */
    virtual void RefreshStatus(void);

    /**
     * @brief  Called to save board preferences in configuration file
     */
    virtual void WritePreferences(void) {};

    /**
     * @brief  Called whe configuration file load  preferences
     */
    virtual void ReadPreferences(char* name, char* value) {};

    /**
     * @brief  return the input ids numbers of names used in input map
     */
    virtual unsigned short GetInputId(char* name) = 0;

    /**
     * @brief  return the output ids numbers of names used in output map
     */
    virtual unsigned short GetOutputId(char* name) = 0;

    /**
     * @brief  Called when window side controls are activated
     */
    virtual void board_Event(const char* controlname) {};

    /**
     * @brief  Called when window side controls are activated
     */
    virtual void board_ButtonEvent(const char* controlname, unsigned int button, unsigned int x, unsigned int y,
                                   unsigned int state) {};

    /**
     * @brief  Called once on board creation
     */
    board(void);

    /**
     * @brief  Called once on board destruction
     */
    virtual ~board(void);

    /**
     * @brief  Enable/disable oscilloscope measurement
     */
    void SetUseOscilloscope(int uo);

    /**
     * @brief  Enable/disable spare parts
     */
    void SetUseSpareParts(int sp);

    /**
     * @brief Get if oscilloscope is in use
     */
    int GetUseOscilloscope(void);

    /**
     * @brief Get if spare parts is in use
     */
    int GetUseSpareParts(void);

    /**
     * @brief Get board name registered in PICSimLab
     */
    virtual std::string GetName(void) = 0;

    /**
     * @brief Set board processor
     */
    void SetProcessorName(std::string proc);

    /**
     * @brief Get board processor in use
     */
    std::string GetProcessorName(void);

    /**
     * @brief Get board input count
     */
    int GetInputCount(void);

    /**
     * @brief Get board input
     */
    input_t* GetInput(int n);

    /**
     * @brief Get board output count
     */
    int GetOutputCount(void);

    /**
     * @brief Get board input
     */
    output_t* GetOutput(int n);

    /**
     * @brief Start debug support
     */
    virtual int DebugInit(int dtyppe) = 0;

    /**
     * @brief Get debug interface name
     */
    virtual std::string GetDebugName(void) = 0;

    /**
     * @brief  Return the about information of part
     */
    virtual std::string GetAboutInfo(void) = 0;

    /**
     * @brief debug step (pooling)
     */
    virtual void DebugLoop(void) = 0;

    /**
     * @brief return true if microcontroller is initialized
     */
    virtual int CpuInitialized(void) = 0;

    /**
     * @brief Set serial port name to use
     */
    virtual void MSetSerial(const char* port) = 0;

    /**
     * @brief board microcontroller init
     */
    virtual int MInit(const char* processor, const char* fname, float freq) = 0;

    /**
     * @brief board microcontroller end
     */
    virtual void MEnd(void) = 0;

    /**
     * @brief Return board microcontroller architecture
     */
    virtual int MGetArchitecture(void) = 0;

    /**
     * @brief board servers shutdown
     */
    virtual void EndServers(void) {};

    /**
     * @brief board microcontroller save non volatile memory to hex file
     */
    virtual int MDumpMemory(const char* fname) = 0;

    /**
     * @brief board microcontroller erase flash memory (program)
     */
    virtual void MEraseFlash(void) = 0;

    /**
     * @brief board microcontroller set frequency
     */
    virtual void MSetFreq(float freq) = 0;

    /**
     * @brief board microcontroller get frequency
     */
    virtual float MGetFreq(void) = 0;

    /**
     * @brief board microcontroller set vcc
     */
    virtual void MSetVCC(float vcc) = 0;

    /**
     * @brief board microcontroller get vcc
     */
    virtual float MGetVCC(void) = 0;

    /**
     * @brief board microcontroller get cpu internal clock (in PIC frequency/4)
     */
    virtual float MGetInstClockFreq(void) = 0;

    /**
     * @brief board microcontroller get Clock pulses per instruction (in PIC equal to 4)
     */
    int MGetClocksPerInstructions(void);

    /**
     * @brief board microcontroller pin count
     */
    virtual int MGetPinCount(void) = 0;

    /**
     * @brief board microcontroller pin name
     */
    virtual std::string MGetPinName(int pin) = 0;

    /**
     * @brief board microcontroller set digital pin
     */
    virtual void MSetPin(int pin, unsigned char value) = 0;

    /**
     * @brief board microcontroller set Default Open Value (external pull)
     */
    virtual void MSetPinDOV(int pin, unsigned char ovalue) = 0;

    /**
     * @brief board microcontroller set analog pin
     */
    virtual void MSetAPin(int pin, float value) = 0;

    /**
     * @brief board microcontroller set digital pin output average value
     */
    virtual void MSetPinOAV(int pin, float value) = 0;

    /**
     * @brief board microcontroller get digital pin value
     */
    virtual unsigned char MGetPin(int pin) = 0;

    /**
     * @brief board microcontroller get all pins list struct
     */
    virtual const picpin* MGetPinsValues(void) = 0;

    /**
     * @brief board microcontroller get pointer to digital pin average value
     */
    virtual float* MGetPinOAVPtr(int pin) = 0;

    /**
     * @brief board microcontroller run one step
     */
    virtual void MStep(void) = 0;

    /**
     * @brief board microcontroller run one or two steps to resume instruction
     */
    virtual void MStepResume(void) = 0;

    /**
     * @brief board microcontroller reset
     */
    virtual int MReset(int flags) = 0;

    /**
     * @brief Get the Reset pin of microcontroller
     */
    virtual int MGetResetPin(void) = 0;

    /**
     * @brief get microcontroler status IO updated
     */
    virtual int MGetIOUpdated(void) = 0;

    /**
     * @brief clear microcontroler status IO updated
     */
    virtual void MClearIOUpdated(void) = 0;

    /**
     * @brief board microcontroller get pointer to processor ID
     */
    virtual unsigned short* DBGGetProcID_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get PC
     */
    virtual unsigned int DBGGetPC(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller set PC
     */
    virtual void DBGSetPC(unsigned int pc) { INCOMPLETE; };

    /**
     * @brief  board microcontroller get RAM memory pointer
     */
    virtual unsigned char* DBGGetRAM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get ROM (FLASH) memory pointer
     */
    virtual unsigned char* DBGGetROM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller  get CONFIG memory pointer
     */
    virtual unsigned char* DBGGetCONFIG_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller  get internal IDS memory pointer
     */
    virtual unsigned char* DBGGetID_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get data EEPROM memory pointer
     */
    virtual unsigned char* DBGGetEEPROM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get RAM memory size
     */
    virtual unsigned int DBGGetRAMSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get ROM  (FLASH) memory size
     */
    virtual unsigned int DBGGetROMSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief board microcontroller get CONFIG memory size
     */
    virtual unsigned int DBGGetCONFIGSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief board microcontroller get internal IDS memory size
     */
    virtual unsigned int DBGGetIDSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get data EEPROM memory size
     */
    virtual unsigned int DBGGetEEPROM_Size(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get last ram write address
     */
    virtual unsigned int DBGGetRAMLAWR(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get last ram read address
     */
    virtual unsigned int DBGGetRAMLARD(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  Calc rotary potentiometer angle
     */
    unsigned char CalcAngle(int i, int x, int y);

    /**
     * @brief  Set board draw scale
     */
    virtual void SetScale(double scale);

    /**
     * @brief  Get board draw scale
     */
    double GetScale(void);

    /**
     * @brief  Get board default clock in MHz
     */

    virtual int GetDefaultClock(void) { return 2; };

    /**
     * @brief  Get board default processor
     */

    std::string GetDefaultProcessor(void) { return DProc; };

    /**
     * @brief  Set board default processor
     */

    void SetDefaultProcessor(std::string dproc) { DProc = dproc; };

    /**
     * @brief Get instruction counter
     */
    uint32_t GetInstCounter(void) { return InstCounter; };

    /**
     * @brief Get elapsed time from instruction counter in us
     */
    uint32_t GetInstCounter_us(const uint32_t start);

    /**
     * @brief Get elapsed time from instruction counter in us
     */
    uint32_t GetInstCounter_ms(const uint32_t start);

    /**
     * @brief Register a new timer with time in us (default enabled)
     */
    int TimerRegister_us(const double micros, void (*Callback)(void* arg), void* arg);

    /**
     * @brief Register a new timer with time in ms (default enabled)
     */
    int TimerRegister_ms(const double miles, void (*Callback)(void* arg), void* arg);

    /**
     * @brief Unregister timer
     */
    int TimerUnregister(const int timer);

    /**
     * @brief Modify timer value with us
     */
    int TimerChange_us(const int timer, const double micros);

    /**
     * @brief Modify timer value with us
     */
    int TimerChange_ms(const int timer, const double miles);

    /**
     * @brief Enable or disable timer
     */
    int TimerSetState(const int timer, const int enabled);

    /**
     * @brief Get elapsed time from instruction counter in ns
     */
    uint64_t TimerGet_ns(const int timer);

    /**
     * @brief Update Timer counters on frequency change
     */
    void TimerUpdateFrequency(float freq);

    /**
     * @brief Lock IO to others threads access
     */
    virtual void IoLockAccess(void) {};

    /**
     * @brief Unlock IO to others threads access
     */
    virtual void IoUnlockAccess(void) {};

    /**
     * @brief Return the UART N RX pin number
     */
    virtual int GetUARTRX(const int uart_num) { return 0; };

    /**
     * @brief Return the UART N TX pin number
     */
    virtual int GetUARTTX(const int uart_num) { return 0; };

    virtual std::string GetUARTStrStatus(const int uart_num) { return "Not implemented!"; };

    /**
     * @brief Return the description of clk label
     */
    virtual std::string GetClkLabel(void) { return "Clk (Mhz)"; };

protected:
    /**
     * @brief Register remote control variables
     */
    virtual void RegisterRemoteControl(void) {};

    /**
     * @brief Increment the Intructions Counter
     */
    void InstCounterInc(void);

    std::string Proc;               ///< Name of processor in use
    std::string DProc;              ///< Name of default board processor
    input_t input[MAX_IDS];         ///< input map elements
    input_t* input_ids[MAX_IDS];    ///< input map elements by id order
    output_t output[MAX_IDS];       ///< output map elements
    output_t* output_ids[MAX_IDS];  ///< output map elements by id order
    int inputc;                     ///< input map elements counter
    int outputc;                    ///< output map elements counter
    int use_oscope;                 ///< use oscilloscope window
    int use_spare;                  ///< use spare parts window
    unsigned char p_RST;            ///< board /RESET pin state
    float Scale;

    /**
     * @brief  Read maps
     */
    void ReadMaps(void);

    /**
     * @brief  Start parallel thread
     */
    void StartThread(void);

    /**
     * @brief  Stop parallel thread
     */
    void StopThread(void);

private:
    uint32_t InstCounter;
    int TimersCount;
    Timers_t Timers[MAX_TIMERS];
    Timers_t* TimersList[MAX_TIMERS];

    /**
     * @brief Read the Input Map
     */
    void ReadInputMap(std::string fname);

    /**
     * @brief Read the Output Map
     */
    void ReadOutputMap(std::string fname);
};

extern int ioupdated;

#endif /* BOARD_H */

#ifndef BOARDS_DEFS_H
#define BOARDS_DEFS_H

#define board_init(name, function)                                  \
    static board* function##_create(void) {                         \
        board* b = new function();                                  \
        b->SetDefaultProcessor(b->GetProcessorName());              \
        return b;                                                   \
    };                                                              \
    static void __attribute__((constructor)) function##_init(void); \
    static void function##_init(void) {                             \
        board_register(name, function##_create);                    \
    }

typedef board* (*board_create_func)(void);

void board_register(const char* name, board_create_func bcreate);

// boards object creation
board* create_board(int* lab, int* lab_);

#define BOARDS_MAX 25

extern int BOARDS_LAST;

typedef struct {
    char name[30];   // name
    char name_[30];  // name without spaces
    board_create_func bcreate;
} board_desc;

extern board_desc boards_list[BOARDS_MAX];

// Arduino Uno is the dafault board
#define DEFAULT_BOARD 2

#endif /* BOARDS_DEFS_H */
