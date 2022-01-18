#include "game.h"
#include "platform.h"

#include <cmath>
#include <cstdio>

struct GameState {
    bool btn0 = false;
    bool btn1 = false;
    bool btn2 = false;

    int xOffset = 0;

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

    gameState->btn0 = input.btnLeft.currentlyDown;
    gameState->btn1 = input.btnMiddle.currentlyDown;
    gameState->btn2 = input.btnRight.currentlyDown;
    gameState->xOffset = input.mouseX;
    return true;
}

void drawRect(Window window, int x, int y, int width, int height) {
    const int x2 = x + width, y2 = y + height;
    for (int x1 = x; x1 < x2; x1++) {
        for (int y1 = y; y1 < y2; y1++) {
            setPixel(window, x1, y1, {.rgb = 0xffffffff});
        }
    }
}

void render(void* pGameState, Window window, float t) {
    const GameState gameState = *(GameState*) pGameState;
    auto row = (uint8_t*) window.pixels;
    for (int y = 0; y < window.height; y++) {
        auto pixel = (int*) row;
        for (int x = 0; x < window.width; x++) {
            const uint8_t blue = x - gameState.xOffset;
            const uint8_t green = y;
            *pixel++ = ((green << 8) | blue);
        }
        row += window.width * sizeof(Pixel);
    }
    if (gameState.btn0) {
        drawRect(window, 10, 10, 10, 10);
    }
    if (gameState.btn1) {
        drawRect(window, 30, 10, 10, 10);
    }
    if (gameState.btn2) {
        drawRect(window, 50, 10, 10, 10);
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
