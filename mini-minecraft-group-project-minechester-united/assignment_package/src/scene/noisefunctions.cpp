#include "noisefunctions.h"

noiseFunctions::noiseFunctions() {}

// basic noise functions to use for height methods

/**float noise1D(int x) {
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

glm::vec2 smoothF(glm::vec2 uv)
{
    return uv * uv * (glm::vec2(3.0f) - 2.0f * uv);
}

float interpNoise2D(glm::vec2 uv)
{
    const float k = 257.0f;
    glm::vec4 l(glm::floor(uv.x), glm::floor(uv.y), glm::fract(uv.x), glm::fract(uv.y));
    float u = l.x + l.y * k;
    glm::vec4 v(u, u + 1.0f, u + k, u + k + 1.0f);

    v = glm::fract(glm::fract(1.23456789f * v) * v / 0.987654321f);

    glm::vec2 lzw = smoothF(glm::vec2(l.z, l.w));
    l.z = lzw.x;
    l.w = lzw.y;

    float lx = glm::mix(v.x, v.y, l.z);
    float ly = glm::mix(v.z, v.w, l.z);

    // Final mix based on l.w
    return glm::mix(lx, ly, l.w);
}

float fbm2D(glm::vec2 uv)
{
    float total = 0;
    float amp = 0.5;
    float freq = 5.0;
    float persistence = 0.5f;
    int octaves = 8;

    for(int i = 0; i <= octaves; i++)
    {
        total += interpNoise2D(uv*freq) * amp;
        amp *= persistence;
        freq *= 2.;
    }
    return total;
}

glm::vec2 random2(glm::vec2 p) {
    glm::vec2 dots = glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)),
                               glm::dot(p, glm::vec2(269.5f, 183.3f)));

    return glm::normalize(glm::fract(glm::sin(dots) * 43758.5453f) * 2.0f - glm::vec2(1.0f));
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

float surflet(glm::vec2 P, glm::vec2 gridPoint)
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

float PerlinNoise(glm::vec2 uv)
{
    // Tile the space
    glm::vec2 uvXLYL = glm::floor(uv);
    glm::vec2 uvXHYL = uvXLYL + glm::vec2(1,0);
    glm::vec2 uvXHYH = uvXLYL + glm::vec2(1,1);
    glm::vec2 uvXLYH = uvXLYL + glm::vec2(0,1);

    return surflet(uv, uvXLYL) + surflet(uv, uvXHYL) + surflet(uv, uvXHYH) + surflet(uv, uvXLYH);
}**/

