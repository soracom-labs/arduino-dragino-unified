/*
 * send_temp_and_humi_with_soracom.ino
 * Ambient temperature and humidity send to Cloud via Unified Endpoint of SORACOM
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/

#define CONSOLE Serial
#define INTERVAL_MS (60000)
#define ENDPOINT "uni.soracom.io"
#define SKETCH_NAME "send_temp_and_humi_with_soracom"
#define VERSION "1.0"

/* for LTE-M Shield for Arduino */
#define RX 10
#define TX 11
#define BAUDRATE 9600
#define BG96_RESET 15

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

#define RESET_DURATION 86400000UL // 1 day
void software_reset() {
  asm volatile ("  jmp 0");
}

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

  CONSOLE.print(F("resetting module "));
  pinMode(BG96_RESET,OUTPUT);
  digitalWrite(BG96_RESET,LOW);
  delay(300);
  digitalWrite(BG96_RESET,HIGH);
  delay(300);
  digitalWrite(BG96_RESET,LOW);
  CONSOLE.println(F(" done."));

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
}

void loop() {
  float t = dht.readTemperature();
  char t_buf1[10];
  dtostrf(t, 5, 1, t_buf1); /* format to "999.9" */
  int h = (int) dht.readHumidity();

  char payload[40];
  sprintf_P(payload, PSTR("{\"temp_c\":%s,\"humi\":%d}"), t_buf1, h);
  /* example:
   *  {"temp_c": 26.0,"humi":35}
   */
  CONSOLE.println(payload);
  u8x8.clear();
  drawText(&u8x8, payload);
  u8x8.println();

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
  while (ctx.connected()) {
    String line = ctx.readStringUntil('\n');
    CONSOLE.println(line);
    if (line == "\r") {
      CONSOLE.println(F("Response header received."));
      break;
    }
  }
  // NOTE: response body is ignore
  ctx.stop();
  CONSOLE.println(F("done."));
  drawText_P(&u8x8, PSTR("done."));

  delay(INTERVAL_MS);
  
#ifdef RESET_DURATION
  if(millis() > RESET_DURATION )
  {
    CONSOLE.println("Execute software reset...");
    delay(1000);
    software_reset();
  }
#endif
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
