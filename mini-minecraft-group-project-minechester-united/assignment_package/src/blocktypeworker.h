#pragma once
#include "scene/terrain.h"
#include <iostream>

class BlockTypeWorker {
private:
    Terrain* m_terrain;
    glm::ivec2 m_zone;

public:
    BlockTypeWorker(Terrain* terrain, const glm::ivec2& zone)
        : m_terrain(terrain), m_zone(zone) {}

    void operator()() {
        m_terrain->instantiateZoneAt(m_zone.x, m_zone.y);
    }
};
