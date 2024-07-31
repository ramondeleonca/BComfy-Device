#include <Arduino.h>
#include <lib/SIM800L.h>
#include <data.h>

// SimSerial
const int SIM_RX = 16;
const int SIM_TX = 17;
HardwareSerial SIM800lSerial(2);
SIM800L sim800l(&SIM800lSerial);

void setup() {
    Serial.begin(115200);
    SIM800lSerial.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);
    sim800l.begin();

    Serial.println("Setup done");

    delay(1000);

    Serial.println("Sending SMS");

    bool res = sim800l.sendSms("+528446012963", "Hello from SIM800L");
    
    // Serial.println("Calling");

    // bool res = sim800l.call(TEST_NUMBER);
    // Serial.println(res);
}

void loop() {
    // put your main code here, to run repeatedly:
}
