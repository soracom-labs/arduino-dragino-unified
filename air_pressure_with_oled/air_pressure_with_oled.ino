/*
 * air_pressure_with_oled.ino
 * Air pressure measurement
 *
 * Copyright SORACOM
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#include <Seeed_BMP280.h>
BMP280 bmp280;

void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Air Pres. Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("VALUE:");
  bmp280.init();
}

void loop(void) {
  float hPa = bmp280.getPressure() / 100.0;
  char buf1[10];
  dtostrf(hPa, 6, 1, buf1); /* format to "9999.9" */
  char buf2[12];
  sprintf(buf2, "%s hPa", buf1);
  u8x8.setCursor(4, 4);
  u8x8.print(buf2);
  delay(1000);
}
