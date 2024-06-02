#ifndef COMFYUI_H
#define COMFYUI_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>
#include "comfyui/icons.h"

class ComfyUIElement {
    public:
        virtual void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        void update() {}
};

class ComfyUIFrame : public ComfyUIElement {
    public:
        int x;
        int y;
        int width;
        int height;
        LinkedList<ComfyUIElement*> children;

    public:
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
                element->update();
                element->draw(display, this);
            }
        }

        int getWidth() {
            return this->width;
        }

        int getHeight() {
            return this->height;
        }
};

class ComfyUIButton : public ComfyUIElement {
    private:
        int x;
        int y;
        String text;
        int paddingX;
        int paddingY;
        int textSize;
        int color1;
        int color2;
        int roundness;
        bool selected = false;
        unsigned char icon[64];
        unsigned int iconWidth;
        unsigned int iconHeight;

        int width;
        int height;
    
    public:
        ComfyUIButton(int x = 0, int y = 0, String text = "ComfyUI Button", const unsigned char icon[] = NULL, const unsigned int iconWidth = 0, const unsigned int iconHeight = 0, int paddingX = 10, int paddingY = 5, int textSize = 1, int color1 = SSD1306_WHITE, int color2 = SSD1306_BLACK, int roundness = 3) {
            this->x = x;
            this->y = y;
            this->text = text;
            if (icon != NULL) {
                memcpy(this->icon, icon, sizeof(this->icon));
                this->iconWidth = iconWidth;
                this->iconHeight = iconHeight;
            }
            this->paddingX = paddingX;
            this->paddingY = paddingY;
            this->textSize = textSize;
            this->color1 = color1;
            this->color2 = color2;
            this->roundness = roundness;
        }

        void setX(int x) {
            this->x = x;
        }

        void setY(int y) {
            this->y = y;
        }

        void setText(String text) {
            this->text = text;
        }

        void setPaddingX(int paddingX) {
            this->paddingX = paddingX;
        }

        void setPaddingY(int paddingY) {
            this->paddingY = paddingY;
        }

        void setTextSize(int textSize) {
            this->textSize = textSize;
        }

        void setColor1(int color1) {
            this->color1 = color1;
        }

        void setColor2(int setColor2) {
            this->color2 = color2;
        }

        void setRoundness(int roundness) {
            this->roundness = roundness;
        }

        void setSelected(bool selected) {
            this->selected = selected;
        }

        bool getSelected() {
            return this->selected;
        }

        int getWidth() {
            return 0;
        }

        int getHeight() {
            return 15;
        }

        void addChild(ComfyUIElement* element) {}

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            // Set the text size
            display->setTextSize(this->textSize);
            
            // Calculate icon padding if an icon is present
            int iconPadding = (this->iconWidth > 0) ? this->iconWidth + (paddingX * 2) : 0;
            
            // Set the cursor position with consideration for icon padding
            display->setCursor(x + iconPadding + paddingX, y + paddingY);
            
            // Get the text bounds
            int16_t textX, textY; // These are unused but required by getTextBounds
            uint16_t textWidth, textHeight;
            display->getTextBounds(text, display->getCursorX(), display->getCursorY(), &textX, &textY, &textWidth, &textHeight);
            
            // Calculate rectangle dimensions
            int rectWidth = this->selected ? parent->getWidth() - paddingX : textWidth + (paddingX * 2) + iconPadding;
            int rectHeight = textHeight + (paddingY * 2);

            // Store width and height
            this->width = rectWidth;
            this->height = rectHeight;

            if (this->selected) {
                // Draw filled rectangle for selected state
                display->fillRoundRect(x, y, rectWidth, rectHeight, this->roundness, this->color1);
                // Draw right arrow bitmap
                display->drawBitmap(x + rectWidth - arrow_right_width - paddingX, y + (rectHeight - arrow_right_height) / 2, arrow_right, arrow_right_width, arrow_right_height, this->color2);
            } else {
                // Draw outlined rectangle for non-selected state
                display->drawRoundRect(x, y, rectWidth, rectHeight, this->roundness, this->color1);
            }
            
            // Draw icon if present
            if (this->iconWidth > 0) 
                display->drawBitmap(x + paddingX, y + paddingY, this->icon, this->iconWidth, this->iconHeight, this->selected ? this->color2 : this->color1);
            
            // Set text color and print the text
            display->setTextColor(this->selected ? this->color2 : this->color1);
            display->println(text);
        }
};

class ComfyUIButtonList : public ComfyUIFrame {
    public:
        int spacing;
        ComfyUIButtonList(int x = 0, int y = 0, int width = 128, int height = 64, int spacing = 3) : ComfyUIFrame(x, y, width, height) {
            this->spacing = spacing;
        }

        void addButton(ComfyUIButton* element) {
            element->setX(this->x);
            element->setY(this->y + (this->children.size() * (element->getHeight() + this->spacing)));
            this->addChild(element);
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            for (int i = 0; i < children.size(); i++) {
                ComfyUIElement* element = children.get(i);
                element->update();
                element->draw(display, this);
            }
        }
};

class ComfyUI {
    private:
        Adafruit_SSD1306* display;
        ComfyUIFrame root = ComfyUIFrame(0, 0, 0, 0);

    public:
        ComfyUI(Adafruit_SSD1306* display) {
            // Set display
            this->display = display;

            // Update root frame's dimensions
            this->root.width = display->width();
            this->root.height = display->height();
        }

        void addElement(ComfyUIElement* element) {
            this->root.addChild(element);
        }

        void update() {
            // Clear display
            display->clearDisplay();

            // Draw elements (draws root frame's children recursively)
            this->root.update();
            this->root.draw(display, &this->root);

            // Display updated display
            display->display();
        }
};

#endif