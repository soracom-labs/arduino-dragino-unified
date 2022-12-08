/*
 * temp_and_humi_with_oled.ino
 * Ambient temperature and humidity
 *
 * Copyright SORACOM
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
 */

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#include <DHT.h>

#define USE_DHT11 // Use DHT11 (Blue)
//#define USE_DHT20 // Use DHT20 (Black)

#ifdef USE_DHT11
  #define dht11Pin 3
  DHT dht(dht11Pin, DHT11);
#endif
#ifdef USE_DHT20
  DHT dht(DHT20);
#endif

void setup(void) {
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Temp&Humi Demo.");
  u8x8.setCursor(0, 2);
  u8x8.print("TEMP:");
  u8x8.setCursor(0, 4);
  u8x8.print("HUMI:");
  dht.begin();
}

void loop(void) {
  float t = dht.readTemperature();
  char t_buf1[10];
  dtostrf(t, 5, 1, t_buf1); /* format to "999.9" */
  char t_buf2[10];
  sprintf(t_buf2, "%s C", t_buf1);
  u8x8.setCursor(6, 2);
  u8x8.print(t_buf2);

  int h = (int) dht.readHumidity();
  char h_buf[10];
  sprintf(h_buf, "%3d %%", h);
  u8x8.setCursor(6, 4);
  u8x8.print(h_buf);
  delay(1000);
}
