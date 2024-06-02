#ifndef COMFYUI_H
#define COMFYUI_H

// Include external libraries
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>

// Include ComfyUI utilities
#include "comfyui/icons.h"
#include "comfyui/enums.h"
#include "comfyui/types.h"

// Include all elements
#include "comfyui/elements/ComfyUIElement.h"
#include "comfyui/elements/ComfyUIFrame.h"
#include "comfyui/elements/ComfyUIText.h"
#include "comfyui/elements/ComfyUIButton.h"
#include "comfyui/elements/ComfyUIButtonList.h"

class ComfyUI {
    public:
        Adafruit_SSD1306* display;
        ComfyUIFrame root;

        ComfyUI(Adafruit_SSD1306* display) {
            this->display = display;
            this->root = ComfyUIFrame(0, 0, display->width(), display->height());
        }

        void addElement(ComfyUIElement* element) {
            this->root.addChild(element);
        }

        void update() {
            // Update the root element's dimensions
            this->root.width = this->display->width();
            this->root.height = this->display->height();

            // Clear the display
            this->display->clearDisplay();

            // Draw the root element subsequently drawing all children
            this->root.draw(this->display, NULL);

            // Display the updated graphics
            this->display->display();
        }
};

#endif