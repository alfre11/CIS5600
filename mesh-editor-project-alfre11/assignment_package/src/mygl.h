#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include<mesh.h>
#include<vertex_display.h>
#include<face_display.h>
#include<halfedge_display.h>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    QTimer timer;
    float currTime;

    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_camera;
    // A variable used to track the mouse's previous position when
    // clicking and dragging on the GL viewport. Used to move the camera
    // in the scene.
    glm::vec2 m_mousePosPrev;

    mesh my_mesh;
    bool start_mesh;
    bool end_mesh;

    vertex_display dV;
    halfedge_display dH;
    face_display dF;

    vertex* selected_Vertex;
    halfedge* selected_Halfedge;
    face* selected_Face;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    std::unordered_map<face*, vertex*> centroidMap;

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void sendToWorld();

    void updateVertX(float f);
    void updateVertY(float f);
    void updateVertZ(float f);

    void updateFaceRed(float f);
    void updateFaceGreen(float f);
    void updateFaceBlue(float f);

    void findCentroid(face* f);
    void smooth_split(halfedge* h);
    void smoothVert(vertex* v);
    void quadrang(face* f);
    void reverseCycle(halfedge* start);



protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

    void triang();
    void addV();
    std::vector<halfedge*>copy_e();
    std::vector<vertex*>copy_v();
    std::vector<face*>copy_f();


    void quadrangulate(face*);

signals:
    void s_vertex(vertex*);
    void s_edge(halfedge*);
    void s_face(face*);

    void select_vertex(vertex*);
    void select_halfedge(halfedge*);
    void select_face(face*);


public slots:
    void tick();
    void load_obj();

    void selected_vertex(QListWidgetItem*);
    void selected_halfedge(QListWidgetItem*);
    void selected_face(QListWidgetItem*);
    void subdivide();

    void triangSlot();
    void addVSlot();
};


#endif // MYGL_H
