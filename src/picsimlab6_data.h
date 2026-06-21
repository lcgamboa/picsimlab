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
    "/*\n"
    "  Simple example to blinking an LED using Platformio IDE.\n"
    "   To upload to PICSimLab:\n"
    "     1- Check if PICSimLab is running;\n"
    "     2- Use Ctrl+Alt+U in VS code.\n"
    "   To debug the code:\n"
    "     1- Toggle the PICSimLab Debug button;\n"
    "     2- Use Ctrl+Shift+D followed by F5 in VS code.\n"
    "*/\n\n"
    "#include <Arduino.h>\n"
    "\n"
    "#define LED_PIN %s\n"
    "\n"
    "void setup()\n"
    "{\n"
    "  pinMode(LED_PIN, OUTPUT);\n"
    "}\n"
    "\n"
    "void loop()\n"
    "{\n"
    "  digitalWrite(LED_PIN, HIGH);\n"
    "  delay(500);\n"
    "  digitalWrite(LED_PIN, LOW);\n"
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
    "default_envs = PICSimLab\n"
    "\n"
    "[env:%s]\n"
    "platform = %s\n"
    "board = %s\n"
    "framework = arduino\n"
    "%s\n"
    "[env:PICSimLab]\n"
    "extends = env:%s\n"
    "extra_scripts = pre:install_python_deps.py\n"
    "upload_protocol = custom\n"
    "upload_command = $PYTHONEXE picsimlab_tool.py \"$BUILD_DIR/firmware.%s\"\n"
    "build_type = debug\n"
    "debug_tool = custom\n"
    "debug_port = localhost:1234\n"
    "debug_build_flags = -O2 -g\n"
    "debug_init_break = tbreak setup\n"
    "debug_init_cmds =\n"
    "    define pio_reset_halt_target\n%s"
    "    end\n"
    "    define pio_reset_run_target\n%s"
    "    end\n"
    "    target extended-remote $DEBUG_PORT\n"
    "    $LOAD_CMDS\n"
    "    pio_reset_halt_target\n"
    "    $INIT_BREAK\n"
    "\n";

#endif /*#CPWINDOW6D*/
