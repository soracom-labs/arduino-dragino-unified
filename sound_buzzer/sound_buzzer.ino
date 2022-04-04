/*
 * sound_buzzer.ino
 * Sound the buzzer
 *
 * Copyright SORACOM
 * This software is released under the MIT License, and libraries used by these sketches 
 * are subject to their respective licenses.
 * See also: https://github.com/soracom-labs/arduino-dragino-unified/README.md
*/

#define buzzerPin 5
#define sound_interval_ms 25
void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  analogWrite(buzzerPin, 128);
  delay(sound_interval_ms);
  analogWrite(buzzerPin, 0);
  delay(1000 - sound_interval_ms);
}
