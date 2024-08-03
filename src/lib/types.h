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

#ifndef TYPES_H
#define TYPES_H

enum PICSimlabCPUState {
    CPU_RUNNING,
    CPU_STEPPING,
    CPU_HALTED,
    CPU_BREAKPOINT,
    CPU_ERROR,
    CPU_POWER_OFF,
    CPU_WAITING_GDB,
    CPU_LAST
};

enum PICSimlabStatus { PS_RUN = 0, PS_DEBUG, PS_SERIAL, PS_LAST };

enum PICSimlabGUIType { GT_GAUGE = 0, GT_SCROLL, GT_LABEL, GT_COMBO, GT_BUTTON, GT_LAST };
enum PICSimlabGUIAction { GA_ADD = 0, GA_DEL, GA_SET, GA_GET, GA_SET_LABEL, GA_LAST };

enum PICSimlabGUIMenu { GMT_HEX, GMT_BIN, GMT_DISABLED, GMT_LAST };

enum PICSimLabWindowAction {
    PWA_COMBOSETITEMS,
    PWA_COMBOSETTEXT,
    PWA_COMBOGETTEXT,
    PWA_COMBOPROPEV,
    PWA_COMBOPARTEV,
    PWA_COMBODELETEITEMS,
    PWA_COMBOADDITEM,

    PWA_SPINDSETMAX,
    PWA_SPINDSETMIN,
    PWA_SPINDSETVALUE,
    PWA_SPINDGETVALUE,

    PWA_SPINSETMAX,
    PWA_SPINSETMIN,
    PWA_SPINSETVALUE,
    PWA_SPINGETVALUE,
    PWA_SPINPROPEV,

    PWA_TEXTCLEAR,
    PWA_TEXTADDLINE,
    PWA_TEXTGETLINE,
    PWA_TEXTGETLINECOUNT,
    PWA_TEXTAPPEND,
    PWA_TEXTTELETYPE,

    PWA_EDITSETTEXT,
    PWA_EDITGETTEXT,
    PWA_EDITPARTEV,

    PWA_LABELSETTEXT,

    PWA_CHECKBOXSETCHECK,
    PWA_CHECKBOXGETCHECK,

    PWA_SETENABLE,
    PWA_SETWIDTH,
    PWA_SETHEIGHT,
    PWA_SETVISIBLE,
    PWA_SETX,
    PWA_SETY,
    PWA_SETTAG,
    PWA_SETCOLOR,
    PWA_GETWIDTH,
    PWA_GETHEIGHT,
    PWA_GETX,
    PWA_GETY,
    PWA_GETTAG,
    PWA_GETCOLOR,

    PWA_WINDOWCREATE,
    PWA_WINDOWDESTROY,
    PWA_WINDOWLOADXML,
    PWA_WINDOWSHOW,
    PWA_WINDOWHIDE,
    PWA_WINDOWPARTEV,
    PWA_WINDOWSHOWEX,
    PWA_WINDOWHIDEEX,
    PWA_WINDOWHASCREATED,

    PWA_GETDISPLAYWIDTH,
    PWA_GETDISPLAYHEIGHT,

    PWA_APPPROCESSEVENTS,

    PWA_BUTTONPARTEV,
    PWA_BUTTONBOARDEV,

    PWA_FILEDIALOGGETFNAME,
    PWA_FILEDIALOGSETFNAME,
    PWA_FILEDIALOGSETFILTER,
    PWA_FILEDIALOGSETTYPE,
    PWA_FILEDIALOGRUN,
    PWA_FILEDIALOGGETTYPE,

    PWA_THREADGETRUNSTATE,
    PWA_THREADRUN,
    PWA_THREADDESTROY,
    PWA_THREADTESTDESTROY,

    PWA_TIMERGETTIME,

    PWA_TOGGLEBSETCHECK,
    PWA_TOGGLEBGETCHECK,

    PWA_LAST,
};

enum PICSimLabSystemCmd {
    PSC_FILEEXISTS = 0,
    PSC_GETUSERDATADIR,
    PSC_GETEXECUTABLEPATH,
    PSC_GETTEMPDIR,
    PSC_REMOVEDIR,
    PSC_CREATEDIR,
    PSC_REMOVEFILE,
    PSC_EXECUTE,
    PSC_ZIPDIR,
    PSC_UNZIPDIR,
    PSC_RENAMEFILE,
    PSC_LAUNCHDEFAULTBROWSER,
    PSC_LAUNCHDEFAULAPPLICATION,
    PSC_SHOWDIALOG,
    PSC_BASENAME,
    PSC_DIRNAME,

    PSC_MUTEXCREATE,
    PSC_MUTEXDESTROY,
    PSC_MUTEXLOCK,
    PSC_MUTEXUNLOCK,

    PSC_AUDIOCHCREATE,
    PSC_AUDIOCHDESTROY,
    PSC_AUDIOCHBEEPSTART,
    PSC_AUDIOCHBEEPSTARTF,
    PSC_AUDIOCHBEEPSTOP,
    PSC_AUDIOCHGETSAMPLERATE,
    PSC_AUDIOCHGETMAX,
    PSC_AUDIOCHSOUNDPLAY,

};

enum PICSimLabWindows {
    PW_MAIN = 0,
    PW_WPROP,
};

#define PFD_OPEN 0x0001
#define PFD_SAVE 0x0002
#define PFD_CHANGE_DIR 0x0080

#define RC_EXIT 1
#define RC_LOAD 2

#endif  // TYPES_H