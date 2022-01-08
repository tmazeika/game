#include "game.h"

#include <cmath>

struct GameState {
    uint32_t xOffset = 0;
};

void initGameState(void* gameStatePtr) {
    *(GameState*) gameStatePtr = GameState{};
}

bool update(void* gameStatePtr, Input input) {
    auto gameState = (GameState*) gameStatePtr;
    gameState->xOffset = input.mouseX;
    return !input.closeRequested;
}

void render(void* gameStatePtr, uint32_t width, uint32_t height, Pixel pixels[],
        float t) {
    auto gameState = (GameState*) gameStatePtr;
    auto* row = (uint8_t*) pixels;
    for (uint32_t y = 0; y < height; y++) {
        auto* pixel = (uint32_t*) row;
        for (uint32_t x = 0; x < width; x++) {
            uint8_t blue = x - gameState->xOffset;
            uint8_t green = y;
            *pixel++ = ((green << 8) | blue);
        }
        row += width * sizeof(Pixel);
    }
}

void writeSound(void* gameStatePtr, size_t sampleCount, SoundSample samples[]) {
//    auto gameState = (GameState*) gameStatePtr;
    static float t = 0.0f;

    const float pi2 = 2.0f * 3.1416f;
    const float maxVolume = 0x7fff;
    const float hz = 261.6255653005986f;
    const float period = 44100.0f / hz;

    for (size_t i = 1; i < sampleCount; i += 2) {
        samples[i - 1] = samples[i] = (SoundSample) (
                sinf(t) * (maxVolume / 15.0f)
        );
        t += pi2 / period;
        if (t >= pi2) {
            t -= pi2;
        }
    }
}
