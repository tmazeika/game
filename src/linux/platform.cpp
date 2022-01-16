#include "../platform.h"

#include <cassert>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>

double getTime() {
    timespec time{};
    assert(clock_gettime(CLOCK_MONOTONIC_RAW, &time) == 0);
    return (double) time.tv_sec + (double) time.tv_nsec / 1000000000.0;
}

File openFileForReading(const char* filename) {
    FILE* file = fopen(filename, "r");
    assert(file);
    return (File) file;
}

File openFileForWriting(const char* filename) {
    FILE* file = fopen(filename, "w");
    assert(file);
    return (File) file;
}

bool readNextFromFile(File file, size_t bytes, void *dest) {
    const size_t items = fread(dest, bytes, 1, (FILE*) file);
    assert(ferror((FILE*) file) == 0);
    return items == 1;
}

void writeNextToFile(File file, size_t bytes, void *src) {
    fwrite(src, bytes, 1, (FILE*) file);
    assert(ferror((FILE*) file) == 0);
}

void closeFile(File file) {
    assert(fclose((FILE*) file) == 0);
}

void readEntireFile(const char* filename, size_t maxBytes, void *dataDest) {
    FILE* file = fopen(filename, "r");
    assert(file);
    struct stat info{};
    assert(stat(filename, &info) == 0);
    assert((size_t) info.st_size <= maxBytes);
    const size_t blocks = info.st_blocks;
    fread(dataDest, 512, blocks, file);
    assert(ferror(file) == 0);
    assert(fclose(file) == 0);
}

void writeEntireFile(const char* filename, size_t bytes, void* dataSrc) {
    FILE *file = fopen(filename, "w");
    assert(file);
    fwrite(dataSrc, bytes, 1, file);
    assert(ferror(file) == 0);
    assert(fclose(file) == 0);
}
