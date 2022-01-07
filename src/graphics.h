#ifndef GAME_GRAPHICS_H
#define GAME_GRAPHICS_H

#include <cstdint>

union Pixel {
    struct {
        // little endian only
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };
    uint32_t rgb;
};

#endif
