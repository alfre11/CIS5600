#pragma once

#include "chunk.h"
#include "QDir"
#include <iostream>

Chunk::Chunk(int x, int z, OpenGLContext* context) : Drawable(context), m_blocks(), minX(x), minZ(z),
    m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}},
    texture(context), newBlock(false)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getLocalBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getLocalBlockAt(int x, int y, int z) const {
    return getLocalBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setLocalBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
    m_hasVBOData = false;
}

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;

        m_hasVBOData = false;
        neighbor->m_hasVBOData = false;
    }
}

void Chunk::createVBOdata() {
    m_opaqueIndices.clear();
    m_opaqueVertexData.clear();
    m_transparentIndices.clear();
    m_transparentVertexData.clear();
    GLuint opindexOffset = 0;
    GLuint transindexOffset = 0;

    for (unsigned int x = 0; x < 16; ++x) {
        for (unsigned int y = 0; y < 256; ++y) {
            for (unsigned int z = 0; z < 16; ++z) {
                BlockType currentBlock = getLocalBlockAt(x, y, z);
                if (currentBlock == EMPTY) continue;

                glm::vec3 pos(x, y, z);
                glm::vec3 color;
                glm::vec3 uv;

                switchFunc(currentBlock, color, uv);

                if (currentBlock != WATER) {
                    if (getBlockAt(x - 1, y, z) == EMPTY || getBlockAt(x - 1, y, z) == WATER) {
                        if (currentBlock == GRASS) uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 4/16, (float) 14/16, 0);

                        addFace(pos, glm::vec3(-1, 0, 0), color, uv, false, opindexOffset); // XNEG
                    }
                    if (getBlockAt(x + 1, y, z) == EMPTY || getBlockAt(x + 1, y, z) == WATER) {
                        if (currentBlock == GRASS) uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 4/16, (float) 14/16, 0);

                        addFace(pos, glm::vec3(1, 0, 0), color, uv, false, opindexOffset);  // XPOS
                    }
                    if ((y != 0) && (getBlockAt(x, y - 1, z) == EMPTY || getBlockAt(x, y - 1, z) == WATER)) {
                        if (currentBlock == GRASS) uv = glm::vec3(2.f/16.f, 15.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 5/16, (float) 14/16, 0);

                        addFace(pos, glm::vec3(0, -1, 0), color, uv, false, opindexOffset); // YNEG
                    }
                    if (getBlockAt(x, y + 1, z) == EMPTY || getBlockAt(x, y + 1, z) == WATER) {
                        if (currentBlock == GRASS) uv = glm::vec3(8.f/16.f, 13.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 5/16, (float) 14/16, 0);

                        addFace(pos, glm::vec3(0, 1, 0), color, uv, false, opindexOffset);  // YPOS
                    }
                    if (getBlockAt(x, y, z - 1) == EMPTY || getBlockAt(x, y, z - 1) == WATER) {
                        if (currentBlock == GRASS) uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 4/16, (float) 14/16, 0);


                        addFace(pos, glm::vec3(0, 0, -1), color, uv, false, opindexOffset); // ZNEG
                    }
                    if (getBlockAt(x, y, z + 1) == EMPTY || getBlockAt(x, y, z + 1) == WATER) {
                        if (currentBlock == GRASS) uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        if (currentBlock == OAK_LOGS) uv = glm::vec3((float) 4/16, (float) 14/16, 0);

                        addFace(pos, glm::vec3(0, 0, 1), color, uv, false, opindexOffset);  // ZPOS
                    }
                } else {
                    if (getBlockAt(x - 1, y, z) == EMPTY) {
                        addFace(pos, glm::vec3(-1, 0, 0), color, uv, true, transindexOffset); // XNEG
                    }
                    if (getBlockAt(x + 1, y, z) == EMPTY) {
                        addFace(pos, glm::vec3(1, 0, 0), color, uv, true, transindexOffset);  // XPOS
                    }
                    if (y != 0 && getBlockAt(x, y - 1, z) == EMPTY) {
                        addFace(pos, glm::vec3(0, -1, 0), color, uv, true, transindexOffset); // YNEG
                    }
                    if (getBlockAt(x, y + 1, z) == EMPTY) {
                        addFace(pos, glm::vec3(0, 1, 0), color, uv, true, transindexOffset);  // YPOS
                    }
                    if (getBlockAt(x, y, z - 1) == EMPTY) {
                        addFace(pos, glm::vec3(0, 0, -1), color, uv, true, transindexOffset); // ZNEG
                    }
                    if (getBlockAt(x, y, z + 1) == EMPTY) {
                        addFace(pos, glm::vec3(0, 0, 1), color, uv, true, transindexOffset);  // ZPOS
                    }
                }
            }
        }
    }
    indexCounts[INDEX] = m_opaqueIndices.size();
    indexCounts[INDEXTRANSP] = m_transparentIndices.size();
    m_hasVBOData = true;
}


void Chunk::addFace(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color, const glm::vec3& uv, bool trans, GLuint& indexOffset) {
    glm::vec4 vertices[4];
    glm::vec2 textures[4];

    std::vector<glm::vec4>& vertexData = trans ? this->m_transparentVertexData : this->m_opaqueVertexData;
    std::vector<GLuint>& indices = trans ? this->m_transparentIndices : this->m_opaqueIndices;

    textures[0] = glm::vec2(uv[0], uv[1]);
    textures[1] = glm::vec2(uv[0] + (1.f/16.f), uv[1]);
    textures[2] = glm::vec2(uv[0] + (1.f/16.f), uv[1] + (1.f/16.f));
    textures[3] = glm::vec2(uv[0], uv[1] + (1.f/16.f));

    if (normal == glm::vec3(1, 0, 0)) {
        vertices[0] = glm::vec4(pos + glm::vec3(1, 0, 0), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(1, 0, 1), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(1, 1, 1), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(1, 1, 0), 1.0f);
    } else if (normal == glm::vec3(-1, 0, 0)) {
        vertices[0] = glm::vec4(pos + glm::vec3(0, 0, 0), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(0, 0, 1), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(0, 1, 1), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(0, 1, 0), 1.0f);
    } else if (normal == glm::vec3(0, 1, 0)) {
        vertices[0] = glm::vec4(pos + glm::vec3(0, 1, 0), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(1, 1, 0), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(1, 1, 1), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(0, 1, 1), 1.0f);
    } else if (normal == glm::vec3(0, -1, 0)) {
        vertices[0] = glm::vec4(pos + glm::vec3(0, 0, 0), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(1, 0, 0), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(1, 0, 1), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(0, 0, 1), 1.0f);
    } else if (normal == glm::vec3(0, 0, 1)) {
        vertices[0] = glm::vec4(pos + glm::vec3(0, 0, 1), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(1, 0, 1), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(1, 1, 1), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(0, 1, 1), 1.0f);
    } else if (normal == glm::vec3(0, 0, -1)) {
        vertices[0] = glm::vec4(pos + glm::vec3(0, 0, 0), 1.0f);
        vertices[1] = glm::vec4(pos + glm::vec3(1, 0, 0), 1.0f);
        vertices[2] = glm::vec4(pos + glm::vec3(1, 1, 0), 1.0f);
        vertices[3] = glm::vec4(pos + glm::vec3(0, 1, 0), 1.0f);
    }

    for (int i = 0; i < 4; ++i) {
        vertexData.push_back(vertices[i] + glm::vec4(minX, 0, minZ, 0));
        vertexData.push_back(glm::vec4(normal, 0.0f));
        vertexData.push_back(glm::vec4(textures[i], uv[2], 0.0f));
    }

    indices.push_back(indexOffset);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset);
    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indexOffset += 4;
};

void Chunk::switchFunc(BlockType currentBlock, glm::vec3& color, glm::vec3& uv) {
    switch (currentBlock) {
    case GRASS:
        break;
    case DIRT:
        uv = glm::vec3(2.f/16.f, 15.f/16.f, 0);
        break;
    case STONE:
        uv = glm::vec3(1.f/16.f, 15.f/16.f, 0);
        break;
    case WATER:
        uv = glm::vec3((float) 13/16, (float) 3/16, 1);
        break;
    case SNOW:
        uv = glm::vec3((float) 2/16, (float) 11/16, 0);
        break;
    case LAVA:
        uv = glm::vec3((float) 13/16, (float) 1/16, 1);
        break;
    case OAK_LEAVES:
        uv = glm::vec3((float) 5/16, (float) 12/16, 0);
        break;
    case ACACIA_LEAVES:
        uv = glm::vec3((float) 4/16, (float) 12/16, 0);
        break;
    case OAK_LOGS:
        uv = glm::vec3((float) 4/16, (float) 14/16, 0);
        break;
    case ACACIA_LOGS:
        uv = glm::vec3((float) 9/16, (float) 6/16, 0);
        break;

    case BEDROCK:
        uv = glm::vec3((float) 1/16, (float) 14/16, 0);
        break;
    case SAND:
        uv = glm::vec3((float) 2/16, (float) 14/16, 0);
        break;
    case ICE:
        uv = glm::vec3((float) 3/16, (float) 11/16, 0);
        break;
    case MYCELIUM:
        uv = glm::vec3((float) 14/16, (float) 11/16, 0);
        break;
    default:
        uv = glm::vec3((float) 15/16, (float) 14/16, 0);
        break;
    }
}

BlockType Chunk::getBlockAt(int x, int y, int z) {
    if (x < 0) {
        return m_neighbors[XNEG] ? m_neighbors[XNEG]->getLocalBlockAt(15, y, z) : UNLOADED;
    } else if (x >= 16) {
        return m_neighbors[XPOS] ? m_neighbors[XPOS]->getLocalBlockAt(0, y, z) : UNLOADED;
    } else if (z < 0) {
        return m_neighbors[ZNEG] ? m_neighbors[ZNEG]->getLocalBlockAt(x, y, 15) : UNLOADED;
    } else if (z >= 16) {
        return m_neighbors[ZPOS] ? m_neighbors[ZPOS]->getLocalBlockAt(x, y, 0) : UNLOADED;
    } else {
        return getLocalBlockAt(x, y, z);
    }
}

/*
void Chunk::createOpaqueVBOdata() {
    std::vector<glm::vec4> opaqueVertexData;
    std::vector<GLuint> opaqueIndices;

    GLuint indexOffset = 0;

    for (unsigned int x = 0; x < 16; ++x) {
        for (unsigned int y = 0; y < 256; ++y) {
            for (unsigned int z = 0; z < 16; ++z) {
                BlockType currentBlock = getLocalBlockAt(x, y, z);
                if (currentBlock == EMPTY || currentBlock == WATER) continue;

                glm::vec3 pos(x, y, z);
                glm::vec3 color;
                glm::vec3 uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);

                switchFunc(currentBlock, color, uv);

                    if (getBlockAt(x - 1, y, z) == EMPTY || getBlockAt(x - 1, y, z) == WATER) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(-1, 0, 0), color, uv, opaqueVertexData, opaqueIndices, indexOffset); // XNEG
                    }
                    if (getBlockAt(x + 1, y, z) == EMPTY || getBlockAt(x + 1, y, z) == WATER) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(1, 0, 0), color, uv, opaqueVertexData, opaqueIndices, indexOffset);  // XPOS
                    }
                    if (y != 0 && (getBlockAt(x, y - 1, z) == EMPTY || getBlockAt(x, y - 1, z) == WATER)) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(2.f/16.f, 15.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(0, -1, 0), color, uv, opaqueVertexData, opaqueIndices, indexOffset); // YNEG
                    }
                    if (getBlockAt(x, y + 1, z) == EMPTY || getBlockAt(x, y + 1, z) == WATER) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(8.f/16.f, 13.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(0, 1, 0), color, uv, opaqueVertexData, opaqueIndices, indexOffset);  // YPOS
                    }
                    if (getBlockAt(x, y, z - 1) == EMPTY || getBlockAt(x, y, z - 1) == WATER) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(0, 0, -1), color, uv, opaqueVertexData, opaqueIndices, indexOffset); // ZNEG
                    }
                    if (getBlockAt(x, y, z + 1) == EMPTY || getBlockAt(x, y, z + 1) == WATER) {
                        if(currentBlock == GRASS) {
                            uv = glm::vec3(3.f/16.f, 15.f/16.f, 0);
                        }
                        addFace(pos, glm::vec3(0, 0, 1), color, uv, opaqueVertexData, opaqueIndices, indexOffset);  // ZPOS
                    }
            }
        }
    }


    m_opaqueVertexData = opaqueVertexData;
    m_opaqueIndices = opaqueIndices;

    indexCounts[INDEX] = opaqueIndices.size();

    m_hasOpaqueVBOData = true;
}

void Chunk::createTransparentVBOdata() {
    std::vector<glm::vec4> transparentVertexData;
    std::vector<GLuint> transparentIndices;

    GLuint indexOffset = 0;

    for (unsigned int x = 0; x < 16; ++x) {
        for (unsigned int y = 0; y < 256; ++y) {
            for (unsigned int z = 0; z < 16; ++z) {
                BlockType currentBlock = getLocalBlockAt(x, y, z);
                if (currentBlock == EMPTY || currentBlock != WATER) continue;

                glm::vec3 pos(x, y, z);
                glm::vec3 color;
                glm::vec3 uv;

                switchFunc(currentBlock, color, uv);

                if (getBlockAt(x - 1, y, z) == EMPTY) {
                    addFace(pos, glm::vec3(-1, 0, 0), color, uv, transparentVertexData, transparentIndices, indexOffset); // XNEG
                }
                if (getBlockAt(x + 1, y, z) == EMPTY) {
                    addFace(pos, glm::vec3(1, 0, 0), color, uv, transparentVertexData, transparentIndices, indexOffset);  // XPOS
                }
                if (y != 0 && getBlockAt(x, y - 1, z) == EMPTY) {
                    addFace(pos, glm::vec3(0, -1, 0), color, uv, transparentVertexData, transparentIndices, indexOffset); // YNEG
                }
                if (getBlockAt(x, y + 1, z) == EMPTY) {
                    addFace(pos, glm::vec3(0, 1, 0), color, uv, transparentVertexData, transparentIndices, indexOffset);  // YPOS
                }
                if (getBlockAt(x, y, z - 1) == EMPTY) {
                    addFace(pos, glm::vec3(0, 0, -1), color, uv, transparentVertexData, transparentIndices, indexOffset); // ZNEG
                }
                if (getBlockAt(x, y, z + 1) == EMPTY) {
                    addFace(pos, glm::vec3(0, 0, 1), color, uv, transparentVertexData, transparentIndices, indexOffset);  // ZPOS
                }

            }
        }
    }


    m_transparentVertexData = transparentVertexData;
    m_transparentIndices = transparentIndices;

    indexCounts[INDEX] = transparentIndices.size();

    m_hasTransparentVBOData = true;
}
*/



void Chunk::bindVBOdata() {
    generateBuffer(INTERLEAVEDOPAQUE);
    bindBuffer(INTERLEAVEDOPAQUE);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_opaqueVertexData.size() * sizeof(glm::vec4), m_opaqueVertexData.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_opaqueIndices.size() * sizeof(GLuint), m_opaqueIndices.data(), GL_STATIC_DRAW);
    generateBuffer(INDEXTRANSP);
    bindBuffer(INDEXTRANSP);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndices.size() * sizeof(GLuint), m_transparentIndices.data(), GL_STATIC_DRAW);

    generateBuffer(INTERLEAVEDTRANS);
    bindBuffer(INTERLEAVEDTRANS);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_transparentVertexData.size() * sizeof(glm::vec4), m_transparentVertexData.data(), GL_STATIC_DRAW);

    generateBuffer(INDEXTRANSP);
    bindBuffer(INDEXTRANSP);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndices.size() * sizeof(GLuint), m_transparentIndices.data(), GL_STATIC_DRAW);
}

GLenum Chunk::drawMode() {
    return GL_TRIANGLES;
}

