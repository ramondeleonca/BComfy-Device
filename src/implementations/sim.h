#include <Arduino.h>
#include <HardwareSerial.h>
#include <data.h>

HardwareSerial SIM800l(2);

void setup() {
    // Inicialización del puerto serial para la comunicación con el monitor serial
    Serial.begin(115200);
    SIM800l.begin(9600, SERIAL_8N1, 16, 17); // Baudrate, parity, RX, TX
    Serial.println("ESP32 con SIM800L listo. Ingresa comandos AT:");
    delay(1000);
}

void loop() {
    if(SIM800l.available()) Serial.print(SIM800l.read());
    if(Serial.available()) SIM800l.write(Serial.read());
}