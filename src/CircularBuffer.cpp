/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry)
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(uint16_t size) : readerPosition(0), writerPosition(0), bufferSize(size), buffer(new uint8_t[size]) {}

CircularBuffer::~CircularBuffer() {
    delete[] buffer;
}

uint16_t CircularBuffer::nextPosition(position_ptr_t positionPtr) {
    bool successfullyUpdated = false;
    position_t existing;
    while(!successfullyUpdated) {
        existing = readAtomic(positionPtr);
        position_t newPos = existing + 1;
        if (newPos >= bufferSize) {
            newPos = 0;
        }
        successfullyUpdated = casAtomic(positionPtr, existing, newPos);
    }
    return existing;
}
