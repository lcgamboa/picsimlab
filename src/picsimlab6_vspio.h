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

static const char blink_idf_code[] =
    "/*\n"
    "  Simple example to blinking an LED using Platformio IDE.\n"
    "   To upload to PICSimLab:\n"
    "     1- Check if PICSimLab is running;\n"
    "     2- Use Ctrl+Alt+U in VS code.\n"
    "   To debug the code:\n"
    "     1- Toggle the PICSimLab Debug button;\n"
    "     2- Use Ctrl+Shift+D followed by F5 in VS code.\n"
    "*/\n\n"
    "#include <stdio.h>\n"
    "#include \"freertos/FreeRTOS.h\"\n"
    "#include \"driver/gpio.h\"\n"
    "\n"
    "#define LED_PIN %s\n"
    "\n"
    "void app_main(void)\n"
    "{\n"
    "    gpio_reset_pin(LED_PIN);\n"
    "    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);\n"
    "\n"
    "    while (1)\n"
    "    {\n"
    "        gpio_set_level(LED_PIN, 1);\n"
    "        vTaskDelay(500 / portTICK_PERIOD_MS);\n"
    "        gpio_set_level(LED_PIN, 0);\n"
    "        vTaskDelay(500 / portTICK_PERIOD_MS);\n"
    "    }\n"
    "}\n"
    "\n";

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
    "framework = %s\n"
    "%s\n"
    "[env:PICSimLab]\n"
    "extends = env:%s\n"
    "extra_scripts = pre:python/install_python_deps.py\n"
    "test_framework = custom\n"
    "test_build_src = yes\n"
    "upload_protocol = custom\n"
    "upload_command = $PYTHONEXE python/picsimlab_tool.py \"$BUILD_DIR/firmware.%s\"\n"
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

static const char blink_test[] =
    "from platformio.public import TestCase, TestCaseSource, TestStatus, UnityTestRunner\n"
    "\n"
    "from PICSimLab_rcontrol import PICSimLab_rcontrol\n"
    "\n"
    "import time,re\n"
    "\n"
    "class CustomTestRunner(UnityTestRunner):\n"
    "    def stage_testing(self):\n"
    "        print(\"Blink time measure test started ....\")\n"
    "        time.sleep(3)\n"
    "        try:  \n"
    "           with PICSimLab_rcontrol(5000) as rc: \n"
    "             #rc.debug=1\n"
    "             rc.cmd_oscshow()\n"
    "             oscopen= rc.get_cmd_response().split()[0]\n"
    "             rc.cmd_oscshow(1)\n"
    "             rc.cmd_oscrdcfg()\n"
    "             osccfg= re.findall(r'(\\d+):\"([^\"]*)\"', rc.get_cmd_response())\n"
    "             rc.cmd_oscwrcfg(0, \"osc_cfg,100,100,0:500.000000,0.000000,1,1,2.500000,3,2,9,7,8\")\n"
    "             rc.cmd_oscwrcfg(1, \"osc_ch1,0,0,0:2.000000,0.000000,1,#FF0000,0,%s\")\n"
    "             rc.cmd_oscwrcfg(2, \"osc_ch2,0,0,0:2.000000,-6.000000,0,#00FF00,0,0\")\n"
    "             cnt = 0\n"
    "             while cnt < 5:\n"
    "                time.sleep(0.1)\n"
    "                cnt = cnt + 0.1\n"
    "                rc.cmd_oscmeasures(1)\n"
    "                val = rc.get_cmd_response()\n"
    "                fcyc = float((val.split('\\n')[9]).split()[1])\n"
    "                if fcyc > 900 :\n"
    "                   break\n"
    "             time.sleep(5)\n"
    "             rc.cmd_oscmeasures(1)\n"
    "             val= rc.get_cmd_response()\n"
    "             rc.cmd_oscwrcfg(osccfg[0][0], osccfg[0][1])\n"
    "             rc.cmd_oscwrcfg(osccfg[1][0], osccfg[1][1])\n"
    "             rc.cmd_oscwrcfg(osccfg[2][0], osccfg[2][1])\n"
    "             rc.cmd_oscshow(oscopen)\n"
    "             pcyc=float((val.split('\\n')[7]).split()[1]) \n"
    "             ncyc=float((val.split('\\n')[8]).split()[1]) \n"
    "\n"
    "             print(f\"Positive cycle: {pcyc} ms\")\n"
    "             print(f\"Negative cycle: {ncyc} ms\")\n"
    "\n"
    "             if  pcyc > 490 and pcyc < 510: \n"
    "                 self.test_suite.add_case(\n"
    "                     TestCase(name=\"Positive cycle\", status=TestStatus.PASSED))\n"
    "             else :   \n"
    "                 self.test_suite.add_case(\n"
    "                     TestCase(name=\"Positive cycle\", \n"
    "                              status=TestStatus.FAILED,\n"
    "                              message=f\"Expected 500 ms Was {pcyc} ms\",\n"
    "                              stdout=f\"Positive cycle: FAIL: Expected 500 ms Was {pcyc} ms\"\n"
    "                              ))\n"
    "                 \n"
    "             if  ncyc > 490 and ncyc < 510: \n"
    "                 self.test_suite.add_case(TestCase(name=\"Negative cycle\", status=TestStatus.PASSED))\n"
    "             else :   \n"
    "                 self.test_suite.add_case(\n"
    "                     TestCase(name=\"Negative cycle\", \n"
    "                              status=TestStatus.FAILED,\n"
    "                              message=f\"Expected 500 ms Was {ncyc} ms\",\n"
    "                              stdout=f\"Negative cycle: FAIL: Expected 500 ms Was {ncyc} ms\"\n"
    "                              ))     \n"
    "\n"
    "        except ConnectionError as e:\n"
    "            print(e)\n"
    "            return -1    \n"
    "           \n"
    "\n"
    "\n";

#endif /*#CPWINDOW6D*/
