#ifndef LIB_COMMANDS_H
#define LIB_COMMANDS_H

#include <Arduino.h>
#include <LinkedList.h>

// #define DEBUG

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
            #ifdef DEBUG
            Serial.println("Added command: " + name);
            #endif
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
                // TODO: Ignore quotes preceeded by a backslash
                if (input.charAt(sepIndex + 1) == '"') {
                    nextSepIndex = input.indexOf('"', sepIndex + 2);
                    nextSepIndex = input.indexOf(sep, nextSepIndex + 1);
                } else nextSepIndex = input.indexOf(sep, sepIndex + 1);

                if (nextSepIndex == -1) {
                    args.add(input.substring(sepIndex + 1));
                } else {
                    args.add(input.substring(sepIndex + 1, nextSepIndex));
                }

                sepIndex = nextSepIndex;
            }

            #ifdef DEBUG
            Serial.println("Received: " + input);
            Serial.println("Command: " + command);
            for (int i = 0; i < args.size(); i++) Serial.println("Arg " + String(i) + ": " + args.get(i));
            #endif

            for (int i = 0; i < commandList.size(); i++) {
                if (commandList.get(i).name == command) {
                    commandList.get(i).function(serial, args);
                    return;
                }
            }
        }
};

#endif