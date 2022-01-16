#include "utils.h"

size_t getStringLength(const char* str) {
    for (size_t len = 0;; len++) {
        if (str[len] == '\0') {
            return len;
        }
    }
}
