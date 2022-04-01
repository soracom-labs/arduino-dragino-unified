/*
 * rotaly_value_with_oled.ino
 * Display rotaly value
 *
 * Copyright SORACOM
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
 */

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#define rotaryPin A0
void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Rotaly Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("VALUE:");
  pinMode(rotaryPin, INPUT);
}

void loop(void) {
  int r = analogRead(rotaryPin);
  int v = map(r, 0, 1023, 100, 0);
  char buf[10];
  sprintf(buf, "%03d", v);
  u8x8.setCursor(7, 2);
  u8x8.print(buf);
}
