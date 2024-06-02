#ifndef HAL_VIBRATIONMOTOR_H
#define HAL_VIBRATIONMOTOR_H

#include <Arduino.h>

class VibrationMotor {
    private:
        int pin;
        int started = -1;
        int duration = 0;

    public:
        VibrationMotor(int pin) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
        };

        void vibrate(int duration) {
            this->duration = duration;
            this->started = millis();
            digitalWrite(this->pin, HIGH);
        };

        void vibrate() {
            digitalWrite(this->pin, HIGH);
        };

        void stop() {
            this->duration = 0;
            this->started = -1;
            digitalWrite(this->pin, LOW);
        };

        void service() {
            if (this->started != -1 && millis() - this->started > this->duration) this->stop();
        }
};

#endif