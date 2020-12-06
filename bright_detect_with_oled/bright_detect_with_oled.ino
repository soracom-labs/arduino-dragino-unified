/*
 * bright_detect_with_oled.ino
 * Bright detector by light sensor
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#define lightSensorPin A6
void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Bright Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("VALUE:");
  pinMode(lightSensorPin, INPUT);
}

void loop(void) {
  int r = analogRead(lightSensorPin);
  char buf[10];
  sprintf(buf, "%04d", r);
  u8x8.setCursor(7, 2);
  u8x8.print(buf);
  u8x8.setCursor(0, 4);
  if (r > 200) {
    u8x8.print("Bright!");
  } else {
    u8x8.print("Dark...");
  }
}
