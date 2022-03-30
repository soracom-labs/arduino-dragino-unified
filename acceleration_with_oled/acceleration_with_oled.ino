/*
 * acceleration_with_oled.ino
 * 3-axis acceleration value
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#include <LIS3DHTR.h>
LIS3DHTR<TwoWire> LIS;

void setup(void) {
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Accel. Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("X:");
  u8x8.setCursor(0, 4);
  u8x8.print("Y:");
  u8x8.setCursor(0, 6);
  u8x8.print("Z:");
  LIS.begin(Wire, 0x19);
  delay(100);
  LIS.setFullScaleRange(LIS3DHTR_RANGE_8G);
  LIS.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
}

void loop(void) {
  float x, y, z;
  LIS.getAcceleration(&x, &y, &z);
  char buf_x[10];
  dtostrf(x, 5, 2, buf_x); /* format to "9.99" */
  u8x8.setCursor(3, 2);
  u8x8.print(buf_x);

  char buf_y[10];
  dtostrf(y, 5, 2, buf_y); /* format to "9.99" */
  u8x8.setCursor(3, 4);
  u8x8.print(buf_y);

  char buf_z[10];
  dtostrf(z, 5, 2, buf_z); /* format to "9.99" */
  u8x8.setCursor(3, 6);
  u8x8.print(buf_z);
  delay(1000);
}
