#ifndef COMFYUI_BUTTON_H
#define COMFYUI_BUTTON_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <comfyui/elements/ComfyUIElement.h>
#include <comfyui/types.h>
#include "comfyui/easings.h"

class ComfyUIButton : public ComfyUIElement {
    private:
        // Target bounds
        int x;
        int lastX;
        int originalX;

        int y;
        int lastY;
        int originalY;

        int width;
        int lastWidth;

        int height;
        int lastHeight;

        // Current bounds
        int currentX;
        int currentY;
        int currentWidth;
        int currentHeight;

        // Padding
        int paddingX;
        int paddingY;

        // Text
        String text;
        int textSize;
        
        // Colors
        int color1;
        int color2;

        // Style
        int roundness;

        // State
        bool selected = false;
        bool pressed = false;
        bool lastPressed = false;

        // Icon
        unsigned char icon[64];
        unsigned int iconWidth;
        unsigned int iconHeight;

        // Animation
        int startedAnimatingX;
        int startedAnimatingY;
        int startedAnimatingWidth;
        int startedAnimatingHeight;
        bool firstUpdate = true;

        // Pressed callback
        void (*pressedCallback)() = NULL;

    public:
        ComfyUIButton(int x = 0, int y = 0, String text = "ComfyUI Button", const unsigned char icon[] = NULL, const unsigned int iconWidth = 0, const unsigned int iconHeight = 0, int paddingX = 10, int paddingY = 5, int textSize = 1, int color1 = SSD1306_WHITE, int color2 = SSD1306_BLACK, int roundness = 3) {
            this->x = x;
            this->lastX = x;
            this->originalX = x;
            this->y = y;
            this->lastY = y;
            this->originalY = y;
            this->currentX = x;
            this->currentY = y;
            this->text = text;
            this->paddingX = paddingX;
            this->paddingY = paddingY;
            this->textSize = textSize;
            this->color1 = color1;
            this->color2 = color2;
            this->roundness = roundness;
            this->iconWidth = iconWidth;
            this->iconHeight = iconHeight;
            if (icon != NULL) memcpy(this->icon, icon, sizeof(this->icon));
        }

        void setSelected(bool selected) {
            this->selected = selected;
        }

        void setPressed(bool pressed) {
            this->pressed = pressed;
        }

        bool isSelected() {
            return this->selected;
        }

        int getWidth() {
            return this->currentWidth;
        }

        int getHeight() {
            return this->currentHeight;
        }

        int getX() {
            return this->currentX;
        }

        int getY() {
            return this->currentY;
        }

        void setX(int x) {
            this->x = x;
        }

        void setY(int y) {
            this->y = y;
        }

        void setPressedCallback(void (*pressedCallback)()) {
            this->pressedCallback = pressedCallback;
        }

        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            // Get the text bounds
            display->setTextSize(this->textSize);
            display->setTextWrap(false);
            int16_t _;
            uint16_t textWidth, textHeight;
            display->getTextBounds(text, 0, 0, &_, &_, &textWidth, &textHeight);

            int width = textWidth + this->paddingX * 2;
            int height = textHeight + this->paddingY * 2;

            // First update
            if (this->firstUpdate) {
                this->width = width;
                this->lastWidth = width;
                this->currentWidth = width;

                this->height = height;
                this->lastHeight = height;
                this->currentHeight = height;

                this->firstUpdate = false;
            }

            // Set correct style
            // TODO: FIX THIS, GET YOUR POSITIONS RIGHT.
            if (this->pressed) {
                this->x = parent->getX() + this->paddingX - 5;
                this->width = parent->getWidth() - this->paddingX * 2 + 10;
                this->height = height + 10;
            } else {
                if (this->selected) {
                    this->x = parent->getX() + this->paddingX;
                    this->width = parent->getWidth() - this->paddingX * 2;
                } else {
                    this->x = this->originalX;
                    this->width = width + (this->iconWidth > 0 ? this->iconWidth + (this->paddingX * 2) : 0);
                }
                this->height = height;
            }

            this->y = parent->getY() + this->originalY - (this->selected ? 5 : 0);

            // Check if button is pressed
            if (this->pressed && !this->lastPressed) {
                if (this->pressedCallback != NULL) this->pressedCallback();
            }

            // Check if properties changed
            if (this->x != this->lastX) {
                this->startedAnimatingX = millis();
                this->lastX = this->x;
            }

            if (this->y != this->lastY) {
                this->startedAnimatingY = millis();
                this->lastY = this->y;
            }

            if (this->width != this->lastWidth) {
                this->startedAnimatingWidth = millis();
                this->lastWidth = this->width;
            }

            if (this->height != this->lastHeight) {
                this->startedAnimatingHeight = millis();
                this->lastHeight = this->height;
            }

            // Animate
            int time = millis();
            if (this->currentX != this->x) this->currentX += (Ease::Cubic::Out(((float)time - (float)this->startedAnimatingX) / 1000.) * (this->x - this->currentX));
            if (this->currentY != this->y) this->currentY += (Ease::Cubic::Out(((float)time - (float)this->startedAnimatingY) / 1000.) * (this->y - this->currentY));
            if (this->currentWidth != this->width) this->currentWidth += (Ease::Cubic::Out(((float)time - (float)this->startedAnimatingWidth) / 1000.) * (this->width - this->currentWidth));
            if (this->currentHeight != this->height) this->currentHeight += (Ease::Cubic::Out(((float)time - (float)this->startedAnimatingHeight) / 1000.) * (this->height - this->currentHeight));
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            // Draw button
            if (this->pressed) {
                display->setTextColor(this->color2);
                display->fillRoundRect(this->currentX, this->currentY, this->currentWidth, this->currentHeight, this->roundness, this->color1);
                if (this->selected) display->drawBitmap(this->currentX + this->currentWidth - arrow_right_width - paddingX, this->currentY + (this->currentHeight / 2) - (arrow_right_height / 2), arrow_right, arrow_right_width, arrow_right_height, this->color2);
            } else {
                display->setTextColor(this->color1);
                display->drawRoundRect(this->currentX, this->currentY, this->currentWidth, this->currentHeight, this->roundness, this->color1);
                if (this->selected) {
                    display->drawLine(this->currentX, this->currentY + this->currentHeight - 2, this->currentX + this->currentWidth - 2, this->currentY + currentHeight - 2, this->color1);
                    display->drawBitmap(this->currentX + this->currentWidth - arrow_right_width - paddingX, this->currentY + (this->currentHeight / 2) - (arrow_right_height / 2), arrow_right, arrow_right_width, arrow_right_height, this->color1);
                }
            }

            // Draw icon
            if (this->iconWidth > 0) display->drawBitmap(this->currentX + this->paddingX, this->currentY + (this->currentHeight / 2) - (this->iconHeight / 2), this->icon, this->iconWidth, this->iconHeight, (this->pressed ? this->color2 : this->color1));

            // Draw text
            display->setTextSize(this->textSize);
            display->setCursor(this->currentX + this->paddingX + (this->iconWidth > 0 ? this->paddingX + this->iconWidth : 0), this->currentY + this->paddingY + (this->pressed ? 5 : 0));
            display->print(this->text);
        }

};

#endif