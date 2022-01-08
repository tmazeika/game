#ifndef GAME_PULSEAUDIO_H
#define GAME_PULSEAUDIO_H

#include "../sound.h"

#include <cstddef>

struct PulseAudio;

typedef void (* PulseAudioWriteCb)(void* userdata, size_t sampleCount, SoundSample samples[]);

PulseAudio* initPulseAudio(PulseAudioWriteCb writeCb, void* userdata);

void destroyPulseAudio(PulseAudio* pa);

#endif
