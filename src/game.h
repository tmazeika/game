#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <cstddef>
#include <cstdint>

#include "graphics.h"
#include "input.h"
#include "sound.h"

const double S_PER_UPDATE = 1.0 / 60.0;

void initGameState(void* gameState);

bool update(void* gameState, Input input);

void render(void* gameState, uint32_t width, uint32_t height, Pixel pixels[],
        float t);

void writeSound(void* gameState, size_t sampleCount, SoundSample samples[]);

#endif
