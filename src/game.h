#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <cstddef>
#include <cstdint>

#include "graphics.h"
#include "input.h"
#include "sound.h"

#ifdef __GNUC__
#define EXPORT extern "C"
#else
// TODO(tmazeika): Support other compilers.
#endif

#define INIT_GAME_STATE(name) void name(void* gameState)

typedef INIT_GAME_STATE((*InitGameState));

#define UPDATE(name) bool name(void* gameState, Input input)

typedef UPDATE((*Update));

UPDATE(updateStub) {
    return true;
}

#define RENDER(name) void name(void* gameState, uint32_t width, \
    uint32_t height, Pixel pixels[], float t)

typedef RENDER((*Render));

RENDER(renderStub) {
    //
}

#define WRITE_SOUND(name) void name(void* gameState, size_t sampleCount, \
    SoundSample samples[])

typedef WRITE_SOUND((*WriteSound));

WRITE_SOUND(writeSoundStub) {
    for (size_t i = 0; i < sampleCount; i++) {
        samples[0] = 0;
    }
}

EXPORT INIT_GAME_STATE(initGameState);

EXPORT UPDATE(update);

EXPORT RENDER(render);

EXPORT WRITE_SOUND(writeSound);

#endif
