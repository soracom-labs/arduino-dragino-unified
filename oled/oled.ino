/*
 * oled.ino
 * Display text on OLED
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
 */

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("OLED Demo.");
  u8x8.setCursor(2, 2);
  u8x8.print("Hello World!!");

  u8x8.setFont(u8x8_font_profont29_2x3_n);
}

long v = 0;
void loop(void) {
  u8x8.setCursor(0, 4);
  u8x8.print(v);
  v++;
  delay(1000);
}
