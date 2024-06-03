#ifndef COMFYUI_TABS_H
#define COMFYUI_TABS_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>
#include <comfyui/elements/ComfyUIFrame.h>
#include <comfyui/elements/ComfyUIButtonList.h>
#include <comfyui/types.h>
#include <comfyui/easings.h>

struct comfyui_tab {
    String name;
    ComfyUIFrame* frame;
};

class ComfyUITabs : public ComfyUIFrame {
    private:
        ComfyUIButtonList* tabsList;
        LinkedList<comfyui_tab> tabs;
        String currentTab = "";

    public:
        ComfyUITabs(int x = 0, int y = 0, int width = 128, int height = 64) : ComfyUIFrame(x, y, width, height) {}

        void addTab(String name, ComfyUIFrame* frame) {
            comfyui_tab tab = {name, frame};
            this->tabs.add(tab);
        }

        void setTabsList(ComfyUIButtonList* tabsList) {
            this->tabsList = tabsList;
        }

        void setTab(String name = "") {
            this->currentTab = name;
        }

        void update(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            
        }

        void draw(Adafruit_SSD1306* display, ComfyUIElement* parent = NULL) {
            if (this->currentTab == "") {
                this->tabsList->update(display, this);
                this->tabsList->draw(display, this);
            } else {
                Serial.println(this->currentTab);
                for (int i = 0; i < this->tabs.size(); i++) {
                    comfyui_tab tab = this->tabs.get(i);
                    if (tab.name == this->currentTab) {
                        tab.frame->update(display, this);
                        tab.frame->draw(display, this);
                    }
                }
            }
        }
};

#endif