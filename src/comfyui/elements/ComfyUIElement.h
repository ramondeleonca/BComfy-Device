#ifndef COMFYUI_ELEMENT_H
#define COMFYUI_ELEMENT_H

#include <Adafruit_SSD1306.h>
#include "comfyui/enums.h"
#include <LinkedList.h>
#include "comfyui/types.h"

class ComfyUIElement {
    private:
        ComfyUIElement* parent = NULL;
        

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

        /* Gets the element's parent */
        ComfyUIElement* getParent() {
            return this->parent;
        };

        void setParent(ComfyUIElement* parent) {
            this->parent = parent;
        };

        /* Returns bounding box of element*/
        comfy_bounding_box getBoundingBox() {
            comfy_bounding_box box;
            box.x = this->getX();
            box.y = this->getY();
            box.width = this->getWidth();
            box.height = this->getHeight();
            return box;
        }
};

#endif