#include "../platform.h"

#include <cassert>
#include <ctime>

double getTime() {
    timespec time{};
    assert(clock_gettime(CLOCK_MONOTONIC_RAW, &time) == 0);
    return (double) time.tv_sec + (double) time.tv_nsec / 1000000000.0;
}
