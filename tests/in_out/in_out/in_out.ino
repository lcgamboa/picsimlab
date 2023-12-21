
#if defined(ARDUINO_ARCH_AVR)
#define ADCMAX 1023
#define ANALOGIN A0
#define LED1  3
#define PBUTTON 2
#define LED2  4
#else
#endif

#if defined(ARDUINO_ARCH_STM32)
#define ADCMAX 1023
#define ANALOGIN PA0
#define LED1  PB1
#define PBUTTON PB3
#define LED2  PB4
#endif

#if defined(ARDUINO_ARCH_ESP32)
#if defined (CONFIG_IDF_TARGET_ESP32C3) 
#define ADCMAX 4095
#define ANALOGIN 0
#define LED1  1
#define PBUTTON 3
#define LED2  4
#else
#define ADCMAX 4095
#define ANALOGIN 4
#define LED1  3
#define PBUTTON 2
#define LED2  5
#endif
#endif


static int bstate = 0;

void isr(void) {
  bstate = digitalRead(PBUTTON);
}

void setup() {
  Serial.begin(115200);
  pinMode(ANALOGIN, INPUT);
  pinMode(PBUTTON , INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  attachInterrupt(digitalPinToInterrupt(PBUTTON), isr, CHANGE);

}

unsigned int value;
unsigned char value2;
char buffer[50];

void loop() {
  value = analogRead(ANALOGIN);
  value2 = map(value, 0 , ADCMAX, 0, 255 );
  analogWrite(LED1, value2);

  digitalWrite(LED2, bstate);
  sprintf(buffer, "AD=%04d PWM=%03d BT=%d", value, value2, bstate);
  Serial.println(buffer);
  delay(250);
}
