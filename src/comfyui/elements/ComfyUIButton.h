#ifndef COMFYUI_BUTTON_H
#define COMFYUI_BUTTON_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <comfyui/elements/ComfyUIElement.h>
#include <comfyui/types.h>

// class ComfyUIButton : public ComfyUIElement {
//     public:
//         int x;
//         int y;
//         int originalX;
//         int originalY;
//         int width;
//         int height;
//         String text;
//         int textSize;
//         int color1;
//         int color2;
//         int paddingX;
//         int paddingY;
//         int roundness;
//         bool selected;
//         bool pressed;

//         ComfyUIButton(int x, int y, String text, int textSize = 1, int color1 = SSD1306_WHITE, int color2 = SSD1306_BLACK, int paddingX = 10, int paddingY = 3, int roundness = 3) {
//             this->x = x + paddingX;
//             this->y = y + paddingY;
//             this->originalX = x;
//             this->originalY = y;
//             this->text = text;
//             this->textSize = textSize;
//             this->color1 = color1;
//             this->color2 = color2;
//             this->paddingX = paddingX;
//             this->paddingY = paddingY;
//             this->roundness = roundness;
//             this->selected = false;
//             this->pressed = false;
//         }

//         bool getSelected() {
//             return this->selected;
//         }

//         void setSelected(bool selected) {
//             this->selected = selected;
//         }

//         bool getPressed() {
//             return this->pressed;
//         }

//         void setPressed(bool pressed) {
//             this->pressed = pressed;
//         }

//         // TODO: PLEASE CHANGE THIS IT WORKS TERRIBLY.
//         void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
//             int16_t x1, y1;
//             uint16_t w, h;
//             display->getTextBounds(this->text, this->x, this->y, &x1, &y1, &w, &h);
            
//             if (this->selected) {
//                 this->width = parent->getWidth() - (this->paddingX * 2);
//                 this->x = parent->getX() + this->paddingX;
//                 this->y = originalY;
//             } else {
//                 this->width = w + this->paddingX * 2;
//                 this->x = originalX + this->paddingX;
//                 this->y = originalY + this->paddingY;
//             }

//             this->height = h + this->paddingY * 2;
//         }

//         void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
//             display->drawRoundRect(this->x, this->y, this->width, this->height, this->roundness, this->color1);
//             display->setTextSize(this->textSize);
//             display->setTextColor(this->color2);
//             display->setCursor(this->x + this->paddingX, this->y + this->paddingY);
//             display->println(this->text);
//         }

//         int getWidth() {
//             return this->width;
//         }

//         int getHeight() {
//             return this->height;
//         }

//         int getX() {
//             return this->x;
//         }

//         int getY() {
//             return this->y;
//         }
// };

class ComfyUIButton : public ComfyUIElement {
    public:
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

        void update(Adafruit_SSD1306* dispaly, ComfyUIElement* parent = NULL) {}

        int getX() {
            return this->x;
        }

        int getY() {
            return this->y;
        }

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

#endif