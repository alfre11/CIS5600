#pragma once
#include "scene/chunk.h"
#include <mutex>
#include <iostream>

class VBOWorker {
private:
    Chunk* m_chunk;

public:
    VBOWorker(Chunk* chunk)
        : m_chunk(chunk) {}

    void operator()() {
        // m_chunk->createVBOdata();
        if(!m_chunk->m_hasTransparentVBOData) m_chunk->createTransparentVBOdata();

    }
};
