#include "mygl.h"
#include "scene/node.h"
#include "scene/translatenode.h"
#include "scene/scalenode.h"
#include "scene/rotatenode.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      prog_flat(this),
      m_geomGrid(this), m_geomSquare(this, {glm::vec3(0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, -0.5f, 1.f),
                                            glm::vec3(0.5f, -0.5f, 1.f)}),
      m_showGrid(true),
      mp_selectedNode(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_geomGrid.destroy();
}

uPtr<Node> MyGL::createGraph() {



    //Torso is root
    uPtr<Node> torso = mkU<ScaleNode>(2.0f, 4.0f, QString("torso"), glm::vec3(0.f,1.f,0.f),  &m_geomSquare);
    uPtr<Node> torsoUnscale = mkU<ScaleNode>(0.5f, .25f, QString("unscale torso"), glm::vec3(1.f,1.f,1.f),  nullptr);


    //Head
    uPtr<Node> head = mkU<ScaleNode>(1.0f, 1.0f,QString("Head"), glm::vec3(0.f,0.f,0.f), &m_geomSquare);
    uPtr<Node> headTrans = mkU<TranslateNode>(0.0f, 0.5f, QString("Head Translation"), glm::vec3(1.f,1.f,1.f), nullptr);
    uPtr<Node> headRot = mkU<RotateNode>(0.0f,QString("Head Rotation"), glm::vec3(0.0f, 0.0f, 0.0f), nullptr);
    uPtr<Node> headTrans2 = mkU<TranslateNode>(0.0f, 2.0f, QString("Head Translation"), glm::vec3(1.f,1.f,1.f), nullptr);

    //head add children
    headTrans->addChild(std::move(head));
    headRot->addChild(std::move(headTrans));
    headTrans2->addChild(std::move(headRot));
    torsoUnscale->addChild(std::move(headTrans2));

    //Left Arm:
    uPtr<Node> leftBic  = mkU<ScaleNode>(1.5f, 0.5f, QString("Left Bicep"), glm::vec3(0.f,0.5f,0.5f), &m_geomSquare);
    uPtr<Node> leftBicTrans = mkU<TranslateNode>(-1.4f, 0.75f, QString("Left Bicep Translation"), glm::vec3(0.6f,0.6f,0.6f), nullptr);

    uPtr<Node> leftForearm  = mkU<ScaleNode>(1.5f, 0.3f, QString("Left Forearm"), glm::vec3(0.7f,0.5f,0.1f), &m_geomSquare);
    uPtr<Node> leftForearmTrans = mkU<TranslateNode>(-0.75f, 0.0f, QString("Left Forearm Translation"), glm::vec3(0.7f,0.5f,0.1f), nullptr);
    uPtr<Node> leftForearmRotate = mkU<RotateNode>(30.0f, QString("Left Forearm Rotation"), glm::vec3(0.6f,0.6f,0.6f), nullptr);
    uPtr<Node> leftForeTrans2 = mkU<TranslateNode>(-2.0f, 0.75f, QString("Left Forearm Translation 2"), glm::vec3(0.6f,0.6f,0.6f), nullptr);


    uPtr<Node> leftArmRot = mkU<RotateNode>(30.0f, QString("Left Arm Rotate"), glm::vec3(0.0f, 0.0f, 0.0f), nullptr);
    uPtr<Node> leftArmTrans = mkU<TranslateNode>(0.0f, 0.5f, QString("Left Arm"), glm::vec3(0.0f, 0.0f, 0.0f), nullptr);


    // //Right Arm:
    uPtr<Node> rightBic  = mkU<ScaleNode>(1.5f, 0.5f, QString("Right Bicep"), glm::vec3(0.f,0.5f,0.5f), &m_geomSquare);
    uPtr<Node> rightBicTrans = mkU<TranslateNode>(1.4f, 0.75f, QString("Right Bicep Translation"), glm::vec3(0.6f,0.6f,0.6f), nullptr);

    uPtr<Node> rightForearm  = mkU<ScaleNode>(1.5f, 0.3f, QString("Right Forearm"), glm::vec3(0.7f,0.5f,0.1f), &m_geomSquare);
    uPtr<Node> rightForearmTrans = mkU<TranslateNode>(0.75f, 0.0f, QString("Right Forearm Translation"), glm::vec3(0.7f,0.5f,0.1f), nullptr);
    uPtr<Node> rightForearmRotate = mkU<RotateNode>(-30.0f, QString("Right Forearm Rotation"), glm::vec3(0.6f,0.6f,0.6f), nullptr);
    uPtr<Node> rightForeTrans2 = mkU<TranslateNode>(2.0f, 0.75f, QString("Right Forearm Translation 2"), glm::vec3(0.6f,0.6f,0.6f), nullptr);


    uPtr<Node> rightArmRot = mkU<RotateNode>(-30.0f, QString("Right Arm Rotate"), glm::vec3(0.0f, 0.0f, 0.0f), nullptr);
    uPtr<Node> rightArmTrans = mkU<TranslateNode>(0.0f, 0.5f, QString("Right Arm"), glm::vec3(0.0f, 0.0f, 0.0f), nullptr);

    //arms children tree
    leftBicTrans->addChild(std::move(leftBic));

    leftForearmTrans->addChild(std::move(leftForearm));
    leftForearmRotate->addChild(std::move(leftForearmTrans));
    leftForeTrans2->addChild(std::move(leftForearmRotate));
    leftArmRot->addChild(std::move(leftBicTrans));
    leftArmRot->addChild(std::move(leftForeTrans2));
    leftArmTrans->addChild(std::move(leftArmRot));

    torsoUnscale->addChild(std::move(leftArmTrans));


    rightBicTrans->addChild(std::move(rightBic));

    rightForearmTrans->addChild(std::move(rightForearm));
    rightForearmRotate->addChild(std::move(rightForearmTrans));

    rightForeTrans2->addChild(std::move(rightForearmRotate));
    rightArmRot->addChild(std::move(rightBicTrans));
    rightArmRot->addChild(std::move(rightForeTrans2));
    rightArmTrans->addChild(std::move(rightArmRot));

    torsoUnscale->addChild(std::move(rightArmTrans));


    //torso addchildren
    torso->addChild(std::move(torsoUnscale));
    return torso;
}

void MyGL::traverse(Node* curr, glm::mat3 acc) {
    if (curr == nullptr) return;
    glm::mat3 currMat = acc * curr->getTransformationMatrix();

    for(const uPtr<Node>& n : curr->getChildren()) {
        MyGL::traverse(n.get(), currMat);
    }

    if(curr->getPolygon() != nullptr) {
        prog_flat.setModelMatrix(currMat);
        (curr->getPolygon())->setColor(curr->getColor());
        prog_flat.draw(*this, *curr->getPolygon());
    }
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the scene geometry
    m_geomGrid.create();
    m_geomSquare.create();

    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);


    // TODO: Call your scene graph construction function here

    this->root = createGraph();
    MyGL::sig_sendRootNode(this->root.get());

}

void MyGL::resizeGL(int w, int h)
{
    glm::mat3 viewMat = glm::scale(glm::mat3(), glm::vec2(0.2, 0.2)); // Screen is -5 to 5

    // Upload the view matrix to our shader (i.e. onto the graphics card)
    prog_flat.setViewMatrix(viewMat);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_showGrid)
    {
        prog_flat.setModelMatrix(glm::mat3());
        prog_flat.draw(*this, m_geomGrid);
    }

    //VVV CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL VVV///////////////////

    // Shapes will be drawn on top of one another, so the last object
    // drawn will appear in front of everything else
/*
    prog_flat.setModelMatrix(glm::mat3());
    m_geomSquare.setColor(glm::vec3(0,1,0));
    prog_flat.draw(*this, m_geomSquare);

    m_geomSquare.setColor(glm::vec3(1,0,0));
    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(-1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(-30.f)));
    prog_flat.draw(*this, m_geomSquare);

    m_geomSquare.setColor(glm::vec3(0,0,1));
    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(30.f)));
    prog_flat.draw(*this, m_geomSquare);

*/
    //^^^ CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL ^^^/////////////////

    MyGL::traverse((this->root).get(), glm::mat3());


    // Here is a good spot to call your scene graph traversal function.
    // Any time you want to draw an instance of geometry, call
    // prog_flat.draw(*this, yourNonPointerGeometry);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    switch(e->key())
    {
    case(Qt::Key_Escape):
        QApplication::quit();
        break;

    case(Qt::Key_G):
        m_showGrid = !m_showGrid;
        break;
    }
}

void MyGL::slot_setSelectedNode(QTreeWidgetItem *i) {
    mp_selectedNode = static_cast<Node*>(i);
}

void MyGL::slot_setTX(double x) {
    // TODO update the currently selected Node's
    // X translation value IF AND ONLY IF
    // the currently selected node can be validly
    // dynamic_cast to a TranslateNode.
    // Remember that a failed dynamic_cast
    // will return a null pointer.
    //dynamic_cast<TranslateNode>(mp_selectedNode);
    TranslateNode* curr = dynamic_cast<TranslateNode*>(mp_selectedNode);
    if(curr == nullptr) return;
    curr->setXt(x);
}

void MyGL::slot_setTY(double x) {
    TranslateNode* curr = dynamic_cast<TranslateNode*>(mp_selectedNode);
    if(curr == nullptr) return;
    curr->setYt(x);
}

void MyGL::slot_setScX(double x) {
    ScaleNode* curr = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if(curr == nullptr) return;
    curr->setXsc(x);
}

void MyGL::slot_setScY(double x) {
    ScaleNode* curr = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if(curr == nullptr) return;
    curr->setYsc(x);
}

void MyGL::slot_setAng(double x) {
    RotateNode* curr = dynamic_cast<RotateNode*>(mp_selectedNode);
    if(curr == nullptr) return;
    curr->setAngle(x);
}

void MyGL::slot_addTranslateNode() {
    // TODO invoke the currently selected Node's
    // addChild function on a newly-instantiated
    // TranslateNode.
    if(mp_selectedNode == nullptr) return;
    mp_selectedNode->addChild(mkU<TranslateNode>(1.0f, 1.0f, QString("New Translate Node"), glm::vec3(.5, .5, .5), nullptr));
}

void MyGL::slot_addScaleNode() {
    if(mp_selectedNode == nullptr) return;
    mp_selectedNode->addChild(mkU<ScaleNode>(1.0f, 1.0f, QString("New Scale Node"), glm::vec3(.5, .5, .5), nullptr));
}

void MyGL::slot_addRotateNode() {
    if(mp_selectedNode == nullptr) return;
    mp_selectedNode->addChild(mkU<RotateNode>(0.0f, QString("New Rotate Node"), glm::vec3(.5, .5, .5), nullptr));
}

void MyGL::slot_addShape() {
    if(mp_selectedNode == nullptr) return;
    if(mp_selectedNode->getPolygon() != nullptr) return;
    mp_selectedNode->setPolygon(&m_geomSquare);
}
