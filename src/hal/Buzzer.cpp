#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H

#include <Arduino.h>

struct beep_t {
    int duration = 250;
    int frequency = 500;
    int delay = 0;
};

class Buzzer {
    private:
        int pin;
        int frequency;
        int duration;
        int startTime;
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

        // void beepSequence(beep_t* sequence[]) {
        //     for (int i = 0; i < sizeof(sequence) / sizeof(sequence[0]); i++) {
                
        //     }
        // }

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