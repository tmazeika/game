#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <cstdint>

struct InputDown {
    bool currentlyDown;
    uint32_t transitions;
};

void onDown(InputDown* inputDown, bool down);

bool wasPreviouslyUp(InputDown inputDown);

bool wasPreviouslyDown(InputDown inputDown);

bool wasAlwaysUp(InputDown inputDown);

bool wasAlwaysDown(InputDown inputDown);

bool wasEverUp(InputDown inputDown);

bool wasEverDown(InputDown inputDown);

bool wasEverNewlyUp(InputDown inputDown);

bool wasEverNewlyDown(InputDown inputDown);

struct Input {
    bool closeRequested;

    int mouseX;
    int mouseY;

    InputDown btnLeft;
    InputDown btnMiddle;
    InputDown btnRight;
    InputDown keyL;
    InputDown keyW;
    InputDown keyA;
    InputDown keyS;
    InputDown keyD;
};

#endif
