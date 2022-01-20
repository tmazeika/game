#include "game.h"
#include "platform.h"

#include <cmath>
#include <cstdio>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

struct GameState {
    int tileMapX = 0;
    int tileMapY = 0;
    float playerX = 100;
    float playerY = 100;

    int tileMap00[9][16] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };
    int tileMap10[9][16] = {
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };
    int tileMap01[9][16] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    };
    int tileMap11[9][16] = {
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };
    int* currentTileMap;
    int* tileMaps[2][2];

    bool recording = false;
    bool playingBack = false;
    File playbackFile = nullptr;
};

float tileWidth = 80.0f;
float tileHeight = 90.0f;

bool isPointEscaped(GameState* gs, float x, float y) {
    int tileX = (int) (x / tileWidth);
    int tileY = (int) (y / tileHeight);
    return tileX >= 16 || tileY >= 9 || tileX < 0 || tileY < 0;
}

bool isPointInTile(GameState* gs, float x, float y) {
    int tileX = (int) (x / tileWidth);
    int tileY = (int) (y / tileHeight);
    return isPointEscaped(gs, x, y) ||
           gs->currentTileMap[tileY * 16 + tileX];
}

void initGameState(void* pGameState) {
    *(GameState*) pGameState = GameState{};
    auto gs = (GameState*) pGameState;
    gs->currentTileMap = (int*) gs->tileMap00;
    gs->tileMaps[0][0] = (int*) gs->tileMap00;
    gs->tileMaps[1][0] = (int*) gs->tileMap10;
    gs->tileMaps[0][1] = (int*) gs->tileMap01;
    gs->tileMaps[1][1] = (int*) gs->tileMap11;
}

bool update(void* pGameState, Input input) {
    auto gs = (GameState*) pGameState;

    if (input.closeRequested) {
        return false;
    }
    if (wasEverNewlyDown(input.keyL) && !gs->playingBack) {
        if (gs->recording) {
            printf("Recording stopped. Starting infinite playback...\n");
            closeFile(gs->playbackFile);
            readEntireFile("gs.g", MAX_GAME_STATE_SIZE, gs);
            gs->recording = false;
            gs->playingBack = true;
            gs->playbackFile = openFileForReading("inputs.g");
        } else {
            printf("Recording started.\n");
            writeEntireFile("gs.g", MAX_GAME_STATE_SIZE, gs);
            gs->recording = true;
            gs->playingBack = false;
            gs->playbackFile = openFileForWriting("inputs.g");
        }
    }
    if (gs->recording) {
        writeNextToFile(gs->playbackFile, sizeof(input), &input);
    } else if (gs->playingBack) {
        if (!readNextFromFile(gs->playbackFile, sizeof(input), &input)) {
            closeFile(gs->playbackFile);
            readEntireFile("gs.g", MAX_GAME_STATE_SIZE, gs);
            gs->recording = false;
            gs->playingBack = true;
            gs->playbackFile = openFileForReading("inputs.g");
            readNextFromFile(gs->playbackFile, sizeof(input), &input);
        }
    }

    const float playerSpeed = 3.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    if (wasEverDown(input.keyW)) {
        dy -= playerSpeed;
    }
    if (wasEverDown(input.keyA)) {
        dx -= playerSpeed;
    }
    if (wasEverDown(input.keyS)) {
        dy += playerSpeed;
    }
    if (wasEverDown(input.keyD)) {
        dx += playerSpeed;
    }

    float newX = gs->playerX + dx;
    float newY = gs->playerY + dy;

    if (isPointInTile(gs, newX, gs->playerY)) {
       dx = 0.0f;
    }
    if (isPointInTile(gs, gs->playerX, newY)) {
        dy = 0.0f;
    }

    float tileMapHeight = 8.0f * tileHeight;
    float tileMapWidth = 15.0f * tileWidth;

    if (newX < tileWidth / 2.0f) {
        if (gs->tileMapX > 0) {
            gs->tileMapX--;
            dx += tileMapWidth;
        }
    }
    if (newX > tileMapWidth + tileWidth / 2.0f) {
        if (gs->tileMapX < 1) {
            gs->tileMapX++;
            dx -= tileMapWidth;
        }
    }
    if (newY < 0) {
        if (gs->tileMapY > 0) {
            gs->tileMapY--;
            dy += tileMapHeight;
        }
    }
    if (newY > tileMapHeight + tileHeight / 2.0f) {
        if (gs->tileMapY < 1) {
            gs->tileMapY++;
            dy -= tileMapHeight;
        }
    }
    gs->currentTileMap = (int*) gs->tileMaps[gs->tileMapY][gs->tileMapX];
    gs->playerX += dx;
    gs->playerY += dy;

    return true;
}

void
drawRect(Window window, int x, int y, int width, int height, float r, float g,
    float b) {
    Pixel pixel{};
    pixel.red = (uint8_t) (r * 255.0f);
    pixel.green = (uint8_t) (g * 255.0f);
    pixel.blue = (uint8_t) (b * 255.0f);

    int x1 = max(0, x), y1 = max(0, y);
    int x2 = min(window.width, x + width), y2 = min(window.height, y + height);
    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++) {
            setPixel(window, x, y, pixel);
        }
    }
}

void render(void* pGameState, Window window, float t) {
    const GameState gs = *(GameState*) pGameState;
    size_t pixelCount = window.width * window.height;
    for (size_t i = 0; i < pixelCount; i++) {
        window.pixels[i] = {.rgb = 0};
    }

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 9; y++) {
            if (gs.currentTileMap[y * 16 + x]) {
                drawRect(window, x * tileWidth, y * tileHeight, tileWidth,
                    tileHeight, 0.3f, 0.3f, 0.4f);
            }
        }
    }
    drawRect(window, (int) gs.playerX - 25, (int) gs.playerY - 80,
        50, 80, 0.1f, 0.5f, 0.8f);
    drawRect(window, (int) gs.playerX - 3, (int) gs.playerY - 3,
        6, 6, 1.0f, 0.0f, 0.0f);
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
