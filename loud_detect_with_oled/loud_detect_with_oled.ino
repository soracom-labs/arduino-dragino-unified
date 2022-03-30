/*
 * loud_detect_with_oled.ino
 * Loud detector by sound sensor
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
 */

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#define soundSensorPin A2
#define sountSamplingCount 3
void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Sound Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("VALUE:");
}

void loop(void) {
  long sum = 0;
  for (int i = 0; i < sountSamplingCount; i++) {
    sum += analogRead(soundSensorPin);
  }
  int avg = sum / sountSamplingCount;
  char buf[10];
  sprintf(buf, "%04d", avg);
  u8x8.setCursor(7, 2);
  u8x8.print(buf);
  u8x8.setCursor(0, 4);
  if (avg > 350) {
    u8x8.print("Loud!    ");
    delay(2000);
  } else {
    u8x8.print("Silent...");
  }
}
