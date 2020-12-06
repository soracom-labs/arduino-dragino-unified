/*
 * sound_buzzer.ino
 * Sound the buzzer
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
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
