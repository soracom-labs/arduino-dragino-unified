/*
 * send_uptime_with_soracom.ino
 * Uptime(Working time from bootup) send to Cloud via Unified Endpoint of SORACOM
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
*/

#define CONSOLE Serial
#define INTERVAL_MS (60000)
#define ENDPOINT "uni.soracom.io"
#define SKETCH_NAME "send_uptime_with_soracom"
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

// #define RESET_DURATION 86400000UL // 1 day
void software_reset() {
  asm volatile ("  jmp 0");
}

void setup() {
  CONSOLE.begin(115200);

  CONSOLE.println();
  CONSOLE.print(F("Welcome to ")); CONSOLE.print(SKETCH_NAME); CONSOLE.print(F(" ")); CONSOLE.println(VERSION);

  CONSOLE.print(F("resetting module "));
  pinMode(BG96_RESET,OUTPUT);
  digitalWrite(BG96_RESET,LOW);
  delay(300);
  digitalWrite(BG96_RESET,HIGH);
  delay(300);
  digitalWrite(BG96_RESET,LOW);
  CONSOLE.println(F(" done."));

  LTE_M_shieldUART.begin(BAUDRATE);

  CONSOLE.print(F("modem.restart()"));
  modem.restart();
  CONSOLE.println(F(" done."));

  CONSOLE.print(F("modem.getModemInfo(): "));
  String modemInfo = modem.getModemInfo();
  CONSOLE.println(modemInfo);

  CONSOLE.print(F("waitForNetwork()"));
  while (!modem.waitForNetwork()) CONSOLE.print(".");
  CONSOLE.println(F(" Ok."));

  CONSOLE.print(F("gprsConnect(soracom.io)"));
  modem.gprsConnect("soracom.io", "sora", "sora");
  CONSOLE.println(F(" done."));

  CONSOLE.print(F("isNetworkConnected()"));
  while (!modem.isNetworkConnected()) CONSOLE.print(".");
  CONSOLE.println(F(" Ok."));

  CONSOLE.print(F("My IP addr: "));
  IPAddress ipaddr = modem.localIP();
  CONSOLE.println(ipaddr);
}

void loop() {
  long uptime_sec = millis() / 1000;

  char payload[120];
  sprintf_P(payload, PSTR("{\"uptime\":%lu}"), uptime_sec);
  CONSOLE.println(payload);

  /* connect */
  if (!ctx.connect(ENDPOINT, 80)) {
    CONSOLE.println(F("failed."));
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
