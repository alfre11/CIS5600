#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include "blocktypeworker.h"
#include "vboworker.h"


Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), mp_context(context)
{}

Terrain::~Terrain() {}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getGlobalBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getLocalBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                                  static_cast<unsigned int>(y),
                                  static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getGlobalBlockAt(glm::vec3 p) const {
    return getGlobalBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    std::lock_guard<std::mutex> lock(m_chunksMutex);
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    std::lock_guard<std::mutex> lock(m_chunksMutex);
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    std::lock_guard<std::mutex> lock(m_chunksMutex);
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setGlobalBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setLocalBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                           static_cast<unsigned int>(y),
                           static_cast<unsigned int>(z - chunkOrigin.y),
                           t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}



// basic noise functions to use for height methods

float noise1D(int x) {
    x = (x << 13) ^ x;
    return (1.0 - static_cast<float>((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff)) / 10737741824.0;
}

float interpNoise1D(float x) {
    int intX = int(floor(x));
    float fractX = glm::fract(x);
    float v1 = noise1D(intX);
    float v2 = noise1D(intX + 1);
    return glm::mix(v1, v2, fractX);
}

float fbm1D(float x) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise1D(x * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }

    return total;

}

float mountainFBM(float x) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise1D(x * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }

    return total;

}

float random1(glm::vec2 p) {
    return glm::fract(glm::sin((glm::dot(p, glm::vec2(127.1, 191.999)))) * 43758.5453);
}


glm::vec2 smoothF(glm::vec2 uv)
{
    return uv * uv * (glm::vec2(3.0f) - 2.0f * uv);
}

glm::vec2 random2(glm::vec2 p) {
    glm::vec2 dots = glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)),
                               glm::dot(p, glm::vec2(269.5f, 183.3f)));

    return glm::normalize(glm::fract(glm::sin(dots) * 43758.5453f) * 2.0f - glm::vec2(1.0f));
}

float interpNoise2D(glm::vec2 uv)
{
    int intX = int(floor(uv.x));
    float fractX = glm::fract(uv.x);
    int intY = int(floor(uv.y));
    float fractY = glm::fract(uv.y);

    float v1 = random1(glm::vec2(intX, intY));
    float v2 = random1(glm::vec2(intX + 1, intY));
    float v3 = random1(glm::vec2(intX, intY + 1));
    float v4 = random1(glm::vec2(intX + 1, intY + 1));
    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    return glm::mix(i1, i2, fractY);

}

float fbm2D(glm::vec2 uv)
{
    float total = 0.f;
    float amp = 0.5;
    float freq = 5.0;
    float persistence = 0.5f;
    int octaves = 8;

    for(int i = 0; i <= octaves; i++)
    {
        total += interpNoise2D(glm::vec2(uv.x * freq, uv.y * freq)) * amp;
        amp *= persistence;
        freq *= 2.f;
    }
    return total;
}



float WorleyNoise(glm::vec2 uv)
{
    // Tile the space
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.
    float secondMinDist = 1.0;
    glm::vec2 closestPoint;

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            glm::vec2 neighbor = glm::vec2(float(x), float(y));

            // Random point inside current neighboring cell
            glm::vec2 point = random2(uvInt + neighbor);

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            glm::vec2 diff = neighbor + point - uvFract;
            float dist = glm::length(diff);
            if(dist < minDist) {
                secondMinDist = minDist;
                minDist = dist;
                closestPoint = point;
            }
            else if(dist < secondMinDist) {
                secondMinDist = dist;
            }
        }
    }
    float result = 0.5 * minDist + 0.5 * secondMinDist;
    return glm::length(result);
}

float surflet2D(glm::vec2 P, glm::vec2 gridPoint)
{
    // Compute falloff function by converting linear distance to a polynomial (quintic smootherstep function)
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.0) + 15 * pow(distX, 4.0) - 10 * pow(distX, 3.0);
    float tY = 1 - 6 * pow(distY, 5.0) + 15 * pow(distY, 4.0) - 10 * pow(distY, 3.0);

    // Get the random vector for the grid point
    glm::vec2 gradient = random2(gridPoint);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float PerlinNoise2D(glm::vec2 uv)
{
    // Tile the space
    glm::vec2 uvXLYL = glm::floor(uv);
    glm::vec2 uvXHYL = uvXLYL + glm::vec2(1,0);
    glm::vec2 uvXHYH = uvXLYL + glm::vec2(1,1);
    glm::vec2 uvXLYH = uvXLYL + glm::vec2(0,1);

    return surflet2D(uv, uvXLYL) + surflet2D(uv, uvXHYL) + surflet2D(uv, uvXHYH) + surflet2D(uv, uvXLYH);
}

glm::vec3 random3(glm::vec3 p) {

    glm::vec3 dots = glm::vec3(glm::dot(p,glm::vec3(127.1, 311.7, 281.2)),
                               glm::dot(p,glm::vec3(269.5, 183.3, 342.9)),
                               glm::dot(p, glm::vec3(420.6, 631.2, 589.1)));

    return glm::fract(glm::sin(dots) * 43758.5453f);
}

float surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * glm::vec3(std::pow(t2.x, 5.f), std::pow(t2.y, 5.f), std::pow(t2.z, 5.f))
                  + 15.f * glm::vec3(std::pow(t2.x, 4.f), std::pow(t2.y, 4.f), std::pow(t2.z, 4.f))
                  - 10.f * glm::vec3(std::pow(t2.x, 3.f), std::pow(t2.y, 3.f), std::pow(t2.z, 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1.f);
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;
}

float perlinNoiseCaves(glm::vec3 p) {
    // surflets and to use in trilinear interpolation later
    float result = 0;
    float surflets[8];
    int surfletIndex = 0;
    glm::vec3 fract = p - glm::floor(p);
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surflets[surfletIndex] = surflet3D(p, glm::floor(p / 10.f) * 10.f + glm::vec3(dx, dy, dz));
                surfletIndex++;
            }
        }
    }
    // trilinear interpolation
    // face at z == 0
    float e = surflets[0] * (1 - fract.x) + surflets[1] * fract.x;
    float f = surflets[2] * (1 - fract.x) + surflets[3] * fract.x;
    float g1 = e * (1 - fract.y) + f * fract.y;

    float l = surflets[4] * (1 - fract.x) + surflets[5] * fract.x;
    float m = surflets[6] * (1 - fract.x) + surflets[7] * fract.x;
    float g2 = l * (1 - fract.y) + m * fract.y;

    result = g1 * (1 - fract.z) + g2 * fract.z;

    return result;
}

float smoothstep(double edge0, double edge1, float f) {
    float t = glm::clamp((f - edge0) / (edge1 - edge0), 0.0, 1.0);
    t *= t * (3.0 - 2.0 * t);
    return t;
}


float Terrain::grassHeight(glm::vec2 xz) {
    float height = 0;

    float amp = 0.03125;
    float freq = 256.f;
    float perlinHeight = 0;
    glm::vec2 offset = glm::vec2(fbm2D(xz / 256.f), fbm2D(xz / 300.f)) + glm::vec2(1000);
    for (int i = 0; i < 4; i++) {
        float currH = PerlinNoise2D((xz + offset) / freq);
        perlinHeight += currH * amp;
        amp *= 2;
        freq *= 0.5;
    }

    height = floor(145 + perlinHeight * 100);

    return height;
}

// noise function for mountain biome height
float Terrain::mountainHeight(glm::vec2 xz) {
    float height = 0;

    float amp = 0.5;
    float freq = 256;
    for (int i = 0; i < 6; i++) {
        glm::vec2 offset = glm::vec2(fbm2D(xz / 256.f), fbm2D(xz / 300.f)) + glm::vec2(1000);
        float currH = abs(PerlinNoise2D((xz + offset) / freq));

        height += currH * amp;
        amp *= sqrt(0.5);
        freq *= 0.5;
    }
    height = floor (170 + height * 110);
    return height;
}

float Terrain::desertHeight(glm::vec2 xz) {
    float height = 0;

    float amp = 0.5;
    float freq = 64.f;
    float perlinHeight = 0;
    glm::vec2 offset = glm::vec2(fbm2D(xz / 256.f), fbm2D(xz / 300.f)) + glm::vec2(1000);
    for (int i = 0; i < 4; i++) {
        float currH = PerlinNoise2D((xz + offset) / freq);
        perlinHeight += currH * amp;
        amp *= 0.5;
        freq *= 0.5;
    }
    float smooth = smoothstep(0.6, 0.5, perlinHeight);
    perlinHeight *= 100;
    height = (smooth * 0.9 + (0.1 * perlinHeight));

    return floor(145 + height);
}

float Terrain::snowHeight(glm::vec2 xz) {
    float height = 0;

    float amp = 0.5;
    float freq = 256;
    for (int i = 0; i < 4; i++) {
        glm::vec2 offset = glm::vec2(fbm2D(xz / 256.f), fbm2D(xz / 300.f)) + glm::vec2(1000);
        float currH = abs(PerlinNoise2D((xz + offset) / freq));

        height += currH * amp;
        amp *= 0.5;
        freq *= 0.5;
    }
    height = floor (135 + height * 100);
    return height;
}

float Terrain::mushroomHeight(glm::vec2 xz) {
    float height = 0;

    float amp = 0.25;
    float freq = 128.f;
    float perlinHeight = 0;
    glm::vec2 offset = glm::vec2(fbm2D(xz / 256.f), fbm2D(xz / 300.f)) + glm::vec2(1000);
    for (int i = 0; i < 4; i++) {
        float currH = PerlinNoise2D((xz + offset) / freq);
        perlinHeight += currH * amp;
        amp *= 0.25;
        freq *= 0.5;
    }
    height = floor(135 + perlinHeight * 100);

    return height;
}

/* high temperature, low moisture correlates with a desert
             * high temperature, high moisture correlates with a mushroom area
             * low temperature, low moisture correlates with a snow area
             * low tempearture, high moisture correlates with a mountain area
             * medium temoerature, medium humidity creates a flat grass area
             * Other mixtures smoothstep between biomes
            */

int Terrain::finalHeight(glm::vec2 xz, float temp, float moist) {

        // these booleans determine whether a biome could be part of the mixed height
    // bias is given towards grass areas
    bool mushroomPossible = temp >= 0.25 && moist >= 0.25;
    bool desertPossible = temp >= 0.25 && moist <= 0.75;
    bool mountainPossible = temp <= 0.75 && moist >= 0.25;
    bool snowPossible = temp <= 0.75 && moist <= 0.75;
    bool grassPossible = temp <= 0.75 && temp >= 0.25 &&
                         moist <= 0.75 && moist >= 0.25;

    int finalHeight = 0;
    // desert snow mix
    if (desertPossible && snowPossible && !grassPossible) {
        float t = smoothstep(0.25, 0.75, temp);
        float m = smoothstep(0, 0.5, moist);
        finalHeight = int(glm::mix((glm::mix(snowHeight(xz), desertHeight(xz), t)), grassHeight(xz), m));
    }
    // snow mountain mix
    else if (snowPossible && mountainPossible && !grassPossible) {
        float t = smoothstep(0, 0.5, temp);
        float m = smoothstep(0.25, 0.75, moist);
        finalHeight = int(glm::mix((glm::mix(snowHeight(xz), mountainHeight(xz), m)), grassHeight(xz), t));
    }
    // mountain mushroom mix
    else if (mountainPossible && mushroomPossible && !grassPossible) {
        float t = smoothstep(0.25, 0.75, temp);
        float m = smoothstep(0.5, 1.f, moist);
        finalHeight = int(glm::mix(grassHeight(xz), (glm::mix(mountainHeight(xz), mushroomHeight(xz), t)), m));
    }
    // mushroom desert mix
    else if (desertPossible && mushroomPossible && !grassPossible) {
        float t = smoothstep(0.5, 1.f, temp);
        float m = smoothstep(0.25, 0.75, moist);
        finalHeight = int(glm::mix(grassHeight(xz), (glm::mix(desertHeight(xz), mushroomHeight(xz), m)), t));
    }
    // Grass desert mix
    else if (grassPossible && temp >= 0.5 && moist < 0.5){
        float t = smoothstep(0.625, 0.9, temp);
        float m = smoothstep(0.1, 0.375, moist);

        float gh = grassHeight(xz);
        float dh = desertHeight(xz);

        finalHeight = int(glm::mix(gh, dh, t) * 0.5 + glm::mix(dh, gh, m) * 0.5);
    }
    // Grass snow mix
    else if (grassPossible && temp < 0.5 && moist < 0.5){
        float t = smoothstep(0.1, 0.375, temp);
        float m = smoothstep(0.1, 0.375, moist);

        float gh = grassHeight(xz);
        float sh = snowHeight(xz);

        finalHeight = int(glm::mix(sh, gh, t) * 0.5 + glm::mix(sh, gh, m) * 0.5);
    }
    // Grass mushroom mix
    else if (grassPossible && temp >= 0.5 && moist >= 0.5){
        float t = smoothstep(0.625, 0.9, temp);
        float m = smoothstep(0.625, 0.9, moist);

        float gh = grassHeight(xz);
        float muh = mushroomHeight(xz);

        finalHeight = int(glm::mix(gh, muh, t) * 0.5 + glm::mix(gh, muh, m) * 0.5);
    }
    // Grass mountain mix
    else if (grassPossible && temp < 0.5 && moist >= 0.5){
        float t = smoothstep(0.1, 0.375, temp);
        float m = smoothstep(0.625, 0.9, moist);

        float gh = grassHeight(xz);
        float moh = mountainHeight(xz);

        finalHeight = int(glm::mix(moh, gh, t) * 0.5 + glm::mix(gh, moh, m) * 0.5);
    }
    return finalHeight;
}

void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (!hasChunkAt(x,z)) {
                int xFloor = static_cast<int>(glm::floor(x / 64.f)) * 64;
                int zFloor = static_cast<int>(glm::floor(z / 64.f)) * 64;
                instantiateZoneAt(xFloor,zFloor);
            }
            const uPtr<Chunk> &chunk = getChunkAt(x, z);
            if (!chunk->hasVBOData()) chunk->createVBOdata();
            chunk->bindVBOdata();
            shaderProgram->drawInterleaved(*chunk, false);
        }
    }
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (!hasChunkAt(x,z)) {
                int xFloor = static_cast<int>(glm::floor(x / 64.f)) * 64;
                int zFloor = static_cast<int>(glm::floor(z / 64.f)) * 64;
                instantiateZoneAt(xFloor,zFloor);
            }
            const uPtr<Chunk> &chunk = getChunkAt(x, z);
            shaderProgram->drawInterleaved(*chunk, true);
        }
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    std::unique_lock<std::mutex> lock(m_chunksMutex);
    uPtr<Chunk> chunk = mkU<Chunk>(x, z, mp_context);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    lock.unlock();
    if(hasChunkAt(x, z + 16)) {
        lock.lock();
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
        lock.unlock();
    }
    if(hasChunkAt(x, z - 16)) {
        lock.lock();
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
        lock.unlock();
    }
    if(hasChunkAt(x + 16, z)) {
        lock.lock();
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
        lock.unlock();
    }
    if(hasChunkAt(x - 16, z)) {
        lock.lock();
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
        lock.unlock();
    }



    //This will define where the tree can spawn in (Only 1 tree allowed per chunk to avoid crowding)
    int xPlace = rand() % 7;
    int zPlace = rand() % 7;
    xPlace += 4; //keep it towards the center to avoid redner conflicts
    zPlace += 4;

    int yVal = 0; //This is going to be the y value of tree


    int grassFactor =  0;
    int mountainFactor = 0;


    bool searchY = false;


    int xCounter = 0;




    int drawX = 0;
    int drawY = 0;
    int drawZ = 0;
    for (int i = x; i < x + 16; i++) {
        xCounter++;
        int zCounter = 0;
        for (int j = z; j < z + 16; j++) {
            zCounter++;
            glm::vec2 xz(i, j);
            // calculate tempearture and moisture, giving bias towards x/z respectively
            float biomeNoise = PerlinNoise2D(glm::vec2(i, j) / 1024.f);
            float currTemp = 0.5 * (fmin((fmax(biomeNoise + i / 500.f, -1.f)), 1.f) + 1.f);
            float currMoist = 0.5 * (fmin((fmax(biomeNoise + j / 500.f, -1.f)), 1.f) + 1.f);

            // these booleans determine the type of blocks that will be placed

            bool mushroomTerrain = currTemp >= 0.5 && currMoist >= 0.5;
            bool desertTerrain = currTemp >= 0.5 && currMoist < 0.5;
            bool mountainTerrain = currTemp < 0.5 && currMoist >= 0.5;
            bool snowTerrain = currTemp < 0.5 && currMoist < 0.5;
            bool grassTerrain = currTemp >= 0.25 && currTemp <= 0.75 &&
                                currMoist >= 0.25 && currMoist <= 0.75;

            // this function calculates a mixed height that smoothly steps between biomes

            int mixedBiomeHeight = finalHeight(xz, currTemp, currMoist);
            if (xCounter == xPlace && zCounter == zPlace) {
                yVal = 0;
                drawX = i;
                drawZ = j;
                searchY = true;
            }


            // Grass terrain
            if (grassTerrain) {
                grassFactor++;

                //float currGrassHeight = grassHeight(xz);
                for (int y = 0; y <= mixedBiomeHeight; y++) {
                    // bedrock at 0
                    if (y == 0) {
                        setGlobalBlockAt(i, y, j, BEDROCK);
                        if (searchY) yVal = std::max(yVal, y);

                        // caves from 1 to 64
                    } else if (y <= 128 && y >= 1) {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);


                        // commented out for easier testing until all blocks show up
                        /*float caveValue = perlinNoiseCaves(glm::vec3(i, y, j));
                        if (caveValue >= 0) {
                            setGlobalBlockAt(i, y, j, STONE);
                        } else if (y < 25) {
                            setGlobalBlockAt(i, y, j, LAVA);
                        } else {
                            setGlobalBlockAt(i, y, j, EMPTY);
                        }
                    */
                        // dirt from 129 to grass height
                    } else if (y < mixedBiomeHeight) {
                        setGlobalBlockAt(i, y, j, DIRT);
                        if (searchY) yVal = std::max(yVal, y);


                        // grass at grass height
                    } else if (y == mixedBiomeHeight) {
                        setGlobalBlockAt(i, y, j, GRASS);
                        if (searchY) yVal = std::max(yVal, y);

                    }
                }

                // water should be above grass if grass is less than 160
                int waterHeight = fmax(mixedBiomeHeight, 129);
                //MODDED WATER HEIGHT FOR TESTING< CHANGE BACK FOR NORMAL
                for (int y = waterHeight; y < 138; y++) {
                    setGlobalBlockAt(i, y, j, WATER);
                    if (searchY) yVal = std::max(yVal, y);

                }

            }
            // Mountain Terrain
            else if (mountainTerrain) {
                mountainFactor++;

                for (int y = 0; y <= mixedBiomeHeight; y++) {
                    if (y == 0) {
                        setGlobalBlockAt(i, y, j, BEDROCK);
                        if (searchY) yVal = std::max(yVal, y);

                    } else if (y <= 128 && y >= 1) {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);

                        // commented out for easier testing until all blocks start showing up
                        /*float caveValue = perlinNoiseCaves(glm::vec3(i, y, j));
                        if (caveValue >= 0) {
                            setGlobalBlockAt(i, y, j, STONE);
                        } else if (y < 25) {
                            setGlobalBlockAt(i, y, j, LAVA);
                        } else {
                            setGlobalBlockAt(i, y, j, EMPTY);
                        }
                    */

                    } else if (y == mixedBiomeHeight && y > 200) {
                        setGlobalBlockAt(i, y, j, SNOW);
                        if (searchY) yVal = std::max(yVal, y);
                    }
                    else {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);
                    }
                }
                int waterHeight = fmax(mixedBiomeHeight, 129);
                for (int y = waterHeight; y < 138; y++) {
                    setGlobalBlockAt(i, y, j, WATER);
                    if (searchY) yVal = std::max(yVal, y);
                }


            }
            // desert Terrain
            else if (desertTerrain) {
                grassFactor++;
                //float currDesertHeight = desertHeight(xz);
                for (int y = 0; y <= mixedBiomeHeight; y++) {
                    // bedrock at 0
                    if (y == 0) {
                        setGlobalBlockAt(i, y, j, BEDROCK);
                        if (searchY) yVal = std::max(yVal, y);

                        // caves from 1 to 64
                    } else if (y <= 128 && y >= 1) {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);

                        // commented out for easier testing until all blocks show up
                        /*float caveValue = perlinNoiseCaves(glm::vec3(i, y, j));
                        if (caveValue >= 0) {
                            setGlobalBlockAt(i, y, j, STONE);
                        } else if (y < 25) {
                            setGlobalBlockAt(i, y, j, LAVA);
                        } else {
                            setGlobalBlockAt(i, y, j, EMPTY);
                        }
                    */
                    } else {
                        setGlobalBlockAt(i, y, j, SAND);
                        if (searchY) yVal = std::max(yVal, y);

                    }
                }
                int waterHeight = fmax(mixedBiomeHeight, 129);
                for (int y = waterHeight; y < 138; y++) {
                    setGlobalBlockAt(i, y, j, WATER);
                    if (searchY) yVal = std::max(yVal, y);

                }
            }
            // snow Terrain
            else if (snowTerrain) {
                mountainFactor++;
                //float currSnowHeight = snowHeight(xz);
                for (int y = 0; y <= mixedBiomeHeight; y++) {
                    // bedrock at 0
                    if (y == 0) {
                        setGlobalBlockAt(i, y, j, BEDROCK);
                        if (searchY) yVal = std::max(yVal, y);

                        // caves from 1 to 64
                    } else if (y <= 128 && y >= 1) {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);

                        // commented out for easier testing until all blocks show up
                        /*float caveValue = perlinNoiseCaves(glm::vec3(i, y, j));
                        if (caveValue >= 0) {
                            setGlobalBlockAt(i, y, j, STONE);
                        } else if (y < 25) {
                            setGlobalBlockAt(i, y, j, LAVA);
                        } else {
                            setGlobalBlockAt(i, y, j, EMPTY);
                        }
                    */
                    } else {
                        setGlobalBlockAt(i, y, j, SNOW);
                        if (searchY) yVal = std::max(yVal, y);
                    }

                }
                // snow biome puts ice on top of water
                int waterHeight = fmax(mixedBiomeHeight, 129);
                for (int y = waterHeight; y < 138; y++) {

                    y == 137 ? setGlobalBlockAt(i, y, j, ICE) : setGlobalBlockAt(i, y, j, WATER);
                    if (searchY) yVal = std::max(yVal, y);

                }
            }
            // mushroom Terrain
            else if (mushroomTerrain) {
                grassFactor++;
                mountainFactor++;
                //float currMushroomHeight = mushroomHeight(xz);
                for (int y = 0; y <= mixedBiomeHeight; y++) {
                    // bedrock at 0
                    if (y == 0) {
                        setGlobalBlockAt(i, y, j, BEDROCK);
                        if (searchY) yVal = std::max(yVal, y);

                        // caves from 1 to 64
                    } else if (y <= 128 && y >= 1) {
                        setGlobalBlockAt(i, y, j, STONE);
                        if (searchY) yVal = std::max(yVal, y);

                        // commented out for easier testing until all blocks show up
                        /*float caveValue = perlinNoiseCaves(glm::vec3(i, y, j));
                        if (caveValue >= 0) {
                            setGlobalBlockAt(i, y, j, STONE);
                        } else if (y < 25) {
                            setGlobalBlockAt(i, y, j, LAVA);
                        } else {
                            setGlobalBlockAt(i, y, j, EMPTY);
                        }
                    */
                        // dirt from 129 to grass height
                    } else if (y < mixedBiomeHeight) {
                        setGlobalBlockAt(i, y, j, DIRT);
                        if (searchY) yVal = std::max(yVal, y);

                        // grass at grass height
                    } else if (y == mixedBiomeHeight) {
                        setGlobalBlockAt(i, y, j, MYCELIUM);
                        if (searchY) yVal = std::max(yVal, y);

                    }
                }
                int waterHeight = fmax(mixedBiomeHeight, 129);
                for (int y = waterHeight; y < 138; y++) {
                    setGlobalBlockAt(i, y, j, WATER);
                    if (searchY) yVal = std::max(yVal, y);

                }
            }
            searchY = false;
        }
    }


    int random = rand() % 10;
    if (mountainFactor > grassFactor) { // Mountain/Snowy Areas will have DEAD or Oak Trees
        if (random > 6) {
            if (random < 8) {
                Tree(drawX, yVal, drawZ, 1);
            } else {
                Tree(drawX, yVal, drawZ, 2);
            }
        }
    } else { // Grassy areas will have different kinds of LIVING trees
        if (random > 4) {
            if (random < 7) {
                Tree(drawX, yVal, drawZ, 3);
            } else {
                Tree(drawX, yVal, drawZ, 2);
            }
        }
    }





    return cPtr;
}

void Terrain::instantiateZoneAt(int xZone, int zZone) {
    for (int x = xZone; x < xZone + 64; x += 16) {
        for (int z = zZone; z < zZone + 64; z += 16) {
            if (!hasChunkAt(x, z)) {
                instantiateChunkAt(x, z);
            }
        }
    }
    std::lock_guard<std::mutex> lock(m_terrainMutex);
    m_generatedTerrain.insert(toKey(xZone, zZone));
}

void Terrain::CreateTestScene()
{
    instantiateZoneAt(0, 0);
    instantiateZoneAt(64, 0);
    instantiateZoneAt(0, 64);
    instantiateZoneAt(-64, 0);
    instantiateZoneAt(0, -64);
    instantiateZoneAt(64, 64);
    instantiateZoneAt(-64, -64);
    instantiateZoneAt(64, -64);
    instantiateZoneAt(-64, 64);

    std::lock_guard<std::mutex> lock(m_terrainMutex);
    m_generatedTerrain.insert(toKey(0, 0));
    m_generatedTerrain.insert(toKey(64, 0));
    m_generatedTerrain.insert(toKey(0, 64));
    m_generatedTerrain.insert(toKey(-64, 0));
    m_generatedTerrain.insert(toKey(0, -64));
    m_generatedTerrain.insert(toKey(64, 64));
    m_generatedTerrain.insert(toKey(-64, -64));
    m_generatedTerrain.insert(toKey(64, -64));
    m_generatedTerrain.insert(toKey(-64, 64));
}

void Terrain::loadChunks(const glm::vec3& playerPos) {
    int playerZoneX = static_cast<int>(std::floor(playerPos.x / 64.0f)) * 64;
    int playerZoneZ = static_cast<int>(std::floor(playerPos.z / 64.0f)) * 64;

    std::vector<std::thread> threads;

    for (int xZone = playerZoneX - 64; xZone <= playerZoneX + 64; xZone += 64) {
        for (int zZone = playerZoneZ - 64; zZone <= playerZoneZ + 64; zZone += 64) {
            glm::ivec2 zone(xZone, zZone);

            std::unique_lock<std::mutex> lock(m_terrainMutex);
            if (m_generatedTerrain.find(toKey(zone.x, zone.y)) == m_generatedTerrain.end()) {
                lock.unlock();
                threads.emplace_back(BlockTypeWorker(this, zone));
            } else {
                lock.unlock();
                for (int xChunk = xZone; xChunk < xZone + 64; xChunk += 16) {
                    for (int zChunk = zZone; zChunk < zZone + 64; zChunk += 16) {
                        Chunk* chunk = m_chunks[toKey(xChunk, zChunk)].get();

                        threads.emplace_back(
                            VBOWorker(chunk)
                            );
                    }
                }
            }
        }
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}


void Terrain::Tree(int x, int y, int z, int type) {

    BlockType leaf = OAK_LEAVES;

    BlockType log = OAK_LOGS;
    if (type == 3) {
        log = ACACIA_LOGS;
        leaf = ACACIA_LEAVES;
    }



    // Generate a trunk with random height
    int tHeight = 4 + (rand() % 5); // Trunk height between 4 and 7
    for (int i = -1; i < tHeight; ++i) {
        setGlobalBlockAt(x, y + i, z, log);
    }


    if (type == 2) {
        int canopyRadius = 2;
        int canopyHeight = 3;
        int canopyStart = y + tHeight;
        for (int deltay = 0; deltay < canopyHeight; ++deltay) {
            int currentRadius = canopyRadius - deltay;
            for (int deltax = -currentRadius; deltax <= currentRadius; ++deltax) {
                for (int deltaz = -currentRadius; deltaz <= currentRadius; ++deltaz) {

                    if (deltax * deltax + deltaz * deltaz <= currentRadius * currentRadius) {
                        if (hasChunkAt(x+deltax, z+deltaz)) setGlobalBlockAt(x + deltax, canopyStart + deltay, z + deltaz, leaf);
                    }
                }
            }
        }
    }

    if (type == 3) {
        int canopyRadius = 3;
        int frondHeight = 2;
        int canopyStart = y + tHeight;

        for (int deltay = 0; deltay < frondHeight; ++deltay) {
            int currentRadius = canopyRadius + deltay;

            for (int deltax = -currentRadius; deltax <= currentRadius; ++deltax) {
                for (int deltaz = -currentRadius; deltaz <= currentRadius; ++deltaz) {
                    if (deltax * deltax + deltaz * deltaz <= currentRadius * currentRadius) {
                        if (hasChunkAt(x + deltax, z + deltaz)) {
                            setGlobalBlockAt(x + deltax, canopyStart + deltay, z + deltaz, leaf);
                        }
                    }
                }
            }
        }
    }



}





















