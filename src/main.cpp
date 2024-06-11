#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WS2812FX.h>
#include "hal/PushButton.cpp"
#include "hal/VibrationMotor.cpp"
#include "hal/Potentiometer.cpp"
#include "hal/Buzzer.cpp"
#include "comfyui/ComfyUI2.cpp"
#include "comfyui/icons.h"
#include "comfyui/elements/ComfyUITabs.h"
#include "aikey.h"
#include <WiFi.h>
#include <Preferences.h>
#include "lib/Commands.cpp"
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "./ble.h"

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

// LED Config
const int LED_PIN = 2;
const int LED_COUNT = 10;
int LED_BRIGHTNESS = 100;
WS2812FX leds = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// GPS Config
const int GPS_RX = 16;
const int GPS_TX = 17;
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

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

// Preferences
Preferences preferences;

// Context
enum context_t {
    CALL,
    MESSAGES,
    GAMES
};

// ComfyUI
ComfyUI bigUI(&bigDisplay);

ComfyUIButton callButton = ComfyUIButton(10, 10, "Llamada", volup, volup_width, volup_height);
ComfyUIButton messagesButton = ComfyUIButton(10, 30, "Mensajes", sdcardmounted, sdcardmounted_width, sdcardmounted_height);
ComfyUIButton gamesButton = ComfyUIButton(10, 50, "Juegos", gamemode, gamemode_width, gamemode_height);

ComfyUIButtonList buttonList = ComfyUIButtonList();

ComfyUITabs tabsUI = ComfyUITabs();

ComfyUIFrame callFrame = ComfyUIFrame();
ComfyUIFrame messagesFrame = ComfyUIFrame();
ComfyUIFrame gamesFrame = ComfyUIFrame();

ComfyUIText callText = ComfyUIText(10, 10, "Llamada");
ComfyUIText mensajesText = ComfyUIText(10, 10, "Mensajes");
ComfyUIText gamesText = ComfyUIText(10, 10, "Juegos");

// Buttons
PushButton button1(13);
PushButton button2(12);
PushButton button3(14);
PushButton button4(27);

// Potentiometer
Potentiometer potentiometer(15);

// Vibration motor
VibrationMotor vibrationMotor(4);

// Buzzer
Buzzer buzzer(5);

// Commands
Commands serialCommands(&Serial);

// Loops
unsigned long dt = 0;

TaskHandle_t backgroundLoopTask;
void backgroundLoop(void *params) {
    while (true) {
        smallDisplay.clearDisplay();
        smallDisplay.setTextColor(SSD1306_WHITE);
        smallDisplay.setTextSize(2);
        smallDisplay.setCursor(0, 0);
        smallDisplay.print("FPS: ");
        smallDisplay.println(1000 / (int)dt);
        smallDisplay.print("MEM: ");
        smallDisplay.print((ESP.getFreeHeap() * 100) / ESP.getHeapSize());
        smallDisplay.println("%");
        smallDisplay.display();

        leds.service();
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    // * Begin console Serial
    Serial.begin(115200);

    // * Begin BLE
    BLEDevice::init(DEVICE_NAME.c_str());
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pUserService = pServer->createService(BLE_USER_SERVICE_UUID);
    BLECharacteristic *pUserNameCharacteristic = pUserService->createCharacteristic(
        BLE_USER_NAME_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pUserNameCharacteristic->setValue("USER_NAME");
    BLECharacteristic *pUserPhoneCharacteristic = pUserService->createCharacteristic(
        BLE_USER_PHONE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pUserPhoneCharacteristic->setValue("USER_PHONE");
    BLECharacteristic *pUserConditionCharacteristic = pUserService->createCharacteristic(
        BLE_USER_CONDITION_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pUserConditionCharacteristic->setValue("USER_CONDITION");
    pUserService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_USER_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    // * Begin Preferences
    preferences.begin(PRODUCT_NAME.c_str(), false);

    // * Begin GPS Serial
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    // * Begin LEDs
    leds.begin();
    leds.setBrightness(LED_BRIGHTNESS);
    leds.setSpeed(5000);
    leds.setMode(FX_MODE_RAINBOW_CYCLE);
    leds.start();

    // * Begin I2C for displays
    Wire.begin();
    smallDisplayWire.begin(SMALL_DISPLAY_SDA, SMALL_DISPLAY_SCL);

    // * Begin Displays
    smallDisplay.begin(SSD1306_SWITCHCAPVCC, SMALL_DISPLAY_ADDRESS);
    bigDisplay.begin(SSD1306_SWITCHCAPVCC, BIG_DISPLAY_ADDRESS);

    // * UI
    buttonList.addChild(&callButton);
    buttonList.addChild(&messagesButton);
    buttonList.addChild(&gamesButton);

    tabsUI.setTabsList(&buttonList);

    callFrame.addChild(&callText);
    messagesFrame.addChild(&mensajesText);
    gamesFrame.addChild(&gamesText);
    
    tabsUI.addTab("Llamada", &callFrame);
    tabsUI.addTab("Mensajes", &messagesFrame);
    tabsUI.addTab("Juegos", &gamesFrame);

    callButton.setPressedCallback([]() {
        tabsUI.setTab("Llamada");
    });

    messagesButton.setPressedCallback([]() {
        tabsUI.setTab("Mensajes");
    });

    gamesButton.setPressedCallback([]() {
        tabsUI.setTab("Juegos");
    });

    button1.setOnRising([]() {
        tabsUI.setTab();
        vibrationMotor.vibrate(100);
        buzzer.beep(1500, 100);
    });

    bigUI.addElement(&tabsUI);

    button2.setOnRising([]() {
        buttonList.selectPrevious();
        vibrationMotor.vibrate(100);
        buzzer.beep(1500, 100);
    });

    button3.setOnRising([]() {
        buttonList.selectNext();
        vibrationMotor.vibrate(100);
        buzzer.beep(1500, 100);
    });

    button4.setOnRising([]() {
        buttonList.getSelectedButton()->setPressed(true);
        vibrationMotor.vibrate(250);
        buzzer.beep(200, 250);
    });

    button4.setOnFalling([]() {
        buttonList.getSelectedButton()->setPressed(false);
    });

    serialCommands.addCommand("get_mac", [](Stream *serial, LinkedList<String> args) {
        serial->println(WiFi.macAddress());
    });

    vibrationMotor.vibrate(1000);

    xTaskCreate(
        backgroundLoop,
        "backgroundLoop",
        4096,
        NULL,
        1,
        &backgroundLoopTask
    );
}

//! REMEMBER DO NOT USE DELAY IN LOOP
unsigned long lastTime = 0;
unsigned long lastChange = 0;
uint changePeriod = 500;
void loop() {
    unsigned long currentTime = millis();
    dt = currentTime - lastTime;

    bigUI.update();

    button1.service();
    button2.service();
    button3.service();
    button4.service();

    vibrationMotor.service();

    buzzer.service(); 

    if (Serial.available()) serialCommands.readSerial();

    lastTime = currentTime;
}