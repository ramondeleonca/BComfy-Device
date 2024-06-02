#ifndef HAL_POTENTIOMETER_H
#define HAL_POTENTIOMETER_H

#include <Arduino.h>

class Potentiometer {
    private:
        int pin;
        int value;
        int rawValue;
        int lastValue;
        int min;
        int max;

    public:
        Potentiometer(int pin, int min = 0, int max = 4096) {
            this->pin = pin;
            this->value = 0;
            this->rawValue = 0;
            this->lastValue = 0;
            this->min = min;
            this->max = max;
            pinMode(pin, INPUT);
        };

        int getValue() {
            this->rawValue = analogRead(this->pin);
            this->value = map(this->rawValue, 0, 4096, this->min, this->max);
            return this->value;
        };
};

#endif