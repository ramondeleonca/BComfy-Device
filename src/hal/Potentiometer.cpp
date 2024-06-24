#ifndef HAL_POTENTIOMETER_H
#define HAL_POTENTIOMETER_H

#include <Arduino.h>

class Potentiometer {
    private:
        int pin;
        int rawValue;
        int stepSize;
        int sampleStart = -2;
        int sampleTime;
        int lastStep;
        void (*onStep)(int);

    public:
        Potentiometer(int pin, int stepSize = 360, int sampleTime = 2000) {
            this->pin = pin;
            this->rawValue = 0;
            this->stepSize = stepSize;
            this->sampleTime = sampleTime;
            pinMode(pin, INPUT);
        };

        void service() {
            unsigned long currentTime = millis();
            int currentValue = analogRead(this->pin) & 0b1111111111000000;
            if (this->sampleStart == -2) {
                this->sampleStart = currentValue;
            } else if (this->sampleStart == -1) {
                this->sampleStart = currentValue;
            } else if (currentTime - this->sampleTime > this->sampleStart) {
                this->rawValue = (this->rawValue + currentValue) / 2;
                this->sampleStart = -1;
            }

            if (!this->lastStep) this->lastStep = this->rawValue;
            if (abs(this->rawValue - this->lastStep) > this->stepSize) {
                if (this->onStep) this->onStep(this->rawValue - this->lastStep);
                this->lastStep = this->rawValue;
            }
        }

        int getValue() {
            return this->rawValue;
        }

        void setOnStep(void (*onStep)(int)) {
            this->onStep = onStep;
        }
};

#endif