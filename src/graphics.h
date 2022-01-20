#ifndef GAME_GRAPHICS_H
#define GAME_GRAPHICS_H

#include <cstdint>

const double S_PER_UPDATE = 1.0 / 120.0;

union Pixel {
    struct {
        // Little endian only.
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };
    uint32_t rgb;
};

struct Window {
    Pixel* pixels;
    int width;
    int height;
};

void setPixel(Window window, int x, int y, Pixel pixel);

#endif
