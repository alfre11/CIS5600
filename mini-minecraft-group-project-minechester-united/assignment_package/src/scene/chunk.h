#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "drawable.h"
#include "texture.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include <glm/gtx/epsilon.hpp> // For glm::epsilonEqual

//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, UNLOADED, LAVA, BEDROCK, SAND, ICE,
    MYCELIUM, OAK_LEAVES, ACACIA_LEAVES, ACACIA_LOGS, OAK_LOGS
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.
class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;


    // The coordinates of the chunk's lower-left corner in world space
    int minX, minZ;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;


    // std::vector<glm::vec4> m_vertexData;
    // std::vector<GLuint> m_indexData;

    std::vector<glm::vec4> m_opaqueVertexData;
    std::vector<glm::vec4> m_transparentVertexData;
    std::vector<GLuint> m_opaqueIndices;
    std::vector<GLuint> m_transparentIndices;

    Texture texture;

public:
    Chunk(int x, int z, OpenGLContext* context);
    BlockType getLocalBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getLocalBlockAt(int x, int y, int z) const;
    void setLocalBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void createVBOdata() override;

    bool m_hasVBOData = false;

    void addFace(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color, const glm::vec3& uv,
                 bool trans, GLuint &indexOffset);

    void switchFunc(BlockType currentBlock, glm::vec3& color, glm::vec3& uv);

    BlockType getBlockAt(int x, int y, int z);

    void bindVBOdata();

    GLenum drawMode() override;

    bool newBlock;
    bool hasVBOData() const { return m_hasVBOData; }

    void markVBODataComplete() { m_hasVBOData = true; }

    glm::vec2 getLocation() { return glm::vec2(minX,minZ); }
};
