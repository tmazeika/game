#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <cstdint>

struct Input {
    bool closeRequested;

    uint32_t mouseX;
    uint32_t mouseY;

    bool leftBtnDown;
    uint32_t leftBtnTransitions;
};

#endif
