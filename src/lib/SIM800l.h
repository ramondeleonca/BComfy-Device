#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>

// UNCOMMENT THIS LINE TO ENABLE DEBUGGING
#define DEBUG

class SIM800l {
    private:
        Stream *serial;

    public:
        SIM800l(Stream *serial) {
            this->serial = serial;
        }

        bool begin() {
            return this->sendAtCommandWithReply("AT").indexOf("OK") != -1;
        }

        bool call(String number) {
            return this->sendAtCommandWithReply("ATD" + number + ";").indexOf("OK") != -1;
        }

        bool hangUp() {
            return this->sendAtCommandWithReply("ATH").indexOf("OK") != -1;
        }

        bool setVolume(int volume) {
            return this->sendAtCommandWithReply("AT+CLVL=" + String(volume)).indexOf("OK") != -1;
        }
        
        void sendAtCommandWithoutReply(String command) {
            this->serial->println(command);
        }

        String sendAtCommandWithReply(String command) {
            this->serial->println(command);
            return this->serial->readString();
        }
};

#endif