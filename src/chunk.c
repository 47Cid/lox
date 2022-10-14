#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "vm.h"

void initChunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    chunk->lineCount = 0;
    chunk->lineCapacity = 0;
    chunk->lines = NULL;

    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    if (chunk->lineCapacity < chunk->lineCount + 1) {
        int oldCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldCapacity);

        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->lineCapacity);

        if (oldCapacity == 0)
            chunk->lines[0] = 0;
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;

    if (line == chunk->lineCount + 1) {
        chunk->lines[chunk->lineCount]++;
    } else {
        chunk->lineCount++;
        chunk->lines[chunk->lineCount] = 1;
    }
}

int getLine(Chunk *chunk, int offset) {
    int totalSum = 0;

    for (int i = 0; i <= chunk->lineCount; i++) {
        totalSum += chunk->lines[i];

        if (offset < totalSum) {
            return i + 1;
        }
    }

    return -1;
}

int addConstant(Chunk *chunk, Value value) {
    push(value);
    writeValueArray(&chunk->constants, value);
    pop();
    return chunk->constants.count - 1;
}
