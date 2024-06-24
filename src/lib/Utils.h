#ifndef UTILS_H
#define UTILS_H

class Utils {
    public:
        class Array {
            public:
                template <typename T>
                static int getRealIndex(const T* array, int size, int index) {
                    // Handle negative indices
                    if (index < 0) {
                        index = (index % size + size) % size;
                    } else {
                        index = index % size;
                    }
                    return index;
                }
        };
};

#endif