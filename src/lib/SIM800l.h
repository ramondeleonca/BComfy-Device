#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>

// UNCOMMENT THIS LINE TO ENABLE DEBUGGING
#define DEBUG

class SIM800L {
    private:
        Stream *serial;
        Strnig last;

    public:
        SIM800L(Stream *serial) {
            this->serial = serial;

            // Check if serial begun, if not begin with 9600 baud, 8 stop bits and no parity
            if (false) {
                serial.begin(9600);
            }
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

        bool sendSms(String number, String message) {
            this->serial->println("AT+CMGF=1");
            this->serial->readStringUntil('\n');
            this->serial->println("AT+CMGS=\"" + number + "\"");
            this->serial->readStringUntil('\n');
            this->serial->print(message);
            this->serial->write(26);
            // return this->serial->readStringUntil('\n').indexOf("OK") != -1;
            return true;
        }

        String sendAtCommandWithReply(String command) {
            this->serial->println(command);
            return this->serial->readStringUntil('\n');
        }
};

#endif