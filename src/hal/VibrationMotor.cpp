#ifndef HAL_VIBRATIONMOTOR_H
#define HAL_VIBRATIONMOTOR_H

#include <Arduino.h>

class VibrationMotor {
    private:
        int pin;

    public:
        VibrationMotor(int pin) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
        };

        void vibrate(int duration) {
            digitalWrite(this->pin, HIGH);
            delay(duration);
            digitalWrite(this->pin, LOW);
        };

        void vibrate() {
            digitalWrite(this->pin, HIGH);
        };

        void stop() {
            digitalWrite(this->pin, LOW);
        };
};

#endif