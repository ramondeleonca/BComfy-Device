#ifndef LIB_COMMANDS_H
#define LIB_COMMANDS_H

#include <Arduino.h>
#include <LinkedList.h>

struct Command {
    String name;
    void (*function)(Stream* serial, LinkedList<String> args);
};

class Commands {
    private:
        LinkedList<Command> commandList;
        Stream* serial;
        char sep;
        char term;
    
    public:
        Commands(Stream* serial = NULL, char sep = ' ', char term = '\n') {
            this->serial = serial;
            this->sep = sep;
            this->term = term;
        }

        void addCommand(String name, void (*function)(Stream* serial, LinkedList<String> args)) {
            Command command;
            command.name = name;
            command.function = function;
            Serial.println("Added command: " + name);
            commandList.add(command);
        }

        void readSerial() {
            String input = serial->readStringUntil(term);
            input.trim();

            int sepIndex = input.indexOf(sep);
            String command = input.substring(0, sepIndex);

            LinkedList<String> args;
            int nextSepIndex;
            while (sepIndex != -1) {
                if (input.charAt(sepIndex + 1) == '"') nextSepIndex = input.indexOf('"', sepIndex + 2);
                else nextSepIndex = input.indexOf(sep, sepIndex + 1);

                if (nextSepIndex == -1) {
                    args.add(input.substring(sepIndex + 1));
                } else {
                    args.add(input.substring(sepIndex + 1, nextSepIndex));
                }

                sepIndex = nextSepIndex;
            }

            Serial.println("Received: " + input);
            Serial.println("Command: " + command);
            for (int i = 0; i < args.size(); i++) {
                Serial.println("Arg " + String(i) + ": " + args.get(i));
            }
        }
};

#endif