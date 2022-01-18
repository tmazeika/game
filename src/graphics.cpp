#include "graphics.h"

#include <cassert>
#include <cstddef>

void setPixel(Window window, int x, int y, Pixel pixel) {
    const size_t i = y * window.width + x;
    assert(i < (size_t) (window.width * window.height));
    window.pixels[i] = pixel;
}
