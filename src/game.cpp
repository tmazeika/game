#include "game.h"
#include "platform.h"

#include <cmath>
#include <cstdio>

struct GameState {
    uint32_t xOffset = 0;

    bool recording = false;
    bool playingBack = false;
    File playbackFile = nullptr;
};

void initGameState(void* pGameState) {
    *(GameState*) pGameState = GameState{};
}

bool update(void* pGameState, Input input) {
    auto gameState = (GameState*) pGameState;

    if (input.closeRequested) {
        return false;
    }
    if (wasEverNewlyDown(input.keyL) && !gameState->playingBack) {
        if (gameState->recording) {
            printf("Recording stopped. Starting infinite playback...\n");
            closeFile(gameState->playbackFile);
            readEntireFile("gameState.g", MAX_GAME_STATE_SIZE, gameState);
            gameState->recording = false;
            gameState->playingBack = true;
            gameState->playbackFile = openFileForReading("inputs.g");
        } else {
            printf("Recording started.\n");
            writeEntireFile("gameState.g", MAX_GAME_STATE_SIZE, gameState);
            gameState->recording = true;
            gameState->playingBack = false;
            gameState->playbackFile = openFileForWriting("inputs.g");
        }
    }
    if (gameState->recording) {
        writeNextToFile(gameState->playbackFile, sizeof(input), &input);
    } else if (gameState->playingBack) {
        if (!readNextFromFile(gameState->playbackFile, sizeof(input), &input)) {
            closeFile(gameState->playbackFile);
            readEntireFile("gameState.g", MAX_GAME_STATE_SIZE, gameState);
            gameState->recording = false;
            gameState->playingBack = true;
            gameState->playbackFile = openFileForReading("inputs.g");
            readNextFromFile(gameState->playbackFile, sizeof(input), &input);
        }
    }

    gameState->xOffset = input.mouseX;
    return true;
}

void render(void* pGameState, uint32_t width, uint32_t height, Pixel pixels[],
        float t) {
    auto gameState = (GameState*) pGameState;
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

void writeSound(void* pGameState, size_t sampleCount, SoundSample samples[]) {
//    auto gameState = (GameState*) gameStatePtr;
    static float t = 0.0f;

    const float pi2 = 2.0f * 3.1416f;
    const float maxVolume = 0; // Muted for now...
//    const float maxVolume = 0x7fff;
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
