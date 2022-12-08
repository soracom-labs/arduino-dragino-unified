/*
 * send_multiple_sensor_data_with_soracom.ino
 * Multiple sensor value send to Cloud via Unified Endpoint of SORACOM
 *
 * Copyright SORACOM
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/

#define CONSOLE Serial
#define INTERVAL_MS (60000)
#define ENDPOINT "uni.soracom.io"
#define SKETCH_NAME "send_multiple_sensor_data_with_soracom"
#define VERSION "1.1"

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

#include <DHT.h>
#define USE_DHT11 // Use DHT11 (Blue)
// #define USE_DHT20 // Use DHT20 (Black)

#ifdef USE_DHT11
  #define dht11Pin 3
  DHT dht(dht11Pin, DHT11);
#endif
#ifdef USE_DHT20
  #include <Wire.h>
  DHT dht(DHT20);
#endif

#include <Seeed_BMP280.h>
BMP280 bmp280;

#define RESET_DURATION 86400000UL // 1 day
void software_reset() {
  asm volatile ("  jmp 0");
}

void setup() {
  CONSOLE.begin(115200);
  LTE_M_shieldUART.begin(BAUDRATE);

  CONSOLE.print(F("Welcome to ")); CONSOLE.print(F(SKETCH_NAME)); CONSOLE.print(F(" ")); CONSOLE.println(F(VERSION));
  delay(3000);

  CONSOLE.print(F("resetting module..."));
  pinMode(BG96_RESET,OUTPUT);
  digitalWrite(BG96_RESET,LOW);
  delay(300);
  digitalWrite(BG96_RESET,HIGH);
  delay(300);
  digitalWrite(BG96_RESET,LOW);
  CONSOLE.println(F(" done."));

  CONSOLE.print(F("modem.restart()..."));
  modem.restart();
  CONSOLE.println(F("done."));
  delay(500);

  CONSOLE.print(F("modem.getModemInfo()..."));
  String modemInfo = modem.getModemInfo();
  int modem_info_len = modemInfo.length() + 1;
  char modem_info_buf[modem_info_len];
  modemInfo.toCharArray(modem_info_buf, modem_info_len);
  CONSOLE.println(modem_info_buf);
  delay(2000);

  CONSOLE.print(F("waitForNetwork()..."));
  while (!modem.waitForNetwork());
  CONSOLE.println(F("Ok."));
  delay(500);

  CONSOLE.print(F("gprsConnect(soracom.io)..."));
  modem.gprsConnect("soracom.io", "sora", "sora");
  CONSOLE.println(F("Ok."));
  delay(500);

  CONSOLE.print(F("isNetworkConnected()..."));
  while (!modem.isNetworkConnected());
  CONSOLE.println(F("Ok."));
  delay(500);

  CONSOLE.print(F("My IP addr: "));
  IPAddress ipaddr = modem.localIP();
  char ip_addr_buf[20];
  sprintf_P(ip_addr_buf, PSTR("%d.%d.%d.%d"), ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
  CONSOLE.println(ip_addr_buf);
  CONSOLE.println();
  delay(2000);

  #ifdef USE_DHT20
  Wire.begin();
  #endif

  dht.begin();
  bmp280.init();
}

#define lightSensorPin A6
#define soundSensorPin A2
#define soundSamplingCount 3
void loop() {
  int light_raw_value = analogRead(lightSensorPin);
  int light_mapped_value = map(light_raw_value, 0, 1023, 0, 100);

  long sound_raw_value_sum = 0;
  for (int i = 0; i < soundSamplingCount; i++) {
    sound_raw_value_sum += analogRead(soundSensorPin);
  }
  int sound_mapped_value = map(sound_raw_value_sum / soundSamplingCount, 0, 1023, 0, 100);

  float temp = dht.readTemperature();
  char temp_buf[10];
  dtostrf(temp, 5, 1, temp_buf); /* format to "999.9" */
  int humi = (int) dht.readHumidity();

  float hPa = bmp280.getPressure() / 100.0;
  char hPa_buf[10];
  dtostrf(hPa, 6, 1, hPa_buf); /* format to "9999.9" */

  char line_buf[18];
  sprintf_P(line_buf, PSTR("lgt: %3d"), light_mapped_value);
  CONSOLE.println(line_buf);
  sprintf_P(line_buf, PSTR("snd: %3d"), sound_mapped_value);
  CONSOLE.println(line_buf);
  sprintf_P(line_buf, PSTR("tmp: %s"), temp_buf);
  CONSOLE.println(line_buf);
  sprintf_P(line_buf, PSTR("hmd: %3d"), humi);
  CONSOLE.println(line_buf);
  sprintf_P(line_buf, PSTR("hPa: %s"), hPa_buf);
  CONSOLE.println(line_buf);

  char payload[90];
  sprintf_P(payload, PSTR("{\"light\":%d,\"sound\":%d,\"temp_c\":%s,\"humi\":%d,\"air_pressure_hpa\":%s}"),
    light_mapped_value, sound_mapped_value, temp_buf, humi, hPa_buf);
  /* example:
   *  {"light":48,"sound":60,"temp_c": 25.0,"humi":34,"air_pressure_hpa":1015.5}
   */
  CONSOLE.println(payload);

  CONSOLE.print(F("Send..."));
  /* connect */
  if (!ctx.connect(ENDPOINT, 80)) {
    CONSOLE.println(F("failed."));
    CONSOLE.print(F("failed."));
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
  CONSOLE.println();

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
