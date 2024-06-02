#ifndef COMFYUI_H
#define COMFYUI_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>
#include "comfyui/icons.h"
#include "comfyui/enums.h"

class ComfyUIElement {
    public:
        /* Draws the element with precalculated values from the update function */
        virtual void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) = 0;

        /* Updates and calcualtes elements dimensions and contents, DO NOT DRAW in this function*/
        virtual void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) = 0;

        /* Returns width of element*/
        virtual int getWidth() = 0;

        /* Returns height of element*/
        virtual int getHeight() = 0;

        /* Returns x position of element*/
        virtual int getX() = 0;

        /* Returns y position of element*/
        virtual int getY() = 0;
};

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

class ComfyUIButton : public ComfyUIElement {
    public:
        int x;
        int y;
        int originalX;
        int originalY;
        int width;
        int height;
        String text;
        int textSize;
        int color1;
        int color2;
        int paddingX;
        int paddingY;
        int roundness;
        bool selected;
        bool pressed;

        ComfyUIButton(int x, int y, String text, int textSize = 1, int color1 = SSD1306_WHITE, int color2 = SSD1306_BLACK, int paddingX = 10, int paddingY = 3, int roundness = 3) {
            this->x = x + paddingX;
            this->y = y + paddingY;
            this->originalX = x;
            this->originalY = y;
            this->text = text;
            this->textSize = textSize;
            this->color1 = color1;
            this->color2 = color2;
            this->paddingX = paddingX;
            this->paddingY = paddingY;
            this->roundness = roundness;
            this->selected = false;
            this->pressed = false;
        }

        bool getSelected() {
            return this->selected;
        }

        void setSelected(bool selected) {
            this->selected = selected;
        }

        bool getPressed() {
            return this->pressed;
        }

        void setPressed(bool pressed) {
            this->pressed = pressed;
        }

        // TODO: PLEASE CHANGE THIS IT WORKS TERRIBLY.
        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            int16_t x1, y1;
            uint16_t w, h;
            display->getTextBounds(this->text, this->x, this->y, &x1, &y1, &w, &h);
            
            if (this->selected) {
                this->width = parent->getWidth() - (this->paddingX * 2);
                this->x = parent->getX() + this->paddingX;
                this->y = originalY;
            } else {
                this->width = w + this->paddingX * 2;
                this->x = originalX + this->paddingX;
                this->y = originalY + this->paddingY;
            }

            this->height = h + this->paddingY * 2;
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            display->drawRoundRect(this->x, this->y, this->width, this->height, this->roundness, this->color1);
            display->setTextSize(this->textSize);
            display->setTextColor(this->color2);
            display->setCursor(this->x + this->paddingX, this->y + this->paddingY);
            display->println(this->text);
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
            this->display->clearDisplay();
            this->root.draw(this->display, NULL);
            this->display->display();
        }
};

#endif