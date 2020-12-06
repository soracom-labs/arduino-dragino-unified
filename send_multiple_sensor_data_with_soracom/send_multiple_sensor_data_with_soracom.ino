/*
 * send_multiple_sensor_data_with_soracom.ino
 * Multiple sensor value send to Cloud via Unified Endpoint of SORACOM
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
*/

#define CONSOLE Serial
#define INTERVAL_MS (60000)
#define ENDPOINT "uni.soracom.io"
#define SKETCH_NAME "send_multiple_sensor_data_with_soracom"
#define VERSION "1.0"

/* for LTE-M Shield for Arduino */
#define RX 10
#define TX 11
#define BAUDRATE 9600

#define TINY_GSM_MODEM_BG96
#include <TinyGsmClient.h>

#include <SoftwareSerial.h>
SoftwareSerial LTE_M_shieldUART(RX, TX);
TinyGsm modem(LTE_M_shieldUART);
TinyGsmClient ctx(modem);

#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1

#include <DHT.h>
#define dht11Pin 3
DHT dht(dht11Pin, DHT11);

#define OLED_MAX_CHAR_LENGTH 16
void drawText(U8X8* u8x8, const char* in_str, int width = OLED_MAX_CHAR_LENGTH);
void drawText_P(U8X8* u8x8, const char* pgm_s, int width = OLED_MAX_CHAR_LENGTH);

#include <Seeed_BMP280.h>
BMP280 bmp280;

void setup() {
  CONSOLE.begin(115200);
  LTE_M_shieldUART.begin(BAUDRATE);
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_victoriamedium8_r);

  CONSOLE.print(F("Welcome to ")); CONSOLE.print(SKETCH_NAME); CONSOLE.print(F(" ")); CONSOLE.println(VERSION);
  u8x8.clear();
  drawText_P(&u8x8, PSTR("Welcome to ")); drawText(&u8x8, SKETCH_NAME); drawText_P(&u8x8, PSTR(" ")); drawText(&u8x8, VERSION);
  delay(3000);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("modem.restart()..."));
  modem.restart();
  drawText_P(&u8x8, PSTR("done."));
  delay(500);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("modem.getModemInfo():"));
  String modemInfo = modem.getModemInfo();
  u8x8.println();
  int modem_info_len = modemInfo.length() + 1;
  char modem_info_buf[modem_info_len];
  modemInfo.toCharArray(modem_info_buf, modem_info_len);
  drawText(&u8x8, modem_info_buf);
  delay(2000);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("waitForNetwork()..."));
  while (!modem.waitForNetwork());
  drawText_P(&u8x8, PSTR("Ok."));
  delay(500);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("gprsConnect(soracom.io)..."));
  modem.gprsConnect("soracom.io", "sora", "sora");
  drawText_P(&u8x8, PSTR("done."));
  delay(500);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("isNetworkConnected()..."));
  while (!modem.isNetworkConnected());
  drawText_P(&u8x8, PSTR("Ok."));
  delay(500);

  u8x8.clear();
  drawText_P(&u8x8, PSTR("My IP addr: "));
  u8x8.println();
  IPAddress ipaddr = modem.localIP();
  CONSOLE.println(ipaddr);
  char ip_addr_buf[20];
  sprintf_P(ip_addr_buf, PSTR("%d.%d.%d.%d"), ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
  drawText(&u8x8, ip_addr_buf);
  delay(2000);

  dht.begin();
  bmp280.init();
}

#define lightSensorPin A6
#define soundSensorPin A2
#define sountSamplingCount 3
void loop() {
  int light_raw_value = analogRead(lightSensorPin);
  int light_mapped_value = map(light_raw_value, 0, 1023, 0, 100);

  long sound_raw_value_sum = 0;
  for (int i = 0; i < sountSamplingCount; i++) {
    sound_raw_value_sum += analogRead(soundSensorPin);
  }
  int sound_mapped_value = map(sound_raw_value_sum / sountSamplingCount, 0, 1023, 0, 100);

  float temp = dht.readTemperature();
  char temp_buf[10];
  dtostrf(temp, 5, 1, temp_buf); /* format to "999.9" */
  int humi = (int) dht.readHumidity();

  float hPa = bmp280.getPressure() / 100.0;
  char hPa_buf[10];
  dtostrf(hPa, 6, 1, hPa_buf); /* format to "9999.9" */

  u8x8.clear();
  u8x8.setInverseFont(1);
  u8x8.drawString(0, 0, " Ambient Monitor");
  u8x8.println();
  u8x8.setInverseFont(0);
  char line_buf[18];
  sprintf_P(line_buf, PSTR("lgt: %3d"), light_mapped_value);
  drawText(&u8x8, line_buf);
  u8x8.println();
  sprintf_P(line_buf, PSTR("snd: %3d"), sound_mapped_value);
  drawText(&u8x8, line_buf);
  u8x8.println();
  sprintf_P(line_buf, PSTR("tmp: %s"), temp_buf);
  drawText(&u8x8, line_buf);
  u8x8.println();
  sprintf_P(line_buf, PSTR("hmd: %3d"), humi);
  drawText(&u8x8, line_buf);
  u8x8.println();
  sprintf_P(line_buf, PSTR("hPa:%s"), hPa_buf);
  drawText(&u8x8, line_buf);
  u8x8.println();

  char payload[90];
  sprintf_P(payload, PSTR("{\"light\":%d,\"sound\":%d,\"temp_c\":%s,\"humi\":%d,\"air_pressure_hpa\":%s}"),
    light_mapped_value, sound_mapped_value, temp_buf, humi, hPa_buf);
  /* example:
   *  {"light":48,"sound":60,"temp_c": 25.0,"humi":34,"air_pressure_hpa":1015.5}
   */
  CONSOLE.println(payload);

  CONSOLE.print(F("Send..."));
  drawText_P(&u8x8, PSTR("Send..."));
  /* connect */
  if (!ctx.connect(ENDPOINT, 80)) {
    CONSOLE.println(F("failed."));
    drawText_P(&u8x8, PSTR("failed."));
    delay(3000);
    return;
  }
  /* send request */
  char hdr_buf[40];
  ctx.println(F("POST / HTTP/1.1"));
  sprintf_P(hdr_buf, PSTR("Host: %s"), ENDPOINT);
  ctx.println(hdr_buf);
  ctx.println(F("Content-Type: application/json"));
  sprintf_P(hdr_buf, PSTR("Content-Length: %d"), strlen(payload));
  ctx.println(hdr_buf);
  ctx.println();
  ctx.println(payload);
  /* receive response */
  // NOTE: response header and body are ignore due to saving for memory
  ctx.stop();
  CONSOLE.println(F("done."));
  u8x8.clearLine(6);

  delay(INTERVAL_MS);
}

void drawText(U8X8* u8x8, const char* in_str, int width = OLED_MAX_CHAR_LENGTH) {
  size_t len = strlen(in_str);
  for (int i = 0 ; i < len ; i++) {
    if (u8x8->tx > width - 1) {
      u8x8->tx = 0; // CR
      u8x8->ty++;   // LF
    }
    u8x8->print(in_str[i]);
  }
}

void drawText_P(U8X8* u8x8, const char* pgm_s, int width = OLED_MAX_CHAR_LENGTH) {
  size_t len = strlen_P(pgm_s);
  for (int i = 0 ; i < len ; i++) {
    if (u8x8->tx > width - 1) {
      u8x8->tx = 0; // CR
      u8x8->ty++;   // LF
    }
    char c = pgm_read_byte(pgm_s++);
    u8x8->print(c);
  }
}
