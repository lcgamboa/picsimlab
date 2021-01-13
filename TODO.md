### TODOs
| Filename | line # | TODO
|:------|:------:|:------
| [src/picsimlab1.cc](src/picsimlab1.cc#L359) | 359 | verify initialization errors
| [src/picsimlab4.cc](src/picsimlab4.cc#L586) | 586 | select the better mode for channel trigguer
| [src/boards/board_McLab2.cc](src/boards/board_McLab2.cc#L103) | 103 | jumper support
| [src/boards/board_PICGenios.cc](src/boards/board_PICGenios.cc#L62) | 62 | TEMP cooler must don't work with AQUE=0
| [src/boards/bsim_gpsim.cc](src/boards/bsim_gpsim.cc#L170) | 170 | add VCC and GND pins
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L866) | 866 | default output value is not used yet (DOV)
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1156) | 1156 | avr ID pointer
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1163) | 1163 | avr EEPROM pointer
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1189) | 1189 | avr ID size
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1196) | 1196 | AVR EEPROM size
| [src/devices/eth_w5500.cc](src/devices/eth_w5500.cc#L366) | 366 | add support to buffer size different of 2k
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L141) | 141 | ssd1306 Scrolling Command Table
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L155) | 155 | ssd1306 Continuous Vertical and Horizontal Scroll Setup
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L174) | 174 | ssd1306 Set Vertical Scroll
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L282) | 282 | ssd1306 Set Display Offset
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L84) | 84 | decode
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L87) | 87 | itensity
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L90) | 90 | scan limit
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L93) | 93 | shutdown
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L100) | 100 | display test
| [src/devices/rtc_ds1307.cc](src/devices/rtc_ds1307.cc#L408) | 408 | int output
| [src/devices/rtc_pfc8563.cc](src/devices/rtc_pfc8563.cc#L408) | 408 | int output and countdown timer
| [src/parts/part_IO_MCP23S17.cc](src/parts/part_IO_MCP23S17.cc#L487) | 487 | only write support implemented

### FIXMEs
| Filename | line # | FIXME
|:------|:------:|:------
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1182) | 1182 | avr CONFIG size
| [src/devices/io_MCP23X17.cc](src/devices/io_MCP23X17.cc#L118) | 118 | only for BANK=0;
