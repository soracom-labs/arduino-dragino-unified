/*
 * Connectivity diagnostics for LTE-M Shield for Arduino
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
 */

#define BG96_TX 11
#define BG96_RX 10
#define BG96_RST 15

#define BAUDRATE 9600

#define TINY_GSM_MODEM_BG96
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

#define CONSOLE Serial
SoftwareSerial SerialForModem(BG96_RX, BG96_TX);
TinyGsm MODEM(SerialForModem);

void resetBG96() {
  digitalWrite(BG96_RST, LOW);
  delay(300);
  digitalWrite(BG96_RST, HIGH);
  delay(300);
  digitalWrite(BG96_RST, LOW);
}

String executeAT(String command, long timeout) {
  String buf;
  
  MODEM.sendAT(command);
  if (MODEM.waitResponse(timeout, buf) != 1) {
    return "ERROR";
  }

  return formatResponse(buf);
}

String formatResponse(String str) {
  str.replace("\r\nOK\r\n", "[OK]");
  str.replace("\rOK\r", "[OK]");
  str.replace("\r\n", "\t");
  str.replace("\r", "\t");
  str.trim();
  return str;
}

bool isOk(String str) {
  return str.indexOf("[OK]") >= 0;
}

void showModemInformation() {
  String res;

  CONSOLE.println(F("> AT+GSN"));
  res = executeAT(F("+GSN"), 300);
  CONSOLE.println(res);
  
  CONSOLE.println(F("> AT+CIMI"));
  res = executeAT(F("+CIMI"), 300);
  CONSOLE.println(res);
  
  CONSOLE.println(F("> AT+QSIMSTAT?"));
  res = executeAT(F("+QSIMSTAT?"), 300);
  CONSOLE.println(res);
  
}

void showNetworkInformation() {
  String res;
  
  CONSOLE.println(F("> AT+QIACT?"));
  res = executeAT(F("+QIACT?"), 300);
  CONSOLE.println(res);

  CONSOLE.println(F("> AT+QCSQ"));
  res = executeAT(F("+QCSQ"), 300);
  CONSOLE.println(res);
  
  CONSOLE.println(F("> AT+COPS?"));
  res = executeAT(F("+COPS?"), 300);
  CONSOLE.println(res);
  
  CONSOLE.println(F("> AT+CGPADDR"));
  res = executeAT(F("+CGPADDR"), 300);
  CONSOLE.println(res);
}

bool setupNetworkConfigurations() {
  String res;
  
  CONSOLE.println(F("> AT+CGDCONT=1,\"IP\",\"soracom.io\",\"0.0.0.0\",0,0,0,0"));
  res = executeAT(F("+CGDCONT=1,\"IP\",\"soracom.io\",\"0.0.0.0\",0,0,0,0"), 300);
  CONSOLE.println(res);
  bool setupPDP = isOk(res);

  CONSOLE.println(F("> AT+QCFG=\"nwscanmode\",0,0"));
  res = executeAT(F("+QCFG=\"nwscanmode\",0,0"), 300);
  CONSOLE.println(res);
  bool networkScanMode = isOk(res);
  
  CONSOLE.println(F("> AT+QCFG=\"iotopmode\",0,0"));
  res = executeAT(F("+QCFG=\"iotopmode\",0,0"), 300);
  CONSOLE.println(res);
  bool networkCategory = isOk(res);

  CONSOLE.println(F("> AT+QCFG=\"nwscanseq\",00,1"));
  res = executeAT(F("+QCFG=\"nwscanseq\",00,1"), 300);
  CONSOLE.println(res);
  bool scanSequence = isOk(res);

  return (setupPDP && networkScanMode && networkCategory && scanSequence);
}

int printPingResult(String input) {
  input.replace("+QPING: ", "");

  char buf[100] = { 0 };
  input.toCharArray(buf, 100);
  if (strlen(buf) <= 0) return;
  
  int result = atoi(strtok(buf, ","));
  if (result == 0) {
    CONSOLE.print(F("Dest=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Bytes=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Time=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", TTL=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.println();
  }
  else {
    CONSOLE.print("(R)Error: "); CONSOLE.println(result);
  }

  return result;
}

int printPingSummary(String input) {
  input.replace("+QPING: ", "");

  char buf[100] = { 0 };
  input.toCharArray(buf, 100);
  if (strlen(buf) <= 0) return;
  
  int result = atoi(strtok(buf, ","));
  if (result == 0) {
    CONSOLE.print(F("Sent=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Received=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Lost=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Min=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Max=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.print(F(", Avg=")); CONSOLE.print(strtok(NULL, ","));
    CONSOLE.println();
  }
  else {
    CONSOLE.print("(S)Error: "); CONSOLE.println(result);
  }

  return result;
}

void pingToSoracomNetwork() {
  String res;
  res = executeAT(F("+QPING=1,\"pong.soracom.io\",3,3"), 3000);

  if (isOk(res)) {
    MODEM.stream.readStringUntil('\n');
    String try1 = MODEM.stream.readStringUntil('\n');
    MODEM.stream.readStringUntil('\n');
    String try2 = MODEM.stream.readStringUntil('\n');
    MODEM.stream.readStringUntil('\n');
    String try3 = MODEM.stream.readStringUntil('\n');
    MODEM.stream.readStringUntil('\n');
    String summary = MODEM.stream.readStringUntil('\n');
    MODEM.stream.readStringUntil('\n');

    printPingResult(try1);
    printPingResult(try2);
    printPingResult(try3);
    printPingSummary(summary);
  }
}

void setup() {
  CONSOLE.begin(BAUDRATE);
  SerialForModem.begin(BAUDRATE);

  CONSOLE.println();
  CONSOLE.println(F("****************************"));
  CONSOLE.println(F("* Connectivity diagnostics *"));
  CONSOLE.println(F("****************************"));

  CONSOLE.println();
  CONSOLE.print(F("--- Initializing modem, please wait for a while..."));
  pinMode(BG96_RST, OUTPUT);
  resetBG96();
  MODEM.restart();
  CONSOLE.println(F("[OK]"));

  CONSOLE.print(F("Target modem: "));
  String modemInfo = MODEM.getModemInfo();
  CONSOLE.println(modemInfo);

  CONSOLE.print(F("Testing AT Command: "));
  bool testATResult = MODEM.testAT();
  CONSOLE.println((testATResult) ? "[OK]" : "[FAILED]");
  if (!testATResult) {
    CONSOLE.println(F("Failed to execute test command, please RESET and retry later."));
    while(1);
  }

  CONSOLE.println();
  CONSOLE.println(F("--- Getting modem info..."));
  showModemInformation();

  CONSOLE.println();
  CONSOLE.println(F("--- Executing AT commands to connect SORACOM network..."));
  bool setupNetworkResult = setupNetworkConfigurations();
  if (!setupNetworkResult) {
    CONSOLE.println(F("Failed to execute setup commands, please RESET and retry later."));
    while(1);
  }

  CONSOLE.println();
  CONSOLE.print(F("--- Connecting to cellular network, please wait for a while..."));
  bool networkConnect = MODEM.gprsConnect("soracom.io", "sora", "sora");
  bool networkConnected = MODEM.waitForNetwork();
  CONSOLE.println((networkConnect && networkConnected) ? "[OK]" : "[FAILED]");
  if (!(networkConnect && networkConnected)) {
    CONSOLE.println(F("Failed to connect cellular network."));
    CONSOLE.println(F("Make sure active SIM has been inserted to the modem, and then check the antenna has been connected correctly."));
    CONSOLE.println(F("Please RESET and retry later."));
    while(1);
  }

  CONSOLE.println();
  CONSOLE.println(F("--- Getting network info..."));
  showNetworkInformation();

  CONSOLE.println();
  CONSOLE.println(F("--- Conntectivity test: Ping to pong.soracom.io..."));
  pingToSoracomNetwork();

  CONSOLE.println();
  CONSOLE.println(F("--- Execution completed, please write your own sketch and enjoy it."));
}

void loop() {
  // NOOP
}
