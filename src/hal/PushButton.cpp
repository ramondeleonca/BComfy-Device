#ifndef HAL_PUSHBUTTON_H
#define HAL_PUSHBUTTON_H

#include <Arduino.h>

class PushButton {
    private:
        int pin;
        int lastState;
        int state;
        int trigger;
        void (*callback)(void) = NULL;

    public:
        PushButton(int pin, int trigger = RISING) {
            this->pin = pin;
            this->trigger = trigger;
            pinMode(pin, INPUT);
        }

        void setCallback(void (*callback)(void)) {
            this->callback = callback;
        }

        void service() {
            this->lastState = this->state;
            this->state = digitalRead(this->pin);
            if (this->trigger == RISING && this->lastState == LOW && this->state == HIGH) {
                if (this->callback != NULL) this->callback();
            } else if (this->trigger == FALLING && this->lastState == HIGH && this->state == LOW) {
                if (this->callback != NULL) this->callback();
            }
        }
};

#endif