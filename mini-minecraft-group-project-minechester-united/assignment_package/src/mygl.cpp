#include "mygl.h"
#include "qdatetime.h"
#include <glm_includes.h>
#include <QDir>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QAudioOutput>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
    m_worldAxes(this),
    m_progLambert(this), m_progFlat(this), m_sky(this),
    m_progInstanced(this), m_terrain(this), m_player(glm::vec3(48.f, 170.f, 48.f), m_terrain), progPostProcessNoOp(this),
    progPostProcessLava(this), progPostProcessWater(this), selectedPostProcessShader(&progPostProcessNoOp), quadDrawable(this), skyDrawable(this),
    postProcessFrameBuffer(this, this->width(), this->height(), this->devicePixelRatio()),
    atlas(this), placeBlockSound(new QMediaPlayer()), placeAudioOut(new QAudioOutput())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible

    placeBlockSound->setAudioOutput(placeAudioOut);
    placeBlockSound->setSource(QUrl("qrc:/soundFiles/Sound/dirtplacesound.mp3"));
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

QString getCurrentPath() {
    QString path = QDir::currentPath();
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
#ifdef __APPLE__
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
    path = path.left(path.lastIndexOf("/"));
#endif
    return path;
}


void MyGL::createTextures() {
    QString texturePath = getCurrentPath() + "/textures/minecraft_textures_all.png";
    // std::cout << texturePath.toStdString() << std::endl;
    atlas.create(texturePath.toStdString().c_str());
    atlas.load(0);
    //atlas.bind(0);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    postProcessFrameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    postProcessFrameBuffer.create();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.0f, 0.0f, 0.0f, 1);

    printGLErrorLog();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");

    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    m_sky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");

    progPostProcessNoOp.create(":/glsl/passthrough.vert.glsl", ":/glsl/noOp.frag.glsl");
    progPostProcessLava.create(":/glsl/passthrough.vert.glsl", ":/glsl/lava.frag.glsl");
    progPostProcessWater.create(":/glsl/passthrough.vert.glsl", ":/glsl/water.frag.glsl");

    quadDrawable.createVBOdata();
    skyDrawable.createVBOdata();

    QMediaPlayer* music = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    QUrl musicUrl = QUrl("qrc:/soundFiles/Sound/startMusic.mp3");
    music->setLoops(QMediaPlayer::Infinite);
    music->setSource(musicUrl);
    music->setAudioOutput(audioOutput);
    audioOutput->setVolume(10);
    music->play();

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    createTextures();

    m_progLambert.setUnifInt("u_Texture", 0);

    printGLErrorLog();
}


void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);

    m_sky.setUnifMat4("u_ViewProj", glm::inverse(viewproj));
    m_sky.setUnifVec2("u_Dimensions", glm::vec2(w, h));
    m_sky.setUnifVec3("u_Eye", m_player.mcr_position);

    postProcessFrameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    postProcessFrameBuffer.destroy();
    postProcessFrameBuffer.create();

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    ++ticker;
    ++skyTime;
    if(ticker >= 16) {
        ++currTime;
        ticker = 0;
    }
    m_terrain.loadChunks(m_player.mcr_position);
    m_progLambert.setUnifVec3("u_Player", m_player.getPos());
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data

    // std::cout << "u_Time: " << std::fmod(currTime, 16) << std::endl;

    m_progLambert.setUnifFloat("u_Time", std::fmod(currTime, 16));
    m_progLambert.setUnifFloat("u_FogTime", skyTime);

    m_sky.setUnifFloat("u_Time", skyTime);

    progPostProcessWater.setUnifFloat("u_Time", ticker);

    double currT = QDateTime::currentMSecsSinceEpoch();
    double dt = currT - prevTime;
    m_player.tick(dt, m_player.currIns);
    prevTime = currT;
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifMat4("u_Model", glm::mat4());
    m_progLambert.setUnifMat4("u_ModelInvTr", glm::mat4());
    m_progLambert.setUnifInt("u_Texture", 0);

    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_sky.setUnifMat4("u_ViewProj", glm::inverse(viewproj));
    m_sky.setUnifVec3("u_Eye", m_player.mcr_position);


    postProcessFrameBuffer.bindFrameBuffer();
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sky.draw(skyDrawable);

    renderTerrain();
    atlas.bind(0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());


    postProcessFrameBuffer.bindToTextureSlot(1);  // Bind framebuffer texture to texture slot 1
    printGLErrorLog();
    progPostProcessWater.useMe();
    progPostProcessWater.setUnifInt("u_Texture", postProcessFrameBuffer.getTextureSlot());  // Send the texture to the shader
    if(m_player.inLiquid) {
        progPostProcessWater.setUnifInt("u_InLiquid", m_player.inLava ? 2 : 1);
    } else {
        progPostProcessWater.setUnifInt("u_InLiquid", 0);
    }


    progPostProcessWater.drawOverlay(quadDrawable);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    m_terrain.draw(m_player.mcr_position.x - 64, m_player.mcr_position.x + 64,
                   m_player.mcr_position.z - 64, m_player.mcr_position.z + 64, &m_progLambert);
}


void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
        // } else if (e->key() == Qt::Key_Right) {
        //     // m_player.rotateOnUpGlobal(-amount);
        // } else if (e->key() == Qt::Key_Left) {
        //     // m_player.rotateOnUpGlobal(amount);
        // } else if (e->key() == Qt::Key_Up) {
        //     // m_player.rotateOnRightLocal(-amount);
        // } else if (e->key() == Qt::Key_Down) {
        //     // m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_W) {
        // m_player.moveForwardLocal(amount);
        m_player.currIns.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        // m_player.moveForwardLocal(-amount);
        m_player.currIns.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        // m_player.moveRightLocal(amount);
        m_player.currIns.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        // m_player.moveRightLocal(-amount);
        m_player.currIns.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
        // m_player.moveUpGlobal(-amount);
        m_player.currIns.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
        // m_player.moveUpGlobal(amount);
        m_player.currIns.ePressed = true;
    } else if (e->key() == Qt::Key_F) {
        m_player.inFlight = !(m_player.inFlight);
    } else if (e->key() == Qt::Key_Space) {
        // m_player.moveUpGlobal(amount);
        m_player.currIns.spacePressed = true;
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_W) {
        // m_player.moveForwardLocal(amount);
        m_player.currIns.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
        // m_player.moveForwardLocal(-amount);
        m_player.currIns.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
        // m_player.moveRightLocal(amount);
        m_player.currIns.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
        // m_player.moveRightLocal(-amount);
        m_player.currIns.aPressed = false;
    } else if (e->key() == Qt::Key_Space) {
        // m_player.moveUpGlobal(amount);
        m_player.currIns.spacePressed = false;
    }  else if (e->key() == Qt::Key_Q) {
        // m_player.moveUpGlobal(amount);
        m_player.currIns.qPressed = false;
    }  else if (e->key() == Qt::Key_E) {
        // m_player.moveUpGlobal(amount);
        m_player.currIns.ePressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    setCursor(Qt::BlankCursor);

    glm::vec2 center = glm::vec2(width() / 2, height() / 2);
    glm::vec2 xy = glm::vec2(e->pos().x(), e->pos().y());

    glm::vec2 delta = xy - center;

    if (xy.x != 0 || xy.y != 0) {
        float sensitivity = 0.1f;
        float deltaYaw = delta.x * sensitivity;
        float deltaPitch = delta.y * sensitivity;

        m_player.rotateOnUpGlobal(-deltaYaw);
        m_player.rotateOnRightLocal(-deltaPitch);

        moveMouseToCenter();
    }
    //m_player.currIns.mouseX = center.x;
    //m_player.currIns.mouseY = center.y;
}

glm::ivec3 findPlaceBlock(float len, glm::ivec3 block, glm::vec3 camPos) {
    glm::ivec3 placed = block;
    // std::cout << "place beg: " << placed[0] << ", " << placed[1] << ", " << placed[2] << std::endl;
    std::vector<glm::vec3> centroids = std::vector<glm::vec3>();
    for(int i = 0; i < 3; ++i) {
        glm::vec3 newCent = glm::vec3(block.x, block.y, block.z);
        newCent[(i + 1) % 3] += 0.5f;
        newCent[(i + 2) % 3] += 0.5f;
        // std::cout << "newCent1: " << newCent[0] << ", " << newCent[1] << ", " << newCent[2] << std::endl;
        centroids.push_back(newCent);
        newCent[i] += 1.f;
        // std::cout << "newCent2: " << newCent[0] << ", " << newCent[1] << ", " << newCent[2] << std::endl;
        centroids.push_back(newCent);
    }


    float minDiff = (float) INT_MAX;
    glm::vec3 correctCent;

    for(glm::vec3 curr : centroids) {
        float diff = std::abs(glm::length(camPos - curr) - len);
        if(diff < minDiff) {
            minDiff = diff;
            correctCent = curr;
        }
    }
    // std::cout << "camPos: " << camPos[0] << ", " << camPos[1] << ", " << camPos[2] << std::endl;
    // std::cout << "correctCent: " << correctCent[0] << ", " << correctCent[1] << ", " << correctCent[2] << std::endl;

    for(int i = 0; i < 3; ++i) {
        if(correctCent[i] != block[i] + 0.5f) {
            // std::cout << block[i] << ", " << correctCent[i] << std::endl;
            if(correctCent[i] == block[i]) {
                --placed[i];
            } else if(correctCent[i] == block[i] + 1) {
                ++placed[i];
            } else {
                // std::cout << "I fucked up" << correctCent[i] << std::endl;
            }
        }
    }

    // std::cout << "place end: " << placed[0] << ", " << placed[1] << ", " << placed[2] << std::endl;

    return placed;
}


void MyGL::mousePressEvent(QMouseEvent *e) {
    // TODO
    if (e->buttons() == Qt::LeftButton) {
        glm::ivec3 blockHit;
        float len;
        if(m_player.blockInRange(m_terrain, &blockHit, &len)) {
            // std::cout << "time to break" << std::endl;
            m_terrain.setGlobalBlockAt(blockHit.x, blockHit.y, blockHit.z, EMPTY);
            m_terrain.getChunkAt(blockHit.x, blockHit.z)->newBlock = true;
            update();
        }
    }

    if (e->buttons() == Qt::RightButton) {
        glm::ivec3 blockPlaced;
        glm::ivec3 blockHit;
        float len;
        if(m_player.blockInRange(m_terrain, &blockHit, &len)) {
            // std::cout << "time to break" << std::endl;
            glm::vec3 p = m_player.getPos();
            p[1] += 1.5f;
            blockPlaced = findPlaceBlock(len, blockHit, p);
            if((blockPlaced.y + 1 < p.y - 1.5 || blockPlaced.y > p.y + 0.5f) || (blockPlaced.x > p.x + 0.5 || blockPlaced.x + 1 < p.x - 0.5)) {
                if(m_terrain.getGlobalBlockAt(blockPlaced.x, blockPlaced.y, blockPlaced.z) == EMPTY) {
                    if(placeBlockSound->isPlaying()) {
                        placeBlockSound->setPosition(0);
                    } else {
                        placeBlockSound->play();
                    }
                    m_terrain.setGlobalBlockAt(blockPlaced.x, blockPlaced.y, blockPlaced.z, GRASS);
                    m_terrain.getChunkAt(blockHit.x, blockHit.z)->newBlock = true;

                    update();
                }
            }
        }
    }
}
