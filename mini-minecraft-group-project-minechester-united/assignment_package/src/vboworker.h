#pragma once
#include "scene/chunk.h"

class VBOWorker {
private:
    Chunk* m_chunk;

public:
    VBOWorker(Chunk* chunk)
        : m_chunk(chunk) {}

    void operator()() {
        if (!m_chunk->hasVBOData()) m_chunk->createVBOdata();
    }
};
