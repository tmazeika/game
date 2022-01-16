#include "input.h"

void onDown(InputDown* inputDown, bool down) {
    if (down != inputDown->currentlyDown) {
        inputDown->currentlyDown = down;
        inputDown->transitions++;
    }
}

bool wasPreviouslyUp(InputDown inputDown) {
    return (inputDown.currentlyDown && inputDown.transitions % 2 == 1) ||
           (!inputDown.currentlyDown && inputDown.transitions % 2 == 0);
}

bool wasPreviouslyDown(InputDown inputDown) {
    return (inputDown.currentlyDown && inputDown.transitions % 2 == 0) ||
           (!inputDown.currentlyDown && inputDown.transitions % 2 == 1);
}

bool wasAlwaysUp(InputDown inputDown) {
    return !inputDown.currentlyDown && inputDown.transitions == 0;
}

bool wasAlwaysDown(InputDown inputDown) {
    return inputDown.currentlyDown && inputDown.transitions == 0;
}

bool wasEverUp(InputDown inputDown) {
    return !inputDown.currentlyDown || inputDown.transitions > 0;
}

bool wasEverDown(InputDown inputDown) {
    return inputDown.currentlyDown || inputDown.transitions > 0;
}

bool wasEverNewlyUp(InputDown inputDown) {
    return wasPreviouslyDown(inputDown) && wasEverUp(inputDown);
}

bool wasEverNewlyDown(InputDown inputDown) {
    return wasPreviouslyUp(inputDown) && wasEverDown(inputDown);
}
