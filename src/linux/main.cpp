#include "pulseaudio.h"
#include "xcb.h"
#include "../game.h"
#include "../platform.h"

#include <cassert>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>

const char* GAME_LIB_SO = "build/linux_game_debug.so";
const __useconds_t U_SLEEP = 1000 * 1; // 1ms

struct GameLib {
    void* lib;
    InitGameState initGameState;
    Update update;
    Render render;
    WriteSound writeSound;
};

GameLib loadGameLib() {
    void* lib = dlopen(GAME_LIB_SO, RTLD_NOW);
    if (const char* err = dlerror()) {
        fprintf(stderr, "Failed to load game library: %s\n", err);
    }
    assert(lib);
    return {
        .lib = lib,
        .initGameState = (InitGameState) dlsym(lib, "initGameState"),
        .update = (Update) dlsym(lib, "update"),
        .render = (Render) dlsym(lib, "render"),
        .writeSound = (WriteSound) dlsym(lib, "writeSound"),
    };
}

double getLastGameLibModTime() {
    struct stat res{};
    assert(stat(GAME_LIB_SO, &res) == 0);
    return (double) res.st_mtim.tv_sec +
           (double) res.st_mtim.tv_nsec / 1000000000.0;
}

void unloadGameLib(GameLib* gameLib) {
    gameLib->update = &updateStub;
    gameLib->render = &renderStub;
    gameLib->writeSound = &writeSoundStub;
    dlclose(gameLib->lib);
}

int main() {
    void* gameStateBaseAddr = (void*) ((1LL << 45) * 2); // Address at 64 TiB.
    void* gameState = mmap(gameStateBaseAddr, MAX_GAME_STATE_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0);
    assert(gameState == gameStateBaseAddr);

    double prevGameLibModTime = getLastGameLibModTime();
    GameLib gameLib = loadGameLib();

    gameLib.initGameState(gameState);
    PulseAudio* pa = initPulseAudio(&gameLib.writeSound, gameState);
    XCB* xcb = initXCB("gameLib");

    bool running = true;
    double prevTime = getTime();
    double lagTime = 0.0;

    while (running) {
        const double curTime = getTime();
        const double deltaTime = curTime - prevTime;
        prevTime = curTime;
        lagTime += deltaTime;

        double lastGameLibModTime = getLastGameLibModTime();
        if (lastGameLibModTime > prevGameLibModTime) {
            prevGameLibModTime = lastGameLibModTime;
            unloadGameLib(&gameLib);
            gameLib = loadGameLib();
        }

        // Update (fixed time step).
        while (running && lagTime >= S_PER_UPDATE) {
            lagTime -= S_PER_UPDATE;
            running = gameLib.update(gameState, pollXCBEvents(xcb));
        }

        // Render.
        const Window window = getXCBWindow(xcb);
        const auto tTime = (float) (lagTime / S_PER_UPDATE);
        gameLib.render(gameState, window, tTime);
        updateXCBGraphics(xcb);
        usleep(U_SLEEP);
    }

    destroyXCB(xcb);
    destroyPulseAudio(pa);
    unloadGameLib(&gameLib);
    munmap(gameState, MAX_GAME_STATE_SIZE);
    return 0;
}
