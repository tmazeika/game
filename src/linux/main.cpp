#include "pulseaudio.h"
#include "xcb.h"
#include "../game.h"
#include "../platform.h"

#include <cstdlib>
#include <dlfcn.h>
#include <unistd.h>

const char* GAME_LIB_SO = "build/linux_game_debug.so";
const double S_PER_HOT_RELOAD = 2.0;
const __useconds_t U_SLEEP = 1000 * 5; // 5ms

struct GameLib {
    void* lib;
    InitGameState initGameState;
    Update update;
    Render render;
    WriteSound writeSound;
};

GameLib loadGameLib() {
    void* lib = dlopen(GAME_LIB_SO, RTLD_NOW);
    return {
            .lib = lib,
            .initGameState = (InitGameState) dlsym(lib, "initGameState"),
            .update = (Update) dlsym(lib, "update"),
            .render = (Render) dlsym(lib, "render"),
            .writeSound = (WriteSound) dlsym(lib, "writeSound"),
    };
}

void unloadGameLib(GameLib* gameLib) {
    gameLib->update = &updateStub;
    gameLib->render = &renderStub;
    gameLib->writeSound = &writeSoundStub;
    dlclose(gameLib->lib);
}

int main() {
    void* gameState = malloc((1LL << 20) * 8); // 8 MiB
    GameLib gameLib = loadGameLib();

    gameLib.initGameState(gameState);
    PulseAudio* pa = initPulseAudio(&gameLib.writeSound, gameState);
    XCB* xcb = initXCB("gameLib");

    bool running = true;
    double prevTime = getTime();
    double lagTime = 0.0;
    double hotReloadTime = 0.0;

    while (running) {
        const double curTime = getTime();
        const double deltaTime = curTime - prevTime;
        prevTime = curTime;
        lagTime += deltaTime;
        hotReloadTime += deltaTime;

        if (hotReloadTime >= S_PER_HOT_RELOAD) {
            hotReloadTime -= S_PER_HOT_RELOAD;
            unloadGameLib(&gameLib);
            gameLib = loadGameLib();
        }

        const Input input = pollXCBEvents(xcb);
        const auto graphics = getXCBGraphicsInfo(xcb);

        // Update (fixed time step).
        while (running && lagTime >= S_PER_UPDATE) {
            lagTime -= S_PER_UPDATE;
            running = gameLib.update(gameState, input);
        }
        const auto tTime = (float) (lagTime / S_PER_UPDATE);

        // Render.
        gameLib.render(gameState, graphics.width, graphics.height,
                graphics.pixels, tTime);
        updateXCBGraphics(xcb);
        usleep(U_SLEEP);
    }

    destroyXCB(xcb);
    destroyPulseAudio(pa);
    unloadGameLib(&gameLib);
    free(gameState);
    return 0;
}
