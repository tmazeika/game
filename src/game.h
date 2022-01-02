#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <cstdint>

union Pixel {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };
    uint32_t rgb;
};

typedef int16_t Sample;

void render(uint32_t width, uint32_t height, Pixel pixels[], float delta);

void writeSound(size_t sampleCount, Sample samples[]);

#endif
