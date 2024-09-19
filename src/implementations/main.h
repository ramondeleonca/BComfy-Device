#ifndef MAIN_CPP
#define MAIN_CPP

#include <Arduino.h>
#include <WiFi.h>
#include <OpenAI.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Preferences.h>
#include <BluetoothSerial.h>
#include <ESP32Time.h>
#include <WS2812FX.h>
#include "aikey.h"
#include "lib/SIM800L.h"
#include "hal/PushButton.cpp"
#include "hal/VibrationMotor.cpp"
#include "hal/Potentiometer.cpp"
#include "hal/Buzzer.cpp"
#include "lib/Commands.h"
#include "comfyui/icons.h"
#include "lib/Utils.h"
#include "lib/BComfySettings.h"
#include "data.h"
#include "mensajes.h"
#include <TinyGPSPlus.h>
#include <SPIFFS.h>
#include <LinkedList.h>

// Device config
const String PRODUCT_NAME = "BComfy";
const String PRODUCT_VERSION = "0.0.1";
const String SOFTWARE_VERSION = "0.0.1";
const String DEVICE_ID = WiFi.macAddress().substring(0, 2);
const String DEVICE_NAME = PRODUCT_NAME + "." + DEVICE_ID;

// AI Config
// * PROVISION API KEY IN aikey.h
const String OPENAI_API_KEY = AI_KEY;
const String OPENAI_MODEL = "gpt-3.5-turbo-0125";
const String OPENAI_CHAT_SYSTEM = "Your job is to generate motivational messages for a person with anxiety, these messages must not exceed 100 tokens, return only the message";
const int OPENAI_CHAT_TOKENS = 200;
const float OPENAI_CHAT_TEMPERATURE = 0.7;
const float OPENAI_CHAT_PRESENCE_PENALTY = 0.5;
const float OPENAI_CHAT_FREQUENCY_PENALTY = 0.5;
OpenAI openai(OPENAI_API_KEY.c_str());
OpenAI_ChatCompletion chat(openai);

// Displays config
// * BIG DISPLAY
const int BIG_DISPLAY_WIDTH = 128;
const int BIG_DISPLAY_HEIGHT = 64;
const int BIG_DISPLAY_ADDRESS = 0x3C;
Adafruit_SSD1306 bigDisplay(BIG_DISPLAY_WIDTH, BIG_DISPLAY_HEIGHT, &Wire, -1);

// * SMALL DISPLAY
const int SMALL_DISPLAY_WIDTH = 128;
const int SMALL_DISPLAY_HEIGHT = 32;
const int SMALL_DISPLAY_ADDRESS = 0x3C;
const int SMALL_DISPLAY_SDA = 18;
const int SMALL_DISPLAY_SCL = 19;
TwoWire smallDisplayWire(1);
Adafruit_SSD1306 smallDisplay(SMALL_DISPLAY_WIDTH, SMALL_DISPLAY_HEIGHT, &smallDisplayWire, -1);

// LED Strip
// const int LED_PIN = 2;
const int LED_PIN = 15;
const int LED_COUNT = 16; 
const int LED_BRIGHTNESS = 25;
WS2812FX leds = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Contexts
const String screens[] = {"Llamada", "Mensajes", "Juego", "Configuracion"};
const int screensSize = sizeof(screens) / sizeof(screens[0]);
int currentScreen = 0;

// Settings

// Create an esp32time object
ESP32Time rtc;

// Preferences
Preferences preferences;

// Bluetooth Serial
BluetoothSerial SerialBT;

// Commands
Commands serialCommands(&Serial);
Commands bluetoothCommands(&SerialBT);

// Buttons
PushButton backButton(13);
PushButton upButton(12);
PushButton dnButton(14);
PushButton okButton(27);

// Potentiometer
// Potentiometer potentiometer(15);

// Vibration motor
VibrationMotor vibrationMotor(4);

// Buzzer
Buzzer buzzer(5);

// SimSerial
const int SIM_RX = 4;
const int SIM_TX = 2;
HardwareSerial SIM800lSerial(1);

// GPS
const int GPS_RX = 16;
const int GPS_TX = 17;
HardwareSerial neogps(2);
TinyGPSPlus gps;

// Variables loaded in from preferences
String emergencyNumber;

// State
// TODO: Move to a struct
// Not pressed: -1
int buttonStart = -1;
int buttonDuration = 1000;
bool calling = false;
bool callingRequest = false;

// Leds
int ledEffectDuration;
int ledStart;

// ! GAME
int GAME_CACTUS_EVERY = 3000;
int GAME_SCORE = 0;
int GAME_SPEED = 1;
int GAME_JUMP_HEIGHT = cacatus_height + 3;
int gameStart = -1;
bool gameLost = false;
bool gameIsJumping = false;
const int maxCacti = 10;
int gameLastCactus = -1;
LinkedList<int> cacti(maxCacti, -1);

TaskHandle_t backgroundTaskHandle;
void backgroundTask(void* params) {
    while (true) {
        // Parse GPS
        while (neogps.available()) gps.encode(neogps.read());

        // Check SMS
        Serial.println("Checking for new SMS...");
        SIM800lSerial.println("AT+CMGL=\"REC UNREAD\"");
        delay(500);

        String response = "";
        while (SIM800lSerial.available()) {
            char c = SIM800lSerial.read();
            response += c;
        }
        
        Serial.print("SIM800 Response: ");
        Serial.println(response);

        if (response.indexOf("+CMTI") != -1) {
            // SMS found, sending location
            SIM800lSerial.println("AT");
            SIM800lSerial.println("AT+CMGF=1");
            SIM800lSerial.println("AT+CMGD=1,4");
            SIM800lSerial.println("AT+CMGS=\"" + TEST_NUMBER + "\"");
            delay(500);
            SIM800lSerial.print("http://maps.google.com/maps?q=loc:");
            // TODO: Fix GPS
            SIM800lSerial.print(gps.location.lat(), 6);
            SIM800lSerial.print(",");
            SIM800lSerial.print(gps.location.lng(), 6);
            delay(100);
            SIM800lSerial.write(0x1A);

        } else {
            Serial.println("No unread SMS found.");
        }

        delay(2000);
    }
}

// TODO: BACKGROUND TASK FOR SMS

void registerCommands(Commands commands) {
    commands.addCommand("get_mac", [](Stream *serial, LinkedList<String> args) {
        serial->println(WiFi.macAddress());
    });

    commands.addCommand("get_version", [](Stream *serial, LinkedList<String> args) {
        serial->println(PRODUCT_VERSION);
    });

    commands.addCommand("echo", [](Stream *serial, LinkedList<String> args) {
        serial->println(args.get(0));
    });

    commands.addCommand("sim", [](Stream *serial, LinkedList<String> args) {
        String result = "";
        for (int i = 0; i < args.size(); i++) result += args.get(i) + " ";
        result.trim();
        SIM800lSerial.println(result);
    });
}

void setup() {
    // Init serial
    Serial.begin(115200);

    // Init preferences
    preferences.begin(PRODUCT_NAME.c_str(), false);

    // Load persistent variables
    // emergencyNumber = preferences.getString("emergencyNumber");
    emergencyNumber = TEST_NUMBER;

    // Init Bluetooth
    SerialBT.begin(DEVICE_NAME.c_str());

    // Begin SIM800L
    SIM800lSerial.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);
    SIM800lSerial.println("AT+CMGF=1");

    // Begin GPS
    // neogps.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    // Begin I2C for displays
    Wire.begin();
    smallDisplayWire.begin(SMALL_DISPLAY_SDA, SMALL_DISPLAY_SCL);

    // Begin Displays
    smallDisplay.begin(SSD1306_SWITCHCAPVCC, SMALL_DISPLAY_ADDRESS);
    bigDisplay.begin(SSD1306_SWITCHCAPVCC, BIG_DISPLAY_ADDRESS);
    bigDisplay.setRotation(2);

    // Clear displays
    smallDisplay.clearDisplay();
    bigDisplay.clearDisplay();

    smallDisplay.display();
    bigDisplay.display();

    // Register commands
    registerCommands(serialCommands);
    registerCommands(bluetoothCommands);

    // Bind buttons
    upButton.setOnRising([]() {
        currentScreen = Utils::Array::getRealIndex(screens, screensSize, currentScreen - 1);
    });

    dnButton.setOnRising([]() {
        currentScreen = Utils::Array::getRealIndex(screens, screensSize, currentScreen + 1);
    });

    // Final bootup
    vibrationMotor.vibrate(500);

    // Set LED Strip state
    delay(500);
    leds.begin();
    leds.setBrightness(69);
    leds.setSpeed(1);
    leds.setMode(FX_MODE_BREATH);
    leds.setColor(0x0000FF);
    leds.start();
    ledEffectDuration = -1;
    ledStart = millis();

    // Start background task
    xTaskCreate(backgroundTask, "backgroundTask", 4096, NULL, 1, &backgroundTaskHandle);
}

String currentMessage = "";
class BigDisplayUI {
    public:
        static void renderStatusBar() {
            // Current screen text
            bigDisplay.setTextSize(1);
            bigDisplay.setTextColor(SSD1306_WHITE);
            bigDisplay.setCursor(0, 0);
            bigDisplay.println(screens[currentScreen]);

            // Bluetooth icon
            bigDisplay.drawBitmap(BIG_DISPLAY_WIDTH - bluetooth_width - 2, 1, bluetooth, bluetooth_width, bluetooth_height, SSD1306_WHITE);
            
            // Signal icon
            bigDisplay.drawBitmap(BIG_DISPLAY_WIDTH - pin_star_width - 2 - bluetooth_width - 2, 1, pin_star, pin_star_width, pin_star_height, SSD1306_WHITE);

            // Attention icon
            bigDisplay.drawBitmap(BIG_DISPLAY_WIDTH - attention_width - 2 - pin_star_width - 2 - bluetooth_width - 2, 1, attention, attention_width, attention_height, SSD1306_WHITE);

            // Separation line
            bigDisplay.drawLine(0, 10, BIG_DISPLAY_WIDTH, 10, SSD1306_WHITE);
        }

        static void renderScreen() {
            if (currentScreen == 0) {
                if (calling) {
                    // Text
                    String callText = "Llamando...";
                    int16_t _;
                    uint16_t w, h;
                    bigDisplay.getTextBounds(callText, 0, 0, &_, &_, &w, &h);
                    bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2), BIG_DISPLAY_HEIGHT - 25);
                    bigDisplay.println(callText);

                    // Phone icon
                    bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) - (phone_check_big_width / 2), (BIG_DISPLAY_HEIGHT / 2) - (phone_check_big_height / 2) - 5, phone_check_big, phone_check_big_width, phone_check_big_height, SSD1306_WHITE);

                    // Serial.println("Si Pinguino");
                    if (!callingRequest) {
                        Serial.println("No Pinguino");
                        SIM800lSerial.println("AT");
                        SIM800lSerial.println("AT+CLVL=100");
                        delay(500);
                        SIM800lSerial.println("ATD" + TEST_NUMBER + ";");
                        SIM800lSerial.println("AT+CMGF=1");
                        SIM800lSerial.println("AT+CMGD=1,4");
                        SIM800lSerial.println("AT+CMGS=\"" + TEST_NUMBER + "\"");
                        delay(500);
                        SIM800lSerial.print("http://maps.google.com/maps?q=loc:");
                        // TODO: Fix GPS
                        SIM800lSerial.print(gps.location.lat(), 6);
                        SIM800lSerial.print(",");
                        SIM800lSerial.print(gps.location.lng(), 6);
                        delay(100);
                        SIM800lSerial.write(0x1A);
                        callingRequest = true;
                        while (!SIM800lSerial.available()) delay(1);
                        if (!SIM800lSerial.available()) Serial.write(SIM800lSerial.read());
                        leds.setSpeed(1);
                        leds.setMode(FX_MODE_BREATH);
                        leds.setColor(0x00FF00);
                        leds.start();
                        ledEffectDuration = 250;
                        ledStart = millis();
                    }

                    // Hang up
                    backButton.setOnRising([]() {
                        SIM800lSerial.println("AT");
                        SIM800lSerial.println("ATH");
                        callingRequest = false;
                        calling = false;
                        buttonStart = -1;
                    });
                } else {
                    if (buttonStart > -1) {
                        if (millis() - buttonStart < buttonDuration) {
                            // Text
                            String callText = "Manten presionado...";
                            int16_t _;
                            uint16_t w, h;
                            bigDisplay.getTextBounds(callText, 0, 0, &_, &_, &w, &h);
                            bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2), BIG_DISPLAY_HEIGHT - 25);
                            bigDisplay.println(callText);

                            // Draw progress bar
                            bigDisplay.drawRect(2, BIG_DISPLAY_HEIGHT - 10, BIG_DISPLAY_WIDTH - 4, 8, SSD1306_WHITE);
                            bigDisplay.fillRect(2, BIG_DISPLAY_HEIGHT - 10, map(millis() - buttonStart, 0, buttonDuration, 0, BIG_DISPLAY_WIDTH - 4), 8, SSD1306_WHITE);
            
                            // Phone icon
                            bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) - (phone_outgoing_big_width / 2), (BIG_DISPLAY_HEIGHT / 2) - (phone_outgoing_big_height / 2) - 5, phone_outgoing_big, phone_check_big_width, phone_check_big_height, SSD1306_WHITE);
                        } else {
                            // Call
                            calling = true;
                        }
                    } else {
                        // Text
                        String callText = "Llamar";
                        int16_t _;
                        uint16_t w, h;
                        bigDisplay.getTextBounds(callText, 0, 0, &_, &_, &w, &h);
                        bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2) - 2, BIG_DISPLAY_HEIGHT - 25);
                        bigDisplay.println(callText);
                        
                        // Arrow icon
                        bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) + (w / 2) + 2, BIG_DISPLAY_HEIGHT - 25, arrow_right, arrow_right_width, arrow_right_height, SSD1306_WHITE);

                        // Phone icon
                        bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) - (phone_outgoing_big_width / 2), (BIG_DISPLAY_HEIGHT / 2) - (phone_outgoing_big_height / 2) - 5, phone_outgoing_big, phone_check_big_width, phone_check_big_height, SSD1306_WHITE);

                        // Add listener for OK button
                        okButton.setOnRising([]() {
                            buttonStart = millis();
                        });

                        okButton.setOnFalling([]() {
                            if (millis() - buttonStart < buttonDuration) buttonStart = -1;
                        });
                    }
                }
                gameStart = -1;
            } else if (currentScreen == 1) {
                // Text
                String callText = "Mensaje";
                int16_t _;
                uint16_t w, h;
                bigDisplay.getTextBounds(callText, 0, 0, &_, &_, &w, &h);
                bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2) - 2, 15);
                bigDisplay.println(callText);
                
                // Arrow icon
                bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) + (w / 2) + 2, 15, arrow_right, arrow_right_width, arrow_right_height, SSD1306_WHITE);

                // Message
                bigDisplay.setCursor(0, 32);
                bigDisplay.println(currentMessage);

                // Bindings
                okButton.setOnRising([]() {
                    leds.setMode(FX_MODE_BLINK);
                    leds.setColor(0x0000FF);
                    leds.setSpeed(1);
                    leds.start();
                    ledStart = millis();
                    ledEffectDuration = 250;
                    currentMessage = mensajes[random(0, sizeof(mensajes) / sizeof(mensajes[0]))];
                });
                okButton.setOnFalling(NULL);
                gameStart = -1;
            } else if (currentScreen == 2) {
                if (gameStart > -1 ) {
                    // ! GAME
                    int nowTime = millis();
                    gameLost = false;

                    okButton.setOnRising([]() {
                        gameIsJumping = true;
                    });

                    okButton.setOnFalling([]() {
                        gameIsJumping = false;
                    });

                    if (nowTime - gameStart > GAME_CACTUS_EVERY) {
                        cacti.add(BIG_DISPLAY_WIDTH);
                        gameStart = nowTime;
                    }

                    // Draw cacti
                    for (int i = 0; i < cacti.size(); i++) {
                        int cactus = cacti.get(i);
                        if (cactus > -1) {
                            bigDisplay.drawBitmap(cactus, BIG_DISPLAY_HEIGHT - 2 - cacatus_height, cacatus, cacatus_width, cacatus_height, SSD1306_WHITE);
                            cacti.set(i, cactus - GAME_SPEED);

                            // Check collision
                            if (!gameIsJumping && 10 + dino_width > cactus) {
                                // GAME OVER
                                cacti.clear();
                                GAME_SCORE = 0;
                                gameLost = true;
                                leds.setMode(FX_MODE_BLINK);
                                leds.setColor(0xFF0000);
                                leds.setSpeed(2);
                                leds.start();
                                ledStart = millis();
                                ledEffectDuration = 500;
                                buzzer.beep(500, 250);
                                vibrationMotor.vibrate(500);
                                gameStart = -1;
                            }
                        } else {
                            cacti.remove(i);
                            // Adds 10 at beginning
                            GAME_SCORE++;

                            leds.setMode(FX_MODE_BLINK);
                            leds.setColor(0x00FF00);
                            leds.setSpeed(2);
                            leds.start();
                            ledStart = millis();
                            ledEffectDuration = 250;
                        }
                    }

                    // Draw player
                    if (gameIsJumping) {
                        bigDisplay.drawBitmap(10, BIG_DISPLAY_HEIGHT - 2 - dino_height - GAME_JUMP_HEIGHT, dino, dino_width, dino_height, SSD1306_WHITE);
                    } else {
                        bigDisplay.drawBitmap(10, BIG_DISPLAY_HEIGHT - 2 - dino_height, dino, dino_width, dino_height, SSD1306_WHITE);
                    }

                    // Draw Score
                    String text = "Score: " + String(GAME_SCORE);
                    int16_t _;
                    uint16_t w, h;
                    bigDisplay.getTextBounds(text, 0, 0, &_, &_, &w, &h);
                    bigDisplay.setCursor(BIG_DISPLAY_WIDTH - w - 2, 15);
                    bigDisplay.println(text);

                    // Exit 
                    backButton.setOnRising([]() {
                        cacti.clear();
                        GAME_SCORE = 0;
                        gameLost = false;
                        gameStart = -1;
                    });
                } else {
                    // ! START SCREEN
                    // Text
                    String text = "Iniciar";
                    int16_t _;
                    uint16_t w, h;
                    bigDisplay.getTextBounds(text, 0, 0, &_, &_, &w, &h);
                    bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2) - 2, 15);
                    bigDisplay.println(text);

                    // If lost
                    if (gameLost) {
                        // Text
                        String text = "--- Chocaste ---";
                        int16_t _;
                        uint16_t w, h;
                        bigDisplay.getTextBounds(text, 0, 0, &_, &_, &w, &h);
                        bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2) - 2, (BIG_DISPLAY_HEIGHT / 2) - (h / 2));
                        bigDisplay.println(text);
                    }

                    // Arrow icon
                    bigDisplay.drawBitmap((BIG_DISPLAY_WIDTH / 2) + (w / 2) + 2, 15, arrow_right, arrow_right_width, arrow_right_height, SSD1306_WHITE);

                    okButton.setOnRising([]() {
                        gameStart = millis();
                    });

                    // draw player
                    bigDisplay.drawBitmap(10, BIG_DISPLAY_HEIGHT - 2 - dino_height, dino, dino_width, dino_height, SSD1306_WHITE);

                    // Draw cacti
                    for (int i = 0; i < cacti.size(); i++) {
                        int cactus = cacti.get(i);
                        if (cactus > -1) {
                            bigDisplay.drawBitmap(cactus, BIG_DISPLAY_HEIGHT - 2 - cacatus_height, cacatus, cacatus_width, cacatus_height, SSD1306_WHITE);
                        }
                    }
                }

            } else if (currentScreen == 3) {
                // Render Proximamente in the center of the screen
                String text = "Proximamente";
                int16_t _;
                uint16_t w, h;
                bigDisplay.getTextBounds(text, 0, 0, &_, &_, &w, &h);
                bigDisplay.setCursor((BIG_DISPLAY_WIDTH / 2) - (w / 2) - 2, (BIG_DISPLAY_HEIGHT / 2) - (h / 2));
                bigDisplay.println(text);
            }
        }

        static void render() {
            bigDisplay.clearDisplay();
            renderStatusBar();
            renderScreen();
            bigDisplay.display();
        }
};

class SmallDisplayUI {
    public:
        static void renderScreenList() {
            int previousScreen = Utils::Array::getRealIndex(screens, screensSize, currentScreen - 1);
            int nextScreen = Utils::Array::getRealIndex(screens, screensSize, currentScreen + 1);
            
            smallDisplay.clearDisplay();
            smallDisplay.setTextSize(1);
            smallDisplay.setTextColor(SSD1306_WHITE);
            smallDisplay.drawBitmap(2, 10, arrow_down, arrow_down_width, arrow_down_height, SSD1306_WHITE);
            smallDisplay.setCursor(2 + arrow_left_width + 2, 10);
            smallDisplay.println(screens[previousScreen]);

            int16_t _;
            uint16_t w, h;
            smallDisplay.getTextBounds(screens[nextScreen], 0, 0, &_, &_, &w, &h);
            smallDisplay.drawBitmap(SMALL_DISPLAY_WIDTH - 2 - arrow_up_width, 2, arrow_up, arrow_up_width, arrow_up_height, SSD1306_WHITE);
            smallDisplay.setCursor(SMALL_DISPLAY_WIDTH - 2 - arrow_up_width - w - 2, 2);
            smallDisplay.println(screens[nextScreen]);
        }

        static void renderBottomBar() {
            // TODO: Make time blink
            String time = rtc.getTime("%H:%M:%S");
            int16_t _;
            uint16_t w, h;
            smallDisplay.getTextBounds(time, 0, 0, &_, &_, &w, &h);
            smallDisplay.setCursor((SMALL_DISPLAY_WIDTH / 2) - (w / 2), SMALL_DISPLAY_HEIGHT - h);
            smallDisplay.println(time);

            smallDisplay.drawLine(0, SMALL_DISPLAY_HEIGHT - 10, SMALL_DISPLAY_WIDTH, SMALL_DISPLAY_HEIGHT - 10, SSD1306_WHITE);
        }

        static void render() {
            smallDisplay.clearDisplay();
            renderScreenList();
            renderBottomBar();
            smallDisplay.display();
        }
};

void loop() {
    // HAL services
    backButton.service();
    upButton.service();
    dnButton.service();
    okButton.service();
    // potentiometer.service();
    // vibrationMotor.service();
    // buzzer.service();

    // Displays
    // * BIG DISPLAY
    BigDisplayUI::render();

    // * SMALL DISPLAY
    SmallDisplayUI::render();

    // Serial commands
    if (Serial.available()) serialCommands.readSerial();
    if (SerialBT.available()) bluetoothCommands.readSerial();

    // LED Strip
    if (millis() - ledStart > ledEffectDuration) leds.setColor(0);
    leds.service();
}

#endif