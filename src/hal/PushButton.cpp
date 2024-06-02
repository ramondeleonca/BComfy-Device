#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <Arduino.h>

class PushButton {
    private:
        int pin;
        int lastState;
        int currentState;
        int lastDebounceTime;
        int debounceDelay;

    public:
        PushButton(int pin) {
            this->pin = pin;
            this->lastState = LOW;
            this->currentState = LOW;
            this->lastDebounceTime = 0;
            this->debounceDelay = 50;
            pinMode(pin, INPUT);
        };

        bool isPressed() {
            int reading = digitalRead(this->pin);
            if (reading != this->lastState) {
                this->lastDebounceTime = millis();
            }

            if ((millis() - this->lastDebounceTime) > this->debounceDelay) {
                if (reading != this->currentState) {
                    this->currentState = reading;
                    if (this->currentState == HIGH) {
                        return true;
                    }
                }
            }

            this->lastState = reading;
            return false;
        };

        void service() {
            this->isPressed();
        };
};

#endif