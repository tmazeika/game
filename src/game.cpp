#include "game.h"
#include <cmath>

void render(uint32_t width, uint32_t height, Pixel pixels[], int64_t delta) {
    static const auto startTime = std::chrono::high_resolution_clock::now();

    const auto nowTime = std::chrono::high_resolution_clock::now();
    const auto deltaTime = nowTime - startTime;
    const auto diffSeconds =
            (float) std::chrono::duration_cast<std::chrono::nanoseconds>(
                    deltaTime).count() / 1000000000.0f;

    auto* row = (uint8_t*) pixels;
    for (uint32_t y = 0; y < height; y++) {
        auto* pixel = (uint32_t*) row;
        for (uint32_t x = 0; x < width; x++) {
            uint8_t blue = x + (uint32_t) (diffSeconds * 100);
            uint8_t green = y;
            *pixel++ = ((green << 8) | blue);
        }
        row += width * sizeof(Pixel);
    }
}

void writeSound(size_t sampleCount, Sample samples[]) {
    static uint64_t counter = 0;

    const float pi = 3.1416f;
    const float maxVolume = 0x7fff;
    const float hz = 261.6255653005986f;
    const float period = 44100.0f / hz;

    for (size_t i = 1; i < sampleCount; i += 2, counter++) {
        const float t = fmodf((float) counter, period) / period;
        samples[i - 1] = samples[i] = (Sample) (
                sinf(t * 2.0f * pi) * (maxVolume / 2.0f)
        );
    }
}
