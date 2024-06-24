#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>

// UNCOMMENT THIS LINE TO ENABLE DEBUGGING
#define DEBUG

class SIM800L {
    private:
        Stream *serial;

    public:
        SIM800L(Stream *serial) {
            this->serial = serial;
        }

        void begin() {
            this->serial->println("AT");
            #ifdef DEBUG
            Serial.println("SIM800L initialized");
            #endif
        }

        void service() {
            String input = this->serial->readString();

            #ifdef DEBUG
            if (this->serial->available()) Serial.println(input);
            #endif

            if (input.indexOf("ERROR") > 0) this->begin();
        }
};

#endif