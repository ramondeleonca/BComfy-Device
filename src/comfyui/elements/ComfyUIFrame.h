#ifndef COMFYUI_FRAME_H
#define COMFYUI_FRAME_H

#include "comfyui/elements/ComfyUIElement.h"
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>

class ComfyUIFrame : public ComfyUIElement {
    public:
        int x;
        int y;
        int width;
        int height;
        LinkedList<ComfyUIElement*> children;

        ComfyUIFrame(int x = 0, int y = 0, int width = 128, int height = 64) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            this->children = LinkedList<ComfyUIElement*>();
        }

        void addChild(ComfyUIElement* element) {
            this->children.add(element);
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            for (int i = 0; i < children.size(); i++) {
                ComfyUIElement* element = children.get(i);
                element->update(display, this);
                element->draw(display, this);
            }
        }

        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {}

        int getWidth() {
            return this->width;
        }

        int getHeight() {
            return this->height;
        }

        int getX() {
            return this->x;
        }

        int getY() {
            return this->y;
        }
};

#endif