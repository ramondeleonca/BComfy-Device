#include <Arduino.h>
#include <comfyui/elements/ComfyUIFrame.h>
#include <comfyui/elements/ComfyUIButton.h>
#include <comfyui/elements/ComfyUIText.h>
#include <comfyui/elements/ComfyUIButtonList.h>
#include <comfyui/ComfyUI2.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
ComfyUI ui;
ComfyUIFrame frame;
ComfyUIButton button1;
ComfyUIButton button2;
ComfyUIButton button3;
ComfyUIButtonList buttonList;

void setupComfyUI() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    
    ui = ComfyUI(&display);
    frame = ComfyUIFrame(0, 0, 128, 64);
    button1 = ComfyUIButton(0, 0, "Button 1");
    button2 = ComfyUIButton(64, 0, "Button 2");
    button3 = ComfyUIButton(0, 32, "Button 3");
    buttonList = ComfyUIButtonList();
    buttonList.addChild(&button1);
    buttonList.addChild(&button2);
    buttonList.addChild(&button3);
    frame.addChild(&buttonList);
    ui.addElement(&frame);
}