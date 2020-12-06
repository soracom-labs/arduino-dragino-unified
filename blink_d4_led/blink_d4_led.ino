/*
 * blink_d4_led.ino
 * Blink the LED on D4
 *
 * Copyright (c) 2020 SORACOM, INC.
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
*/

#define ledPin 4
void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}
