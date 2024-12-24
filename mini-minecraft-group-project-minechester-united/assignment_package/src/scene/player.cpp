#include "player.h"
#include <QString>
#include <iostream>
#include <ostream>

using namespace glm;

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
    runSound(new QMediaPlayer()), audioOut(new QAudioOutput()), splashSound(new QMediaPlayer()), splashAudioOut(new QAudioOutput()) ,mcr_camera(m_camera), inFlight(true),
    onFloor(false), inLiquid(false), inLava(false)
{
    runSound->setAudioOutput(audioOut);
    runSound->setSource(QUrl("qrc:/soundFiles/Sound/runSound.mp3"));
    runSound->setLoops(QMediaPlayer::Infinite);

    splashSound->setAudioOutput(splashAudioOut);
    splashSound->setSource(QUrl("qrc:/soundFiles/Sound/Minecraft-Old-Water-Splash-Sound-Effect.mp3"));

}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain);
}

void Player::processInputs(InputBundle &inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.

    float accelerant;
    float max_velocity;

    //set acceleration in x and z directions to 1, 0, -1 based on inputs
    //to be scaled by accelerant later
    if(inputs.wPressed) {
        m_acceleration.z = 1;

        if(inputs.sPressed) m_acceleration.z = 0;
    } else if(inputs.sPressed){
        m_acceleration.z = -1;
    } else {
        m_acceleration.z = 0;
        m_velocity.z = 0;
    }
    if(inputs.aPressed) {
        m_acceleration.x = -1;
        if(inputs.dPressed) m_acceleration.x = 0;
    } else if(inputs.dPressed){
        m_acceleration.x = 1;
    } else {
        m_acceleration.x = 0;
        m_velocity.x = 0;
    }
    if(!inFlight) {
        m_acceleration.y = -1;
        if (inputs.spacePressed && inLiquid) {
            m_velocity.y = .005;
            onFloor = false;
        }
        else if(inputs.spacePressed && onFloor) {
            m_velocity.y = .015;
            onFloor = false;
        }
    } else {
        if(inputs.qPressed) {
            m_acceleration.y = 1;
        } else if(inputs.ePressed) {
            m_acceleration.y = -1;
        } else {
            m_acceleration.y = 0;
            m_velocity.y = 0;
        }
    }
    //different speeds for in flight vs not in flight
    //accelerant determines pace of acceleration
    if(inFlight) {
        accelerant = .0025;
        max_velocity = 0.015f;
        runSound->stop();
    } else if (inLiquid) {
        accelerant = .001 * 0.33;
        max_velocity = 0.005 * 0.33;
        runSound->stop();
    }
    else {
        if((std::abs(m_acceleration.x) > 0 || std::abs(m_acceleration.z) > 0) && onFloor) {
            runSound->play();
        } else {
            runSound->stop();
        }
        accelerant = .001;
        max_velocity = 0.005f;
    }

    m_velocity += m_acceleration * accelerant;

    for(int i = 0; i < 3; i+=2) {
        if(m_velocity[i] > max_velocity) m_velocity[i] = max_velocity;
        if(m_velocity[i] < -max_velocity) m_velocity[i] = -max_velocity;
    }

    if(inFlight) {
        if(m_velocity.y > max_velocity) m_velocity.y = max_velocity;
        if(m_velocity.y < -max_velocity) m_velocity.y = -max_velocity;
    }

}

void Player::computePhysics(float dT, const Terrain &terrain) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.

    if(inFlight) {
        // std::cout << dT << std::endl;
        moveForwardLocal(m_velocity.z * dT);
        moveRightLocal(m_velocity.x * dT);
        moveUpLocal(m_velocity.y * dT);
    } else {
        glm::vec3 newForward = m_forward;
        newForward.y = 0;
        newForward = glm::normalize(newForward);

        glm::vec3 newRight = m_right;
        newRight.y = 0;
        newRight = glm::normalize(newRight);

        glm::vec3 forwardMove = newForward * m_velocity.z;
        glm::vec3 rightMove = newRight * m_velocity.x;
        glm::vec3 totalMove = forwardMove + rightMove;

        glm::vec3 bottomVerts[4];
        glm::vec3 midVerts[4];
        glm::vec3 topVerts[4];

        //back left
        bottomVerts[0] = glm::vec3(m_position.x - .5, m_position.y, m_position.z - .5);
        midVerts[0] = glm::vec3(m_position.x - .5, m_position.y + 1, m_position.z - .5);
        topVerts[0] = glm::vec3(m_position.x - .5, m_position.y + 2, m_position.z - .5);

        //back right
        bottomVerts[1] = glm::vec3(m_position.x + .5, m_position.y, m_position.z - .5);
        midVerts[1] = glm::vec3(m_position.x + .5, m_position.y + 1, m_position.z - .5);
        topVerts[1] = glm::vec3(m_position.x + .5, m_position.y + 2, m_position.z - .5);

        //front left
        bottomVerts[2] = glm::vec3(m_position.x - .5, m_position.y, m_position.z + .5);
        midVerts[2] = glm::vec3(m_position.x - .5, m_position.y + 1, m_position.z + .5);
        topVerts[2] = glm::vec3(m_position.x - .5, m_position.y + 2, m_position.z + .5);

        //front right
        bottomVerts[3] = glm::vec3(m_position.x + .5, m_position.y, m_position.z + .5);
        midVerts[3] = glm::vec3(m_position.x + .5, m_position.y + 1, m_position.z + .5);
        topVerts[3] = glm::vec3(m_position.x + .5, m_position.y + 2, m_position.z + .5);

        float minZDis = (float) INT_MAX;
        ivec3 minZBlockHit;
        bool hitBlockZ = false;
        bool inLiquidZ = false;

        float minXDis = (float) INT_MAX;
        ivec3 minXBlockHit;
        bool hitBlockX = false;
        bool inLiquidX = false;

        float minYDis = (float) INT_MAX;
        ivec3 minYBlockHit;
        bool hitBlockY = false;
        bool inLiquidY = false;

        for(int i = 0; i < 4; ++i) {
            vec3 bot = bottomVerts[i];
            vec3 mid = midVerts[i];
            vec3 top = topVerts[i];
            float currDist;
            ivec3 blockHit;

            std::vector<vec3> verts = {bot, mid, top};
            for(vec3 curr: verts) {
                //z
                if(gridMarch(curr, vec3(0,0,totalMove.z) * dT, terrain, &currDist, &blockHit, inLiquidZ, inLava)) {
                    // std::cout << "hit" << std::endl;
                    if(std::abs(currDist) < std::abs(minZDis)) {
                        minZDis = currDist;
                        minZBlockHit = blockHit;
                    }
                    hitBlockZ = true;

                }

                if(gridMarch(curr, vec3(totalMove.x,0,0) * dT, terrain, &currDist, &blockHit, inLiquidX, inLava)) {
                    if(std::abs(currDist) < std::abs(minXDis)) {
                        minXDis = currDist;
                        minXBlockHit = blockHit;
                    }
                    hitBlockX = true;
                }

                if(gridMarch(curr, vec3(0,m_velocity.y,0), terrain, &currDist, &blockHit, inLiquidY, inLava)) {
                    if(std::abs(currDist) < std::abs(minYDis)) {
                        minYDis = currDist;
                        minYBlockHit = blockHit;
                    }
                    hitBlockY = true;
                }
            }
        }
        // player is in liquid if colliding with water/lava from any side
        bool prevInLiquid = inLiquid;
        inLiquid = inLiquidX || inLiquidY || inLiquidZ;

        if(!prevInLiquid && inLiquid) {
            if(splashSound->isPlaying()) {
                splashSound->setPosition(0);
            } else {
                splashSound->play();
            }
        }

        if(hitBlockY) {
            if(m_velocity.y < 0) {
                // std::cout << m_position.y << std::endl;
                onFloor = true;
            }
            if(minYBlockHit.y - m_position.y > 0) {
                float blockY = minXBlockHit.y;
                // added check for liquid
                if(blockY <= totalMove.y + m_position.y + .5 + 0.2f) {
                    m_velocity.y = 0;
                }
            } else if(minYBlockHit.y - m_position.y < 0) {
                float blockY = minYBlockHit.y + 1;
                if(blockY >= totalMove.y + m_position.y - .5 - 0.2f) {
                    m_velocity.y = 0;

                }
            }

        }

        if (abs(minXDis - minZDis) <= 0.15) {
            if(hitBlockX) {
                if(minXBlockHit.x - m_position.x > 0) {
                    float blockX = minXBlockHit.x;
                    if(blockX <= totalMove.x + m_position.x + .5 + 0.2f) {
                        totalMove.x = 0;
                    }
                } else if(minXBlockHit.x - m_position.x < 0) {
                    float blockX = minXBlockHit.x + 1;
                    if(blockX >= totalMove.x + m_position.x - .5 - 0.2f) {
                        totalMove.x = 0;

                    }
                }
            }

            //z
            if(hitBlockZ) {
                if(minZBlockHit.z - m_position.z > 0) {
                    float blockZ = minZBlockHit.z;
                    if(blockZ <= totalMove.z + m_position.z + .5 + 0.2f) {
                        totalMove.z = 0;
                    }

                } else if(minZBlockHit.z - m_position.z < 0) {
                    float blockZ = minZBlockHit.z + 1;
                    if(blockZ >= totalMove.z + m_position.z - .5 - 0.2f) {
                        totalMove.z = 0;
                    }
                }
            }

        } else if (minXDis < minZDis) {
            //x
            if(hitBlockX) {
                if(minXBlockHit.x - m_position.x > 0) {
                    float blockX = minXBlockHit.x;
                    if(blockX <= totalMove.x + m_position.x + .5 + 0.2f) {
                        totalMove.x = 0;
                    }
                } else if(minXBlockHit.x - m_position.x < 0) {
                    float blockX = minXBlockHit.x + 1;
                    if(blockX >= totalMove.x + m_position.x - .5 - 0.2f) {
                        totalMove.x = 0;

                    }
                }
            }
        } else if (minXDis > minZDis){
            //z
            if(hitBlockZ) {
                if(minZBlockHit.z - m_position.z > 0) {
                    float blockZ = minZBlockHit.z;
                    if(blockZ <= totalMove.z + m_position.z + .5 + 0.2f) {
                        totalMove.z = 0;
                    }

                } else if(minZBlockHit.z - m_position.z < 0) {
                    float blockZ = minZBlockHit.z + 1;
                    if(blockZ >= totalMove.z + m_position.z - .5 - 0.2f) {
                        totalMove.z = 0;
                    }
                }
            }

        }

        moveAlongVector(totalMove * dT);
        moveUpGlobal(m_velocity.y * dT);
    }

}

bool Player::blockInRange(const Terrain &terrain, ivec3 *blockHit, float *distOut) {

    // std::cout << "checking block" << std::endl;
    bool dummy = false;
    if(gridMarch(m_position + vec3(0, 1.5, 0), glm::normalize(m_forward) * 3.f, terrain, distOut, blockHit, dummy, inLava)) {
        return true;
    } else {
        return false;
    }
}

bool Player::gridMarch(vec3 rayOrigin, vec3 rayDirection, const Terrain &terrain, float *out_dist, ivec3 *out_blockHit, bool &inLiquid, bool &inLava) {
    float maxLen = length(rayDirection); // Farthest we search
    ivec3 currCell = ivec3(floor(rayOrigin));
    rayDirection = normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        ivec3 offset = ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getGlobalBlockAt(currCell.x, currCell.y, currCell.z);
        // check if inLiquid is already true to account for multiple vert checks

        inLiquid = ((cellType == WATER) || (cellType == LAVA) || inLiquid);
        inLava = cellType == LAVA;

        // don't consider a water/lava block as a normal block collision
        if(cellType != EMPTY && cellType != WATER && cellType != LAVA) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }

    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

glm::vec3 Player::getPos() {
    return m_position;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
