#include "pulseaudio.h"
#include "xcb.h"
#include "../game.h"
#include "../platform.h"

#include <cstdlib>

int main() {
    void* gameState = malloc(1024 * 1024 * 8); // 8 MiB
    initGameState(gameState);
    PulseAudio* pa = initPulseAudio(&writeSound, gameState);
    XCB* xcb = initXCB("game");

    bool running = true;
    double prevTime = getTime();
    double lagTime = 0.0;

    while (running) {
        const double curTime = getTime();
        const double deltaTime = curTime - prevTime;
        prevTime = curTime;
        lagTime += deltaTime;

        const Input input = pollXCBEvents(xcb);
        const auto graphics = getXCBGraphicsInfo(xcb);

        // Update (fixed time step).
        while (running && lagTime >= S_PER_UPDATE) {
            lagTime -= S_PER_UPDATE;
            running = update(gameState, input);
        }
        const auto tTime = (float) (lagTime / S_PER_UPDATE);

        // Render.
        render(gameState, graphics.width, graphics.height, graphics.pixels,
                tTime);
        updateXCBGraphics(xcb);
    }

    destroyXCB(xcb);
    destroyPulseAudio(pa);
    free(gameState);
    return 0;
}
