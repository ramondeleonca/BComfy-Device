#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("BComfy.09");
}

void loop() {
  if (Serial.available()) SerialBT.write(Serial.read());
  if (SerialBT.available()) Serial.write(SerialBT.read());
}