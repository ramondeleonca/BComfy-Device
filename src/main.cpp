#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WS2812FX.h>
#include "hal/PushButton.cpp"
#include "hal/VibrationMotor.cpp"
#include "comfyui/ComfyUI.cpp"
#include "comfyui/icons.h"

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
const int BIG_DISPLAY_WIDTH = 128;
const int BIG_DISPLAY_HEIGHT = 64;
const int BIG_DISPLAY_ADDRESS = 0x3C;
Adafruit_SSD1306 bigDisplay(BIG_DISPLAY_WIDTH, BIG_DISPLAY_HEIGHT, &Wire, -1);

const int SMALL_DISPLAY_WIDTH = 128;
const int SMALL_DISPLAY_HEIGHT = 32;
const int SMALL_DISPLAY_ADDRESS = 0x3C;
const int SMALL_DISPLAY_SDA = 18;
const int SMALL_DISPLAY_SCL = 19;
TwoWire smallDisplayWire(1);
Adafruit_SSD1306 smallDisplay(SMALL_DISPLAY_WIDTH, SMALL_DISPLAY_HEIGHT, &smallDisplayWire, -1);

// ComfyUI
ComfyUI bigUI(&bigDisplay);

ComfyUIButton button1UI = ComfyUIButton(10, 10, "Llamada", phone_outgoing, phone_outgoing_width, phone_outgoing_height);
ComfyUIButton button2UI = ComfyUIButton(10, 30, "Mensajes", message, message_width, message_height);
ComfyUIButton button3UI = ComfyUIButton(10, 50, "Juegos", game_controller, game_controller_width, game_controller_height);

ComfyUIButtonList buttonListUI = ComfyUIButtonList();

// Buttons
PushButton button1(13);
PushButton button2(12);
PushButton button3(14);
PushButton button4(27);

// Vibration motor
VibrationMotor vibrationMotor(4);

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    leds.begin();
    leds.setBrightness(LED_BRIGHTNESS);
    leds.setSpeed(5000);
    leds.setMode(FX_MODE_RAINBOW_CYCLE);
    leds.start();

    Wire.begin();
    smallDisplayWire.begin(SMALL_DISPLAY_SDA, SMALL_DISPLAY_SCL);

    smallDisplay.begin(SSD1306_SWITCHCAPVCC, SMALL_DISPLAY_ADDRESS);
    bigDisplay.begin(SSD1306_SWITCHCAPVCC, BIG_DISPLAY_ADDRESS);

    smallDisplay.display();
    bigDisplay.display();

    buttonListUI.addButton(&button1UI);
    buttonListUI.addButton(&button2UI);
    buttonListUI.addButton(&button3UI);

    bigUI.addElement(&buttonListUI);
}

//! REMEMBER DO NOT USE DELAY IN LOOP
unsigned long lastTime = 0;
void loop() {
    unsigned long currentTime = millis();
    unsigned long dt = currentTime - lastTime;

    button1UI.setSelected(!button1UI.getSelected());

    bigUI.update();

    smallDisplay.clearDisplay();
    smallDisplay.setTextColor(SSD1306_WHITE);
    smallDisplay.setTextSize(2);
    smallDisplay.setCursor(0, 0);
    smallDisplay.print("FPS: ");
    smallDisplay.print(1000 / (float)dt);
    smallDisplay.display();

    leds.service();

    button1.service();
    button2.service();
    button3.service();
    button4.service();

    // delay(500);

    lastTime = currentTime;
}