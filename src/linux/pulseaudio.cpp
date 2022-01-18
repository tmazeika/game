#include "pulseaudio.h"

#include <pulse/pulseaudio.h>

struct PulseAudio {
    void* userdata;
    PulseAudioWriteCb* writeCb;
    pa_threaded_mainloop* mainloop;
    pa_context* ctx;
};

void contextStateCb(pa_context*, void* mainloop) {
    pa_threaded_mainloop_signal((pa_threaded_mainloop*) mainloop, 0);
}

void streamStateCb(pa_stream*, void* mainloop) {
    pa_threaded_mainloop_signal((pa_threaded_mainloop*) mainloop, 0);
}

void streamWriteCb(pa_stream* stream, size_t fillSize, void* paPtr) {
    SoundSample* buf = nullptr;
    pa_stream_begin_write(stream, (void**) &buf, &fillSize);
    auto pa = (PulseAudio*) paPtr;
    (*pa->writeCb)(pa->userdata, fillSize / sizeof(SoundSample), buf);
    pa_stream_write(stream, buf, fillSize, nullptr, 0, PA_SEEK_RELATIVE);
}

PulseAudio* initPulseAudio(PulseAudioWriteCb* writeCb, void* userdata) {
    pa_threaded_mainloop* mainloop = pa_threaded_mainloop_new();
    pa_mainloop_api* mainloopApi = pa_threaded_mainloop_get_api(mainloop);
    pa_context* ctx = pa_context_new(mainloopApi, "game");
    const auto pa = new PulseAudio{
        .userdata = userdata,
        .writeCb = writeCb,
        .mainloop = mainloop,
        .ctx = ctx,
    };
    pa_context_set_state_callback(ctx, &contextStateCb, mainloop);
    pa_threaded_mainloop_lock(mainloop);
    pa_threaded_mainloop_start(mainloop);
    pa_context_connect(ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    while (pa_context_get_state(ctx) != PA_CONTEXT_READY) {
        pa_threaded_mainloop_wait(mainloop);
    }
    const pa_sample_spec sampleSpec = {
        .format = PA_SAMPLE_S16NE,
        .rate = SOUND_SAMPLE_RATE,
        .channels = 2
    };
    pa_stream* stream = pa_stream_new(ctx, "game-audio", &sampleSpec, nullptr);
    pa_stream_set_state_callback(stream, &streamStateCb, mainloop);
    pa_stream_set_write_callback(stream, &streamWriteCb, pa);
    const pa_buffer_attr playbackAttrs = {
        .maxlength = (uint32_t) -1,
        .tlength = (uint32_t) 30000,
        .prebuf = (uint32_t) -1,
        .minreq = (uint32_t) -1,
        .fragsize = (uint32_t) -1,
    };
    pa_stream_connect_playback(stream, nullptr, &playbackAttrs,
        PA_STREAM_ADJUST_LATENCY, nullptr, nullptr);
    while (pa_stream_get_state(stream) != PA_STREAM_READY) {
        pa_threaded_mainloop_wait(mainloop);
    }
    pa_threaded_mainloop_unlock(mainloop);
    return pa;
}

void destroyPulseAudio(PulseAudio* pa) {
    pa_context_disconnect(pa->ctx);
    pa_threaded_mainloop_stop(pa->mainloop);
    pa_threaded_mainloop_free(pa->mainloop);
    delete pa;
}
