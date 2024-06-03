#ifndef COMFYUI_BUTTON_LIST_H
#define COMFYUI_BUTTON_LIST_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>
#include <comfyui/elements/ComfyUIButton.h>
#include <comfyui/elements/ComfyUIFrame.h>

class ComfyUIButtonList : public ComfyUIFrame {
    private:
        int selectedIndex = 0;
        int lastSelectedIndex = 0;

    public:
        ComfyUIButtonList(int x = 0, int y = 0, int width = 128, int height = 64) : ComfyUIFrame(x, y, width, height) {}

        void addChild(ComfyUIButton* button) {
            button->setParent(this);
            this->children.add(button);
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            for (int i = 0; i < children.size(); i++) {
                ComfyUIButton* button = (ComfyUIButton*) children.get(i);
                button->update(display, this);
                button->draw(display, this);
            }
        }

        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            for (int i = 0; i < children.size(); i++) {
                ComfyUIButton* button = (ComfyUIButton*) children.get(i);
                button->setSelected(i == this->selectedIndex);
            }

            this->y = -20 * this->selectedIndex;
        }

        int getSelectedIndex() {
            return this->selectedIndex;
        }

        int getLastSelectedIndex() {
            return this->lastSelectedIndex;
        }

        void setSelectedIndex(int selectedIndex) {
            this->lastSelectedIndex = this->selectedIndex;
            this->selectedIndex = selectedIndex;
        }

        int getButtons() {
            return this->children.size();
        }

        ComfyUIButton* getSelectedButton() {
            return (ComfyUIButton*) this->children.get(this->selectedIndex);
        }

        void selectNext() {
            this->lastSelectedIndex = this->selectedIndex;
            this->selectedIndex++;
            if (this->selectedIndex >= this->children.size()) this->selectedIndex = 0;
        }

        void selectPrevious() {
            this->lastSelectedIndex = this->selectedIndex;
            this->selectedIndex--;
            if (this->selectedIndex < 0) this->selectedIndex = this->children.size() - 1;
        }

};

#endif