#include "config.h"
#include "lcd.h"

void setup() {
    TRISCbits.TRISC7=1;
    lcd_init();
    lcd_cmd(L_CLR);
    lcd_cmd(L_NCR);
    lcd_cmd(L_L1);
    lcd_str(" Serial console");
    lcd_cmd(L_L2);
    lcd_str("open serial term");
    serial_tx_str("Type lines with terminator \\n (LF) \r\n");
}

char buffer[40];
void loop() {  
  serial_rx_str_until(buffer,40,'\n');
  serial_tx_str("Received: ");
  serial_tx_str(buffer);
  lcd_cmd(L_L1);
  lcd_str("                ");//clear line
  lcd_cmd(L_L1);
  lcd_str(buffer);
  
  serial_rx_str_until(buffer,40,'\n');  
  serial_tx_str("Received: ");
  serial_tx_str(buffer);
  lcd_cmd(L_L2);
  lcd_str("                ");//clear line
  lcd_cmd(L_L2);
  lcd_str(buffer);
}
