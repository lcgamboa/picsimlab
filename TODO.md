### TODOs
| Filename | line # | TODO |
|:------|:------:|:------|
| [src/picsimlab4.cc](src/picsimlab4.cc#L413) | 413 | select the better mode for channel trigguer |
| [src/boards/board_Arduino_Mega.cc](src/boards/board_Arduino_Mega.cc#L68) | 68 | cboard_Arduino_Mega: add suport to analog inputs A6 and A7 |
| [src/boards/board_Arduino_Nano.cc](src/boards/board_Arduino_Nano.cc#L69) | 69 | cboard_Arduino_Nano: add suport to analog inputs A6 and A7 |
| [src/boards/board_McLab2.cc](src/boards/board_McLab2.cc#L98) | 98 | jumper support |
| [src/boards/board_PICGenios.cc](src/boards/board_PICGenios.cc#L193) | 193 | TEMP cooler must don't work with AQUE=0 |
| [src/boards/bsim_gpsim.cc](src/boards/bsim_gpsim.cc#L153) | 153 | add VCC and GND pins |
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1209) | 1209 | default output value is not used yet (DOV) |
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1577) | 1577 | avr ID pointer |
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1599) | 1599 | avr ID size |
| [src/devices/eth_w5500.cc](src/devices/eth_w5500.cc#L471) | 471 | add support to buffer size different of 2k |
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L123) | 123 | ssd1306 Scrolling Command Table |
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L134) | 134 | ssd1306 Continuous Vertical and Horizontal Scroll Setup |
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L150) | 150 | ssd1306 Set Vertical Scroll |
| [src/devices/lcd_ssd1306.cc](src/devices/lcd_ssd1306.cc#L250) | 250 | ssd1306 Set Display Offset |
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L72) | 72 | decode |
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L75) | 75 | itensity |
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L78) | 78 | scan limit |
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L81) | 81 | shutdown |
| [src/devices/ldd_max72xx.cc](src/devices/ldd_max72xx.cc#L88) | 88 | display test |
| [src/devices/rtc_ds1307.cc](src/devices/rtc_ds1307.cc#L300) | 300 | int output |
| [src/devices/rtc_pfc8563.cc](src/devices/rtc_pfc8563.cc#L282) | 282 | int output and countdown timer |
| [src/parts/input_ds1621.cc](src/parts/input_ds1621.cc#L200) | 200 | set addr |
| [src/parts/input_ds1621.cc](src/parts/input_ds1621.cc#L212) | 212 | implement Tout output |
| [src/parts/other_IO_MCP23S17.cc](src/parts/other_IO_MCP23S17.cc#L379) | 379 | only write support implemented |
| [src/parts/output_dcmotor.cc](src/parts/output_dcmotor.cc#L160) | 160 | Add transfer funcion of dc motor |

### FIXMEs
| Filename | line # | FIXME |
|:------|:------:|:------|
| [src/boards/board_uCboard.cc](src/boards/board_uCboard.cc#L302) | 302 | NSTEP must be multiplied for 4 |
| [src/boards/bsim_simavr.cc](src/boards/bsim_simavr.cc#L1594) | 1594 | avr CONFIG size |
| [src/devices/io_MCP23X17.cc](src/devices/io_MCP23X17.cc#L134) | 134 | only for BANK=0; |
| [src/lib/picsimlab.cc](src/lib/picsimlab.cc#L1151) | 1151 | remote control disabled |
| [src/parts/input_encoder.cc](src/parts/input_encoder.cc#L163) | 163 | on slow speed output is not 90 degrees |
| [src/parts/output_dcmotor.cc](src/parts/output_dcmotor.cc#L213) | 213 | on slow speed output is not 90 degrees |
