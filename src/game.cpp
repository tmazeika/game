#include "game.h"

const auto startTime = std::chrono::high_resolution_clock::now();

void render(uint32_t width, uint32_t height, Pixel pixels[], int64_t delta) {
    const auto nowTime = std::chrono::high_resolution_clock::now();
    const auto deltaTime = nowTime - startTime;
    const auto diffSeconds =
            (float) std::chrono::duration_cast<std::chrono::nanoseconds>(
                    deltaTime).count() / 1000000000.0f;

    auto* row = (uint8_t*) pixels;
    for (uint32_t y = 0; y < height; y++) {
        auto* pixel = (uint32_t*) row;
        for (uint32_t x = 0; x < width; x++) {
            uint8_t blue = x + (uint32_t)(diffSeconds * 100);
            uint8_t green = y;
            *pixel++ = ((green << 8) | blue);
        }
        row += width * sizeof(Pixel);
    }
}
