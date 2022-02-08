#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

#include <cstddef>

const size_t MAX_GAME_STATE_SIZE = (1LL << 20) * 16; // 16 MiB.

/**
 * Gets the current time in seconds. The returned value means nothing on its
 * own and only has meaning when it is compared to other values returned by this
 * function.
 */
double getTime();

typedef void* File;

File openFileForReading(const char* filename);

File openFileForWriting(const char* filename);

bool readNextFromFile(File file, size_t bytes, void* dest);

void writeNextToFile(File file, size_t bytes, void* src);

void closeFile(File file);

void readEntireFile(const char* filename, size_t maxBytes, void* dataDest);

void writeEntireFile(const char* filename, size_t bytes, void* dataSrc);

#endif
