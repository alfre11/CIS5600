#include "mygl.h"
#include <la.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include<QFileDialog>
#include <fstream>
#include <unordered_set>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      timer(), currTime(0.),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this),
      vao(),
      m_camera(width(), height()),
      m_mousePosPrev(),
        my_mesh(this),
        start_mesh(false),
        end_mesh(false),
    dV(this),
    dH(this),
    dF(this),
    selected_Vertex(nullptr),
    selected_Halfedge(nullptr),
    selected_Face(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}



//selecting a vertex
void MyGL::selected_vertex(QListWidgetItem* i) {
    vertex* v = nullptr;
    if (i==nullptr) {
        return;
    } else {
        v = dynamic_cast<vertex*>(i);
    }
    if (v == nullptr) {
        return;
    }

    select_vertex(v);

    select_face(nullptr);
    selected_Face = nullptr;
    select_halfedge(nullptr);
    selected_Halfedge = nullptr;

    if (!v) {
        return;
    } else {
        selected_Vertex = v;
        dV.updateVertex(v);
        update();
    }
}

//selecting a half edge
void MyGL::selected_halfedge(QListWidgetItem* i) {
    halfedge* h = nullptr;
    if (i==nullptr) {
        return;
    } else {
        h = dynamic_cast<halfedge*>(i);
    }
    if (h == nullptr) {
        return;
    }

    selected_Face = nullptr;
    select_vertex(nullptr);
    selected_Vertex = nullptr;
    select_face(nullptr);

    if (!h) {
        return;
    } else {
        selected_Halfedge = h;
        dH.updateHalfEdge(h);
        update();
    }
}


//selecting a face
void MyGL::selected_face(QListWidgetItem* i) {
    face* f = nullptr;
    if (i==nullptr) {
        return;
    } else {
        f = dynamic_cast<face*>(i);
    }
    if (f == nullptr) {
        return;
    }

    select_face(f);
    select_halfedge(nullptr);
    selected_Halfedge = nullptr;
    select_vertex(nullptr);
    selected_Vertex = nullptr;

    if (!f) {
        return;
    } else {
        selected_Face = f;
        dF.updateFace(f);
        update();
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
    glEnable(GL_DEPTH_TEST);
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

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.initializeAndBufferGeometryData();

    // Create and set up the diffuse shader
    m_progLambert.createAndCompileShaderProgram("lambert.vert.glsl", "lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.createAndCompileShaderProgram("flat.vert.glsl", "flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_camera.recomputeAspectRatio(w, h);

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);

    printGLErrorLog();
}

void MyGL::sendToWorld() {
    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifVec3("u_CamPos", m_camera.eye);
    m_progFlat.setUnifMat4("u_Model", glm::mat4(1.f));
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    sendToWorld();

    if (start_mesh == true) {
        if (end_mesh == true) {
            glm::mat4 model;
            m_progLambert.setUnifMat4("u_Model", model);
            m_progLambert.setUnifMat4("u_ModelInvTr", model);
            m_progLambert.draw(my_mesh);


            glDisable(GL_DEPTH_TEST);  // Disable depth testing to draw selected components on top

            if (selected_Vertex != nullptr) {

                m_progFlat.draw(dV);
            }
            if (selected_Halfedge != nullptr) {
                m_progFlat.draw(dH);
            }
            if (selected_Face != nullptr) {
                m_progFlat.draw(dF);
            }

            glEnable(GL_DEPTH_TEST);
        }
    }

    if ((start_mesh && end_mesh) == false){
        //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
        //Note that we have to transpose the model matrix before passing it to the shader
        //This is because OpenGL expects column-major matrices, but you've
        //implemented row-major matrices.
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
        //Send the geometry's transformation matrix to the shader
        m_progLambert.setUnifMat4("u_Model", model);
        m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
        //Draw the example sphere using our lambert shader
        m_progLambert.draw(m_geomSquare);

        //Now do the same to render the cylinder
        //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
        m_progLambert.setUnifMat4("u_Model", model);
        m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
        m_progLambert.draw(m_geomSquare);
    }
}


//Function to process an obj and load it into our mesh
void MyGL::load_obj() {
    start_mesh = true;
    QString picked_file = QFileDialog::getOpenFileName(nullptr, "Select 3D OBJ File", "", "OBJ Files (*.obj)");
    if (picked_file.isEmpty()) {
        return;
    }
    std::ifstream file(picked_file.toStdString());
    if (!picked_file.isEmpty()) {
        if(!file.is_open()) {
            return;
        }
    }

    std::vector<glm::vec3> vertices;
    std::vector<std::vector<int>> face_indices;



    //read the OBJ
    std::string read_in;
    while (std::getline(file, read_in)) {
        std::stringstream s(read_in);

        std::string letter;
        s >> letter;

        if (letter == "v") {
            glm::vec3 vertex;
            s >> vertex.x;
            s >> vertex.y;
            s >> vertex.z;
            vertices.push_back(vertex);
        }
        if (letter == "f") {
            std::vector<int> ind;
            std::string ind_string;
            while (s >> ind_string) {
                auto find = ind_string.find('/');
                int vertex_index = std::stoi(ind_string.substr(0, find));
                vertex_index -= 1;
                ind.push_back(vertex_index);
            }
            face_indices.push_back(ind);
        }
    }


    //set up the list
    my_mesh.face_list.clear();
    my_mesh.hedge_list.clear();
    my_mesh.vertex_list.clear();

    //populate
    std::map<std::pair<vertex*, vertex*>, halfedge*> vertsToHEs;
    for (const auto &pos : vertices) {
        my_mesh.vertex_list.push_back(mkU<vertex>(pos));
    }

    //generate a randome color
    for (const auto &fa : face_indices) {
        //color
        float h = static_cast<float>(std::rand() % 360);
        float s = 1.0f;
        float l = 0.5f;

        float c = (1 - std::abs(2 * l - 1)) * s;
        float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
        float m = l - c / 2;

        float r = 0, g = 0, b = 0;
        if (0 <= h && h < 60) {
            r = c; g = x; b = 0;
        } else if (60 <= h && h < 120) {
            r = x; g = c; b = 0;
        } else if (120 <= h && h < 180) {
            r = 0; g = c; b = x;
        } else if (180 <= h && h < 240) {
            r = 0; g = x; b = c;
        } else if (240 <= h && h < 300) {
            r = x; g = 0; b = c;
        } else if (300 <= h && h < 360) {
            r = c; g = 0; b = x;
        }

        glm::vec3 random_color = glm::vec3(r + m, g + m, b + m);

        auto newFace = mkU<face>(random_color);
        std::vector<halfedge*> faceHalfEdges;

        halfedge* firstHe = nullptr;
        halfedge* prevHe = nullptr;

        //looop through the faces
        for (size_t i = 0; i < fa.size(); ++i) {

            auto newHalfEdge = mkU<halfedge>();

            vertex* v = my_mesh.vertex_list[fa[i]].get();
            newHalfEdge->vertex = v;

            halfedge* h = newHalfEdge.get();
            newHalfEdge->vertex->halfedge =  h;

            face* f = newFace.get();
            newHalfEdge->face = f;

            int prevInd = (i + fa.size() - 1) % fa.size();

            vertex* p = my_mesh.vertex_list[fa[prevInd]].get();

            std::pair<vertex*, vertex*> currP(std::min(v, p), std::max(v, p));

            //syummetry
            if(vertsToHEs.contains(currP)) {
                newHalfEdge.get()->SYM = vertsToHEs.at(currP);
                vertsToHEs.at(currP)->SYM = newHalfEdge.get();
            } else {
                vertsToHEs.insert({currP, newHalfEdge.get()});
            }

            faceHalfEdges.push_back(newHalfEdge.get());

            v->halfedge = newHalfEdge.get();

            //linking
            if(i ==0){
                firstHe = newHalfEdge.get();
            } else {
                prevHe->NXT = newHalfEdge.get();
            }
            prevHe = h;
            my_mesh.hedge_list.push_back(std::move(newHalfEdge));
        }


        //finish it
        prevHe->NXT = firstHe;
        newFace->halfedge = firstHe;

        size_t size = faceHalfEdges.size();
        for (size_t i = 0; i < size; ++i) {
            unsigned int ind = (i + 1) % size;
            faceHalfEdges[i]->NXT = faceHalfEdges[ind];
        }

        newFace->halfedge = faceHalfEdges[0];
        my_mesh.face_list.push_back(std::move(newFace));
    }


    for (auto &h : my_mesh.hedge_list) {
        for (auto &o : my_mesh.hedge_list) {
            if (h->vertex == o->NXT->vertex &&
                h->NXT->vertex == o->vertex) {
                auto* a = o.get();
                h->SYM = a;

                auto* b = h.get();
                o->SYM = b;
                break;
            } else {
                continue;
            }
        }
    }

    //flip all the edges around
    for (auto &face : my_mesh.face_list) {
        reverseCycle(face.get()->halfedge);
    }


    //buffer set up and pointer init
    my_mesh.initializeAndBufferGeometryData();
    for (auto &face : my_mesh.face_list) s_face(face.get());
    for (auto &halfEdge : my_mesh.hedge_list) s_edge(halfEdge.get());
    for (auto &vertex : my_mesh.vertex_list) s_vertex(vertex.get());
    selected_Face = nullptr;
    selected_Halfedge = nullptr;
    selected_Vertex = nullptr;
    select_face(nullptr);
    select_halfedge(nullptr);
    select_vertex(nullptr);
    end_mesh = true;
}



//Code for viisual debugging
void MyGL::keyPressEvent(QKeyEvent *e) {
    if(e->modifiers() & Qt::ShiftModifier) {
        if(e->key() == Qt::Key_H) {
            if (selected_Face != nullptr) {
                halfedge* n = selected_Face->halfedge;
                select_face(nullptr);
                select_halfedge(n);
                select_vertex(nullptr);
                selected_Vertex = nullptr;
                selected_Halfedge = nullptr;
                selected_Face = nullptr;
                selected_Halfedge = n;
                dH.updateHalfEdge(selected_Halfedge);
                update();
            }
        }
    } else {
        switch (e->key()) {
        case Qt::Key_N:
            if (selected_Halfedge) {
                halfedge* n = selected_Halfedge->NXT;
                select_face(nullptr);
                select_halfedge(n);
                select_vertex(nullptr);
                selected_Vertex = nullptr;
                selected_Face = nullptr;
                selected_Halfedge = n;
                dH.updateHalfEdge(selected_Halfedge);
                update();
            }
            break;
        case Qt::Key_M:
            if (selected_Halfedge ) {
                if (selected_Halfedge->SYM) {
                    halfedge* n = selected_Halfedge->SYM;
                    select_face(nullptr);
                    select_halfedge(n);
                    select_vertex(nullptr);
                    selected_Vertex = nullptr;
                    selected_Face = nullptr;
                    selected_Halfedge = n;
                    dH.updateHalfEdge(selected_Halfedge);
                    update();
                }
            }
            break;
        case Qt::Key_F:
            if (selected_Halfedge) {
                face* n = selected_Halfedge->face;
                select_face(n);
                select_halfedge(nullptr);
                select_vertex(nullptr);
                selected_Vertex = nullptr;
                selected_Halfedge = nullptr;
                selected_Face = n;
                dF.updateFace(selected_Face);
                update();
            }
            break;
        case Qt::Key_V:
            if (selected_Halfedge) {
                vertex* n = selected_Halfedge->vertex;
                select_face(nullptr);
                select_halfedge(nullptr);
                select_vertex(n);
                selected_Face = nullptr;
                selected_Halfedge = nullptr;
                selected_Vertex = n;
                dV.updateVertex(selected_Vertex);
                update();
            }
            break;
        case Qt::Key_H:
            if (selected_Vertex) {
                if (selected_Vertex->halfedge) {
                    halfedge* n = selected_Vertex->halfedge;
                    select_face(nullptr);
                    select_halfedge(n);
                    select_vertex(nullptr);
                    selected_Face = nullptr;
                    selected_Vertex = nullptr;
                    selected_Halfedge = n;
                    dH.updateHalfEdge(selected_Halfedge);
                    update();
                }
            }
            break;
            }
    }
}


void MyGL::mousePressEvent(QMouseEvent *e) {
    if(e->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        m_mousePosPrev = glm::vec2(e->pos().x(), e->pos().y());
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    glm::vec2 pos(e->pos().x(), e->pos().y());
    if(e->buttons() & Qt::LeftButton)
    {
        // Rotation
        glm::vec2 diff = 0.2f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.RotateAboutGlobalUp(-diff.x);
        m_camera.RotateAboutLocalRight(-diff.y);
    }
    else if(e->buttons() & Qt::RightButton)
    {
        // Panning
        glm::vec2 diff = 0.05f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.PanAlongRight(-diff.x);
        m_camera.PanAlongUp(diff.y);
    }
}

void MyGL::wheelEvent(QWheelEvent *e) {
    m_camera.Zoom(e->angleDelta().y() * 0.001f);
}

void MyGL::tick() {
    ++currTime;
    update();
}

void MyGL::triangSlot() {
    triang();
    update();
}



//functiont to count the half edges
int countHes(halfedge* h) {
    halfedge* curr = h;
    int count = 0;
    do {
        count++;
        curr = curr->NXT;
    } while(curr != h);

    return count;
}

//helper for generating random colors
glm::vec3 randomColor() {
    float h = static_cast<float>(std::rand() % 360);
    float s = 1.0f;
    float l = 0.5f;

    float c = (1 - std::abs(2 * l - 1)) * s;
    float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
    float m = l - c / 2;

    float r = 0, g = 0, b = 0;
    if (0 <= h && h < 60) {
        r = c; g = x; b = 0;
    } else if (60 <= h && h < 120) {
        r = x; g = c; b = 0;
    } else if (120 <= h && h < 180) {
        r = 0; g = c; b = x;
    } else if (180 <= h && h < 240) {
        r = 0; g = x; b = c;
    } else if (240 <= h && h < 300) {
        r = x; g = 0; b = c;
    } else if (300 <= h && h < 360) {
        r = c; g = 0; b = x;
    }

    glm::vec3 random_color = glm::vec3(r + m, g + m, b + m);
    return random_color;
}

//function for triangulating the faces
void MyGL::triang() {
    if(selected_Face == nullptr) {
        return;
    }

    int HeCount = 0;
    HeCount = countHes(selected_Face->halfedge);

    //make sure that we have the correct sizing
    if(HeCount < 4) {
        return;
    }

    std::vector<uPtr<halfedge>> edges;
    std::vector<uPtr<face>> faces;


    //keep triangulting
    while(HeCount > 3) {
        //Triangulating algorithm - (from class)
        halfedge* he1 = new halfedge();
        halfedge* he2 = new halfedge();

        face* f1 = new face(randomColor());
        face* sf = selected_Face;

        halfedge* start = sf->halfedge;
        start->NXT->face = f1;
        start->NXT->NXT->face = f1;
        he1->face = sf;
        f1->halfedge = he1;
        he2->face = f1;
        he2->NXT = start->NXT->NXT->NXT;
        start->NXT->NXT->NXT = he1;
        he1->NXT = start->NXT;
        start->NXT = he2;
        he2->face = sf;
        he1->SYM = he2;
        he2->SYM = he1;
        he1->vertex = start->vertex;
        he2->vertex = he1->NXT->NXT->vertex;

        auto upf1 = uPtr<face>(f1);
        auto uphe1 = uPtr<halfedge>(he1);
        auto uphe2 = uPtr<halfedge>(he2);

        edges.push_back(std::move(uphe1));
        edges.push_back(std::move(uphe2));
        faces.push_back(std::move(upf1));

        selected_Face = sf;
        HeCount = countHes(selected_Face->halfedge);
    }

    //add components back in
    for(auto& face : faces) {
        s_face(face.get());
        my_mesh.face_list.push_back(std::move(face));
    }

    for(auto& h : edges) {
        s_edge(h.get());
        my_mesh.hedge_list.push_back(std::move(h));
    }
    my_mesh.initializeAndBufferGeometryData();
    update();
}

void MyGL::addVSlot() {
    addV();
}


//adds the vertex
void MyGL::addV(){
    glm::vec3 midpoint = 0.5f * (selected_Halfedge->vertex->vertex_position + selected_Halfedge->SYM->vertex->vertex_position);
    std::unique_ptr<vertex> newVertex = std::make_unique<vertex>(midpoint);
    vertex* v = newVertex.get();


    //additng edge algorithm (From class)
    // 2) creates two new half edges
    halfedge* he_A = selected_Halfedge;
    halfedge* he_B = he_A->SYM;

    std::unique_ptr<halfedge> he_A2 = std::make_unique<halfedge>();
    std::unique_ptr<halfedge> he_B2 = std::make_unique<halfedge>();
    halfedge* heA2 = he_A2.get();
    halfedge* heB2 = he_B2.get();

    // 2a) gives vertex, next, sym and face pointer to he_A2, B2
    he_A2->vertex = he_A->vertex;
    he_A2->NXT = he_A->NXT;
    he_A2->SYM = he_B;
    he_B->SYM = he_A2.get();
    he_A2->face = he_A->face;

    he_B2->vertex = he_B->vertex;
    he_B2->NXT = he_B->NXT;
    he_B2->SYM = he_A;
    he_A->SYM = he_B2.get();
    he_B2->face = he_B->face;

    // 3) adjusts next and vertex pointer of he_A, B
    he_A->NXT = he_A2.get();
    he_A->vertex = v;

    he_B->NXT = he_B2.get();
    he_B->vertex = v;

    // stores new elements
    my_mesh.vertex_list.push_back(std::move(newVertex));
    my_mesh.hedge_list.push_back(std::move(he_A2));
    my_mesh.hedge_list.push_back(std::move(he_B2));

    // signals updates
    s_vertex(v);
    s_edge(heA2);
    s_edge(heB2);

    // updates display
    select_halfedge(he_A);
    dH.updateHalfEdge(he_A);

    update(); // repaints
}



//updates vertex X
void MyGL::updateVertX(float f) {
    if(selected_Vertex != nullptr) {
        selected_Vertex->vertex_position[0] = static_cast<float>(f);
        dV.updateVertex(selected_Vertex);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}
//updates vertex Y
void MyGL::updateVertY(float f) {
    if(selected_Vertex != nullptr) {
        selected_Vertex->vertex_position[1] = static_cast<float>(f);
        dV.updateVertex(selected_Vertex);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}

//updates vertex Z
void MyGL::updateVertZ(float f) {
    if(selected_Vertex != nullptr) {
        selected_Vertex->vertex_position[2] = static_cast<float>(f);
        dV.updateVertex(selected_Vertex);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}


//face change red
void MyGL::updateFaceRed(float f) {
    if(selected_Face != nullptr) {
        selected_Face->face_color[0] = static_cast<float>(f);
        dF.updateFace(selected_Face);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}

//face change green
void MyGL::updateFaceGreen(float f) {
    if(selected_Face != nullptr) {
        selected_Face->face_color[1] = static_cast<float>(f);
        dF.updateFace(selected_Face);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}

//face change blue
void MyGL::updateFaceBlue(float f) {
    if(selected_Face != nullptr) {
        selected_Face->face_color[2] = static_cast<float>(f);
        dF.updateFace(selected_Face);
        my_mesh.initializeAndBufferGeometryData();
        update();
    }
}




//helper fucntions for copying our unique pointer lists and convert them into raw pointer lists
std::vector<halfedge*>  MyGL::copy_e() {
    std::vector<halfedge*> b;
    for(const auto& a : my_mesh.hedge_list) {
        b.push_back(a.get());
    }
    return b;
}
std::vector<vertex*>  MyGL::copy_v() {
    std::vector<vertex*> b;
    for(const auto& a : my_mesh.vertex_list) {
        b.push_back(a.get());
    }
    return b;
}
std::vector<face*>  MyGL::copy_f(){
    std::vector<face*> b;
    for(const auto& a : my_mesh.face_list) {
        b.push_back(a.get());
    }
    return b;
}



//helper functiont to scan for a value
bool has(std::vector<halfedge*>& a, halfedge* b) {
    for (halfedge* possible : a) {
        if(possible == b) return true;
    }
    return false;
}

//function to reverse teh edges of a half edge
void MyGL::reverseCycle(halfedge* start) {
    if (start == nullptr) return;
    halfedge* current = start;
    halfedge* prev = nullptr;
    halfedge* next = nullptr;
    do {
        next = current->NXT;  // Save the next node
        current->NXT = prev;   // Reverse the edge
        prev = current;         // Move prev to current
        current = next;         // Move to the next node
    } while (current != start);
    start->NXT = prev;
}



//mega function for the splitting
void MyGL::subdivide() {
    std::vector<halfedge*> firstHe = copy_e();
    std::vector<face*> firstFaces =  copy_f();
    std::vector<vertex*> firstVerts = copy_v();

    //midpoint map
    std::map<halfedge*, vertex*> mapMidpoints;

    //STEP 1: CENTROIDS
    for(const auto& face : firstFaces) {
        findCentroid(face);
    }


    //STEP TWO MIDPOINTS
    std::unordered_set<halfedge*> split;
    for(const auto& he : firstHe) {
        if(!(split.contains(he->SYM)) && has(firstHe, he->SYM)) {
            split.insert(he);
            split.insert(he->SYM);
            smooth_split(he);
        }
    }

    //STEP 3: ADJUST ORTIGINAL VERS
    for(const auto& v : firstVerts) {
       smoothVert(v);
    }

    //STEP 4: SPLIT FACE UP
    for(const auto& f : firstFaces) {
       quadrang(f);
    }

    my_mesh.initializeAndBufferGeometryData();
    update();
    std::unordered_map<face*, vertex*> newCentroidMap;
    centroidMap = newCentroidMap; //make the centroid map fresh
}

//helper for making centroids
void MyGL::findCentroid(face* f) {
    if(f != nullptr) {
        std::vector<vertex*> listOfVerts;
        halfedge* curr = f->halfedge;
        halfedge* first = curr;
        do {
            listOfVerts.push_back(curr->vertex);
            curr = curr->NXT;
        } while(curr != first);

        glm::vec3 newCoords = glm::vec3();
        for(vertex* vert : listOfVerts) {
            newCoords += vert->vertex_position;
        }

        newCoords /= listOfVerts.size();
        vertex* newV = new vertex(newCoords);
        //signal the vertex to exist
        s_vertex(newV);
        my_mesh.vertex_list.push_back(std::unique_ptr<vertex>(newV));
        centroidMap.insert({f, newV});
    }
}


void MyGL::smooth_split(halfedge* h) {
    if (h != nullptr) {
        vertex* vert1 = h->vertex;
        vertex* vert2 = h->SYM->vertex;
        glm::vec3 p1 = vert1->vertex_position;
        glm::vec3 p2 = vert2->vertex_position;
        glm::vec3 cent1 = centroidMap[h->face]->vertex_position;
        glm::vec3 cent2 = centroidMap[h->SYM->face]->vertex_position;
        glm::vec3 midpoint = (p1 + p2 + cent1 + cent2) / 4.0f;
        std::unique_ptr<vertex> newVertex = std::make_unique<vertex>(midpoint);
        vertex* v = newVertex.get();

        // original half edges
        halfedge* he_A = h;
        halfedge* he_B = h->SYM;

        // makes two new half edges for us to stitch it together!
        std::unique_ptr<halfedge> he_A2 = std::make_unique<halfedge>();
        std::unique_ptr<halfedge> he_B2 = std::make_unique<halfedge>();
        halfedge* heA2 = he_A2.get();
        halfedge* heB2 = he_B2.get();


        // 2a) gives vertex, next, sym and face pointer to he_A2, B2
        he_A2->vertex = he_A->vertex;
        he_A2->NXT = he_A->NXT;
        he_A2->SYM = he_B;
        he_B->SYM = he_A2.get();
        he_A2->face = he_A->face;

        he_B2->vertex = he_B->vertex;
        he_B2->NXT = he_B->NXT;
        he_B2->SYM = he_A;
        he_A->SYM = he_B2.get();
        he_B2->face = he_B->face;

        // 3) adjusts next and vertex pointer of he_A, B
        he_A->NXT = he_A2.get();
        he_A->vertex = v;

        he_B->NXT = he_B2.get();
        he_B->vertex = v;
        he_A2->vertex = vert1;
        he_B2->vertex = vert2;

        //vert -> halfedge
        vert1->halfedge = he_B;
        vert2->halfedge = he_A;
        v->halfedge = he_A;

        // stores new elements
        my_mesh.vertex_list.push_back(std::move(newVertex));
        my_mesh.hedge_list.push_back(std::move(he_A2));
        my_mesh.hedge_list.push_back(std::move(he_B2));

        // signals updates
        s_vertex(v);
        s_edge(heA2);
        s_edge(heB2);
    }
}


//smooth out the originjal vertexs
void MyGL::smoothVert(vertex* v) {
    halfedge* first = v->halfedge;
    halfedge* curr = first;
    glm::vec3 sumCents = glm::vec3();
    glm::vec3 sum2 = glm::vec3();
    float c = 0.0f;
    do {
        sumCents += centroidMap[curr->face]->vertex_position;
        sum2 += curr->vertex->vertex_position;
        curr = curr->SYM->NXT;
        c++;
    } while(curr != first);
    //calcualte the new  vertex positions
    glm::vec3 newCoord = (((c-2)*(v->vertex_position))/(c)) + ((sum2)/(c*c)) + ((sumCents)/(c*c));
    v->vertex_position = newCoord;
    selected_Vertex = v;
    selected_Halfedge = nullptr;
    selected_Face = nullptr;
    dV.updateVertex(selected_Vertex);
    my_mesh.initializeAndBufferGeometryData();
    update();
}


//Funciton to quadtrangulate
void MyGL::quadrang(face* f) {
    std::vector<halfedge*> generatedHalfEdges;
    std::vector<face*> generatedFaces;

    //set up the pointers
    halfedge* outgoingHE = new halfedge();
    halfedge* incomingHE = new halfedge();

    //set up
    vertex* centroidVertex = centroidMap[f];
    halfedge* startingHE = f->halfedge->NXT;
    halfedge* nextHE = startingHE->NXT;
    vertex* primaryVertex = nextHE->vertex;

    halfedge* currentHE = nextHE->NXT;
    halfedge* followingHE = currentHE->NXT->NXT;
    vertex* adjacentVertex = currentHE->NXT->vertex;

    //centorid edges
    outgoingHE->vertex = primaryVertex;
    incomingHE->vertex = centroidVertex;

    //linking
    outgoingHE->face = f;
    incomingHE->face = f;
    outgoingHE->NXT = currentHE;
    incomingHE->NXT = outgoingHE;
    currentHE->NXT->NXT = incomingHE;

    //match pointers
    halfedge* firstLink = outgoingHE;
    halfedge* finalLink = nullptr;
    halfedge* pendingSymmetry = incomingHE;
    f->halfedge = incomingHE;
    primaryVertex = adjacentVertex;
    halfedge* loopHE = followingHE;
    generatedHalfEdges.push_back(incomingHE);
    generatedHalfEdges.push_back(outgoingHE);

    do {
        //random color
        glm::vec3 a = randomColor();
        face* subdividedFace = new face(a);

        followingHE = loopHE->NXT->NXT;
        adjacentVertex = loopHE->NXT->vertex;

        //set up the centroid edges
        halfedge* outboundHE = new halfedge();
        halfedge* inboundHE = new halfedge();
        outboundHE->NXT = loopHE;
        inboundHE->NXT = outboundHE;
        loopHE->NXT->NXT = inboundHE;
        outboundHE->vertex = primaryVertex;
        inboundHE->vertex = centroidVertex;
        outboundHE->face = subdividedFace;
        inboundHE->face = subdividedFace;
        loopHE->face = subdividedFace;
        loopHE->NXT->face = subdividedFace;
        subdividedFace->halfedge = inboundHE;
        outboundHE->SYM = pendingSymmetry;
        pendingSymmetry->SYM = outboundHE;
        pendingSymmetry = inboundHE;
        generatedFaces.push_back(subdividedFace);
        generatedHalfEdges.push_back(inboundHE);
        generatedHalfEdges.push_back(outboundHE);

        //set for next loop
        finalLink = pendingSymmetry;
        primaryVertex = adjacentVertex;
        loopHE = followingHE;

    } while (followingHE != currentHE);

    //finish it up
    finalLink->SYM = firstLink;
    firstLink->SYM = finalLink;

    //update mesh
    for (auto edge : generatedHalfEdges) {
        my_mesh.hedge_list.push_back(std::unique_ptr<halfedge>(edge));
        s_edge(edge);
    }
    for (auto b : generatedFaces) {
        my_mesh.face_list.push_back(std::unique_ptr<face>(b));
        s_face(b);
    }
}

