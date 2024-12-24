#include "mygl.h"
#include <QDir>
#include <string.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>


MyGL::MyGL(QWidget *parent)
    : QOpenGLWidget(parent),
    timer(), currTime(0.f), position(0.0f, 0.0f), velocity(0.1f, 0.1f),
      vao(),
      vertShader(), fragShader(), shaderProgram(),
      shaderAttribVariableHandles(), shaderUniformVariableHandles(),
      bufferPosition(), bufferIndex(),
      indexBufferLength(-1)
{
    // Allow Qt to trigger mouse events
    // even when a mouse button is not held.
    setMouseTracking(true);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
}


MyGL::~MyGL(){}

// As noted in mygl.h, use this function in order to get
// the direct file path to this Qt project so that you
// can read the contents of your GLSL files.
QString MyGL::getCurrentPath() const {
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

// A helper function for createAndCompileShaderProgram.
// It reads the contents of a file into a char*.
char* textFileRead(const char* fileName) {
    char* text = nullptr;

    if (fileName != nullptr) {
        FILE *file = fopen(fileName, "rt");

        if (file != nullptr) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol
            }
            fclose(file);
        }
    }
    return text;
}

void MyGL::createAndCompileShaderProgram(GLuint &vertHandle,
                              GLuint &fragHandle,
                              GLuint &shaderProgramHandle,
                              QString vertFile,
                              QString fragFile) {

    // Set up the direct filepath to the vertex and fragment
    // shader files.
    QString projectPath = getCurrentPath();
    projectPath.append("/glsl/");
    QString vertPath = projectPath + vertFile;
    QString fragPath = projectPath + fragFile;

    // Make the OpenGL function calls that create a
    // vertex shader object, fragment shader object,
    // and shader program object on the GPU. We store
    // CPU-side handles to these GPU-side objects in
    // the GLuints passed into this function.
    vertHandle = glCreateShader(GL_VERTEX_SHADER);
    fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgramHandle = glCreateProgram();

    // Parse the plain-text contents of the vertex
    // and fragment shader files, storing them in C-style
    // char* strings (how OpenGL expects strings to be formatted).
    char *vertexShaderSource = textFileRead(vertPath.toStdString().c_str());
    char *fragmentShaderSource = textFileRead(fragPath.toStdString().c_str());

    // Send the contents of the shader files to the GPU
    glShaderSource(vertHandle, 1, &vertexShaderSource, 0);
    glShaderSource(fragHandle, 1, &fragmentShaderSource, 0);
    // Manually de-allocate the heap memory used to store the
    // shader contents. We don't need it now that it's been sent
    // to the GPU.
    free(vertexShaderSource);
    free(fragmentShaderSource);

    // Tell OpenGL on the GPU to try compiling the shader code
    // we just sent.
    glCompileShader(vertHandle);
    glCompileShader(fragHandle);
    // Check if everything compiled OK. If not, print out
    // any errors sent to us from the GPU-side shader compiler.
    GLint compiled;
    glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        std::cout << "Errors from " << vertFile.toStdString() << ":" << std::endl;
        printShaderInfoLog(vertHandle);
    }
    glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        std::cout << "Errors from " << fragFile.toStdString() << ":" << std::endl;
        printShaderInfoLog(fragHandle);
    }

    // Tell shaderProgramHandle that it manages
    // these particular vertex and fragment shaders
    glAttachShader(shaderProgramHandle, vertHandle);
    glAttachShader(shaderProgramHandle, fragHandle);
    glLinkProgram(shaderProgramHandle);

    // Check for linking success
    GLint linked;
    glGetProgramiv(shaderProgramHandle, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(shaderProgramHandle);
    }
}

void MyGL::getHandlesForShaderVariables(GLuint shaderProgramHandle) {
    glUseProgram(shaderProgramHandle);

    // TODO: Write code that gets the handle IDs for each "in"
    // and "uniform" variable used in your shader program,
    // and put them in shaderAttribVariableHandles and
    // shaderUniformVariableHandles. The key for each ID handle
    // should be the string representation of its name, e.g.
    // "vs_Pos" or "u_ScreenDimensions", etc.
    // You will need to make use of the following OpenGL API calls:
    // - glGetUniformLocation
    // - glGetAttribLocation

    GLuint attribLocation = glGetAttribLocation(shaderProgramHandle, "vs_Pos");
    if (attribLocation != (GLuint) -1) {
        shaderAttribVariableHandles["vs_Pos"] = attribLocation;
    } else {
        std::cerr << "Warning: Attribute vs_Pos not found in shader program." << std::endl;
    }

    GLuint uniformLocation = glGetUniformLocation(shaderProgramHandle, "u_ScreenDimensions");
    if (uniformLocation != (GLuint) -1) {
        shaderUniformVariableHandles["u_ScreenDimensions"] = uniformLocation;
    } else {
        std::cerr << "Warning: Uniform u_ScreenDimensions not found in shader program." << std::endl;
    }

    GLuint uniformModel = glGetUniformLocation(shaderProgramHandle, "u_Model");
    if (uniformModel != (GLuint) -1) {
        shaderUniformVariableHandles["u_Model"] = uniformModel;
    } else {
        std::cerr << "Warning: Uniform u_Model not found in shader program." << std::endl;
    }

    GLuint uniformTime = glGetUniformLocation(shaderProgramHandle, "u_Time");
    if (uniformTime != (GLuint) -1) {
        shaderUniformVariableHandles["u_Time"] = uniformTime;
    } else {
        std::cerr << "Warning: Uniform u_Time not found in shader program." << std::endl;
    }



}

void MyGL::initializeAndBufferGeometryData() {
    // TODO: Populate these two std::vectors
    // with data representing the vertex positions
    // of your shape, and the indices that will
    // triangulate that shape using the fan method.
    std::vector<glm::vec3> pos;
    std::vector<GLuint> idx;

    // Your code here

    //Generate vertices around circle
    for (int i = 0; i < 20; ++i) {
        float angle = (2.0f * M_PI * i) / 20;
        float x = .5 * cos(angle);
        float y = .5 * sin(angle);
        pos.push_back(glm::vec3(x, y, 1.0f));
    }

    //triangulate
    for (int i = 0; i < 18; i++)
    {
        idx.push_back(0);
        idx.push_back(i + 1);
        idx.push_back(i + 2);
    }

    // Set indexBufferLength to the total number of
    // indices used to draw your polygon so that it
    // can be passed to glDrawElements in the
    // drawGeometry function.
    indexBufferLength = idx.size();

    // TODO: Use glGenBuffers, glBindBuffer, and glBufferData
    // to set up the GPU-side storage for your geometry data
    // via bufferPosition and bufferIndex.
    // Don't forget to use the GL_ARRAY_BUFFER flag for bufferPosition,
    // but use GL_ELEMENT_ARRAY_BUFFER for bufferIndex since it represents
    // triangle indices rather than mesh vertices.

    glGenBuffers(1, &bufferPosition);
    printGLErrorLog();
    glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);
    printGLErrorLog();
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_STATIC_DRAW);
    printGLErrorLog();

    glGenBuffers(1, &bufferIndex);
    printGLErrorLog();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    printGLErrorLog();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);
    printGLErrorLog();

}

void MyGL::initializeGL() {
    // Create an OpenGL context using Qt's
    // QOpenGLFunctions_3_2_Core class.
    // If you were programming in a non-Qt context
    // you might use GLEW (GL Extension Wrangler)
    // or GLFW (Graphics Library Framework) instead.
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();
    // Create a Vertex Array Object so that we can render
    // our geometry using Vertex Buffer Objects.
    glGenVertexArrays(1, &vao);
    // We have to have a VAO bound in OpenGL 3.2 Core.
    // But if we're not using multiple VAOs, we
    // can just bind one once.
    glBindVertexArray(vao);

    createAndCompileShaderProgram(this->vertShader, this->fragShader, this->shaderProgram,
                       "passthrough.vert.glsl", "coloring.frag.glsl");
    printGLErrorLog();
    getHandlesForShaderVariables(this->shaderProgram);
    printGLErrorLog();

    initializeAndBufferGeometryData();
    printGLErrorLog();
}

void MyGL::resizeGL(int w, int h) {
    w *= this->devicePixelRatio();
    h *= this->devicePixelRatio();

    // TODO: Add code here to pass your screen dimensions to
    // your vertex shader's uniform variable that stores that information.
    // You'll need to use the glUniform2i OpenGL API function.
    glUseProgram(shaderProgram);

    glUniform2i(shaderUniformVariableHandles["u_ScreenDimensions"], w, h);


}

void MyGL::drawGeometry() {
    glUseProgram(shaderProgram);
    printGLErrorLog();

    // glm::mat3 modelMatrix = glm::mat3(1.0f);

    // float angle = currTime;
    // glm::vec3 modMat = glm::rotate(glm::toQuat(modelMatrix), glm::vec3(angle));


    glm::mat3 translationMatrix = glm::mat3(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        position[0], position[1], 1.0f
    );


    // GLuint modelLoc = glGetUniformLocation(shaderProgram, "u_Model");
    glUniformMatrix3fv(shaderUniformVariableHandles["u_Model"], 1, GL_FALSE, glm::value_ptr(translationMatrix));
    printGLErrorLog();

    // TODO: Use the functions listed below to
    // associate bufferPosition with the shader
    // "in" variable named "vs_Pos", and tell
    // OpenGL that every three consecutive floats
    // in the buffer represent one vec3.
    // - glBindBuffer
    // - glEnableVertexAttribArray
    // - glVertexAttribPointer

    glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);
    printGLErrorLog();

    GLuint posAttribLocation = glGetAttribLocation(shaderProgram, "vs_Pos");
    glEnableVertexAttribArray(posAttribLocation);
    printGLErrorLog();

    glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, static_cast<char*>(0));
    printGLErrorLog();

    // TODO: Use the functions listed below to
    // draw your mesh using the triangle indices stored
    // in bufferIndex.
    // - glBindBuffer
    // - glDrawElements

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    printGLErrorLog();
    glDrawElements(GL_TRIANGLES, indexBufferLength, GL_UNSIGNED_INT, static_cast<char*>(0));
    printGLErrorLog();

    // TODO: Use glDisableVertexAttribArray
    // after drawing your geometry to "lock"
    // the GPU-side variable named vs_Pos.

    glDisableVertexAttribArray(posAttribLocation);
    printGLErrorLog();

}

void MyGL::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawGeometry();
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // Extra credit: Pass the mouse event's position
    // as an X and Y coordinate to the shader program's
    // mouse position uniform variable. Then, use the mouse
    // position to affect your scene in some way.
    // Don't forget to call glUseProgram first!
    glm::ivec2 pos(e->pos().x(), e->pos().y());

    update();
}

void MyGL::tick() {
    // TODO: Add code to update your shader's
    // u_Time variable.
    glUseProgram(shaderProgram);
    currTime += 0.01f;


    if(position[0] + 0.3 > 1.f) {
        velocity[0] *= -1.f;
    }
    if(position[0] - 0.3 < -1.f) {
        velocity[0] *= -1.f;
    }


    if(position[1] + 0.3 > 1.f) {
        velocity[1] *= -1.f;
    }
    if(position[1] - 0.3 < -1.f) {
        velocity[1] *= -1.f;
    }
    position = position + velocity;

    glUniform1f(shaderUniformVariableHandles["u_Time"], currTime);
    // Do not delete this function call! This
    // repaints the GL viewport every time the timer
    // invokes tick().
    update();
}
