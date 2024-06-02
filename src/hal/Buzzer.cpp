#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H

#include <Arduino.h>

class Buzzer {
    private:
        int pin;
        int frequency;
        int duration;
        unsigned long startTime;
        bool active = false;

    public:
        Buzzer(int pin) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
        }

        void beep(int frequency, int duration) {
            this->frequency = frequency;
            this->duration = duration;
            this->startTime = millis();
            this->active = true;
        }

        void service() {
            if (this->active) {
                if (millis() - this->startTime < this->duration) {
                    tone(this->pin, this->frequency);
                } else {
                    noTone(this->pin);
                    this->active = false;
                }
            }
        }
};

#endif