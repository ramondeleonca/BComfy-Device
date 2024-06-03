#ifndef HAL_PUSHBUTTON_H
#define HAL_PUSHBUTTON_H

#include <Arduino.h>

class PushButton {
    private:
        int pin;
        int lastState;
        int state;
        void (*onRising)(void) = NULL;
        void (*onFalling)(void) = NULL;

    public:
        PushButton(int pin) {
            this->pin = pin;
            pinMode(pin, INPUT_PULLDOWN);
        }

        void setOnRising(void (*onRising)(void)) {
            this->onRising = onRising;
        }

        void setOnFalling(void (*onFalling)(void)) {
            this->onFalling = onFalling;
        }

        void service() {
            this->lastState = this->state;
            this->state = digitalRead(this->pin);

            if (this->state != this->lastState) {
                if (this->state == HIGH) {
                    if (this->onRising != NULL) this->onRising();
                } else {
                    if (this->onFalling != NULL) this->onFalling();
                }
            }
        }
};

#endif