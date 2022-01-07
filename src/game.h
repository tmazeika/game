#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <cstddef>
#include <cstdint>

#include "graphics.h"
#include "input.h"
#include "sound.h"

const double S_PER_UPDATE = 1.0 / 60.0;

void initGameState(void* gameStatePtr);

bool update(void* gameStatePtr, Input input);

void render(void* gameStatePtr, uint32_t width, uint32_t height, Pixel pixels[],
        float delta);

void writeSound(size_t sampleCount, SoundSample samples[], void* gameState);

#endif
