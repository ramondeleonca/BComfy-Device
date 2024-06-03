#ifndef COMFYUI_EASINGS_H
#define COMFYUI_EASINGS_H

class Ease {
    public:
        class Cubic {
            public:
                static float Out(float t) {
                    t -= 1;
                    t = t * t * t + 1;
                    return t > 1 ? 1 : t;
                }
        };
};

#endif
