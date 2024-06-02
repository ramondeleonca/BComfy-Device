#ifndef COMFYUI_TEXT_H
#define COMFYUI_TEXT_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <comfyui/elements/ComfyUIElement.h>
#include <comfyui/types.h>

class ComfyUIText : public ComfyUIElement {
    public:
        int x;
        int originalX;
        int y;
        int width;
        int height;
        String text;
        int textSize;
        int color;
        int paddingX;
        comfy_ui_align align;

    public:
        ComfyUIText(int x, int y, String text, int textSize = 1, comfy_ui_align align = comfy_ui_align::LEFT, int paddingX = 0, int color = SSD1306_WHITE) {
            this->x = x + paddingX;
            this->originalX = x;
            this->y = y;
            this->text = text;
            this->textSize = textSize;
            this->align = align;
            this->paddingX = paddingX;
            this->color = color;
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            display->setTextSize(this->textSize);
            display->setTextColor(this->color);
            display->setCursor(this->x, this->y);
            display->println(this->text);
        }

        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            int16_t x1, y1;
            uint16_t w, h;
            display->getTextBounds(this->text, this->x, this->y, &x1, &y1, &w, &h);
            if (this->align == comfy_ui_align::CENTER) {
                this->x = (parent->getWidth() - w) / 2;
            } else if (this->align == comfy_ui_align::RIGHT) {
                this->x = parent->getWidth() - w - this->paddingX;
            }
            this->width = w;
            this->height = h;
        }

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
