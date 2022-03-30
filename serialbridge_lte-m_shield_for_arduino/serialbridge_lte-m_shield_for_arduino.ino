/*
 * serialbridge_lte-m_shield_for_arduino.ino
 * Uptime(Working time from bootup) send to Cloud via Unified Endpoint of SORACOM
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/


/* BG96 */
#define CONSOLE Serial

/* for LTE-M Shield for Arduino */
#define RX 10
#define TX 11
#define BAUDRATE 9600
#include <SoftwareSerial.h>
SoftwareSerial MODEM(RX, TX);

void setup() {
  CONSOLE.begin(9600);
  CONSOLE.println();
  CONSOLE.println(F("Welcome to Serial bridge"));
  MODEM.begin(BAUDRATE);
  MODEM.write("ATE1"); // echo back ON
  MODEM.write(0x0d);   // and send CR
  CONSOLE.println(F(">> Waiting for your AT command"));
}
void loop() {
  while (MODEM.available()) CONSOLE.write(MODEM.read());
  while (CONSOLE.available()) MODEM.write(CONSOLE.read());
}
