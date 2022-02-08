#include "game.h"
#include "platform.h"

#include <cmath>
#include <cstdio>
#include <cassert>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define pow2(x) ((x) * (x))

struct Position {
    uint64_t tileX;
    uint64_t tileY;
    float subTileX;
    float subTileY;
};

const size_t TILE_CHUNK_DIM = 16;

#define arithmeticModf(x, y) ((x) - (y) * floorf((x) / (y)))

void normalizePos(Position* pos) {
    pos->tileX += (int) floorf(pos->subTileX);
    pos->tileY += (int) floorf(pos->subTileY);
    pos->subTileX = arithmeticModf(pos->subTileX, 1.0f);
    pos->subTileY = arithmeticModf(pos->subTileY, 1.0f);
}

bool isValidPos(Position* pos) {
    int x = (int) floorf(pos->subTileX);
    int y = (int) floorf(pos->subTileY);
    bool ok;
    if (x < 0) {
        ok = pos->tileX >= -x;
    } else {
        ok = pos->tileX <= ((uint64_t) -1) - x;
    }
    if (y < 0) {
        ok = ok && pos->tileY >= -y;
    } else {
        ok = ok && pos->tileY <= ((uint64_t) -1) - y;
    }
    return ok;
}

typedef uint8_t Tile;

struct TileChunk {
    uint32_t x;
    uint32_t y;
    Tile tiles[TILE_CHUNK_DIM][TILE_CHUNK_DIM];
};

struct TileMap {
    size_t chunkCount;
    TileChunk* chunks;
};

bool testPosInTileMap(TileMap* map, Position* pos) {
    for (size_t i = 0; i < map->chunkCount; i++) {
        TileChunk* chunk = map->chunks + i;
        if ((chunk->x <= pos->tileX &&
             pos->tileX < chunk->x + TILE_CHUNK_DIM) &&
            (chunk->y <= pos->tileY &&
             pos->tileY < chunk->y + TILE_CHUNK_DIM)) {
            uint64_t tileX = pos->tileX - chunk->x;
            uint64_t tileY = pos->tileY - chunk->y;
            return chunk->tiles[tileY][tileX] == 1;
        }
    }
    return false;
}

struct MemoryRegion {
    size_t size;
    size_t used;
    uint8_t* base;
};

void* memoryPush_(MemoryRegion* mem, size_t bytes) {
    assert(mem->used + bytes <= mem->size);
    mem->used += bytes;
    void* base = mem->base;
    mem->base += bytes;
    return base;
}

#define memoryPush(mem, type) (type *) memoryPush_(mem, sizeof(type))

struct GameState {
    bool recording = false;
    bool playingBack = false;
    File playbackFile = nullptr;

    MemoryRegion heap;

    TileMap tileMap;
    Position playerPos;
};

const float pixelsPerMeter = 70.0f;
const float tileSize = 1.4f;

void initGameState(void* pGameState) {
    *(GameState*) pGameState = GameState{};
    auto gs = (GameState*) pGameState;
    gs->heap.size = MAX_GAME_STATE_SIZE - sizeof(GameState);
    gs->heap.base = (uint8_t*) pGameState + sizeof(GameState);
    gs->tileMap.chunks = (TileChunk*) gs->heap.base;
}

bool update(void* pGameState, Window window, Input input) {
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

    int8_t dx = 0;
    int8_t dy = 0;
    if (wasEverDown(input.keyW)) {
        dy += 1;
    }
    if (wasEverDown(input.keyA)) {
        dx -= 1;
    }
    if (wasEverDown(input.keyS)) {
        dy -= 1;
    }
    if (wasEverDown(input.keyD)) {
        dx += 1;
    }
    Position playerPos = gs->playerPos;
    if (dx != 0 || dy != 0) {
        float speed = 0.03f;
        float magnitude = sqrtf((float) (pow2(dx) + pow2(dy)));
        playerPos.subTileX += (float) dx / magnitude * speed;
        playerPos.subTileY += (float) dy / magnitude * speed;
    }
    if (isValidPos(&playerPos)) {
        normalizePos(&playerPos);
        if (!testPosInTileMap(&gs->tileMap, &playerPos)) {
            gs->playerPos = playerPos;
        }
    }
    return true;
}

void
drawRect(Window window, int x1, int y1, int x2, int y2, float r, float g,
    float b) {
    Pixel pixel{};
    pixel.red = (uint8_t) (r * 255.0f);
    pixel.green = (uint8_t) (g * 255.0f);
    pixel.blue = (uint8_t) (b * 255.0f);

    int minX = max(0, min(x1, x2));
    int minY = max(0, min(y1, y2));
    int maxX = min(window.width - 1, max(x1, x2));
    int maxY = min(window.height - 1, max(y1, y2));

    for (int y = maxY; y >= minY; y--) {
        for (int x = minX; x <= maxX; x++) {
            setPixel(window, x, y, pixel);
        }
    }
}

void render(void* pGameState, Window window, float t) {
    auto gs = *(GameState*) pGameState;

    // Clear screen.
    size_t pixelCount = window.width * window.height;
    for (size_t i = 0; i < pixelCount; i++) {
        window.pixels[i] = {.rgb = 0};
    }

    int tileX = (int) ((float) gs.playerPos.tileX * tileSize * pixelsPerMeter);
    int tileY = (int) ((float) gs.playerPos.tileY * tileSize * pixelsPerMeter);
    drawRect(window, tileX, tileY, tileX + (int) (tileSize * pixelsPerMeter), tileY + (int) (tileSize * pixelsPerMeter), 0.05f, 0.1f, 0.15f);

    int playerX = (int) (((float) gs.playerPos.tileX + gs.playerPos.subTileX) *
                         tileSize * pixelsPerMeter);
    int playerY = (int) (((float) gs.playerPos.tileY + gs.playerPos.subTileY) *
                         tileSize * pixelsPerMeter);
    drawRect(window, playerX - 15, playerY - 15, playerX + 15, playerY + 15,
        1.0f, 0.0f, 0.0f);
    drawRect(window, playerX - 5, playerY - 5, playerX + 5, playerY + 5,
        1.0f, 1.0f, 0.0f);
}

void writeSound(void* pGameState, size_t sampleCount, SoundSample samples[]) {
//    auto gameState = (GameState*) gameStatePtr;
    static float t = 0.0f;

    float pi2 = 2.0f * 3.1416f;
    float maxVolume = 0; // Muted for now...
//    float maxVolume = 0x7fff;
    float hz = 261.6255653005986f;
    float period = 44100.0f / hz;

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
