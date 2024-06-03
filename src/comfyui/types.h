#ifndef COMFYUI_TYPES_H
#define COMFYUI_TYPES_H

#include <Arduino.h>

struct comfy_bounding_box {
    int x;
    int y;
    int width;
    int height;
};

struct comfy_style {
    int x;
    int y;
    int width;
    int height;
    int paddingX;
    int paddingY;
    int marginX;
    int marginY;
};

#endif
