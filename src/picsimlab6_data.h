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

#ifndef CPWINDOW6D
#define CPWINDOW6D

static const char blink_code[] =
    "#include <Arduino.h>\n"
    "\n"
    "#define LED %s\n"
    "\n"
    "void setup()\n"
    "{\n"
    "  pinMode(LED, OUTPUT);\n"
    "}\n"
    "\n"
    "void loop()\n"
    "{\n"
    "  digitalWrite(LED, HIGH);\n"
    "  delay(500);\n"
    "  digitalWrite(LED, LOW);\n"
    "  delay(500);\n"
    "}\n"
    "";

static const char platformio_ini[] =
    "; PlatformIO Project Configuration File\n"
    ";\n"
    ";   Build options: build flags, source filter\n"
    ";   Upload options: custom upload port, speed and extra flags\n"
    ";   Library options: dependencies, extra library storages\n"
    ";   Advanced options: extra scripting\n"
    ";\n"
    "; Please visit documentation for the other options and examples\n"
    "; https://docs.platformio.org/page/projectconf.html\n"
    "\n"
    "[platformio]\n"
    "default_envs = PICSimLab-%s\n"
    "\n"
    "[env:uno]\n"
    "platform = atmelavr\n"
    "board = uno\n"
    "framework = arduino\n"
    "\n"
    "[PICSimLab]\n"
    "extends = env:uno\n"
    "upload_protocol = custom\n"
    "build_type = debug\n"
    "debug_tool = custom\n"
    "debug_port = localhost:1234\n"
    "debug_build_flags = -O2 -g\n"
    "debug_init_break = tbreak setup\n"
    "debug_init_cmds =\n"
    "    define pio_reset_halt_target\n"
    "    end\n"
    "    define pio_reset_run_target\n"
    "    end\n"
    "    target extended-remote $DEBUG_PORT\n"
    "    $LOAD_CMDS\n"
    "    pio_reset_halt_target\n"
    "    $INIT_BREAK\n"
    "\n"
    "[env:PICSimLab-Linux]\n"
    "extends = env:uno, PICSimLab\n"
    "upload_command = /usr/bin/picsimlab_tool loadhex \"$BUILD_DIR/firmware.hex\"\n"
    "\n"
    "[env:PICSimLab-Windows]\n"
    "extends = env:uno, PICSimLab\n"
    "upload_command = C:\\\"Program Files\"\\PicsimLab\\picsimlab_tool.exe loadhex \"$BUILD_DIR/firmware.hex\"\n"
    "\n"
    "";

#endif /*#CPWINDOW6D*/
