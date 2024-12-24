#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_2_Core>
#include <QString>
#include <unordered_map>
#include <QMouseEvent>
#include <QTimer>
#include "glm/glm.hpp"

class MyGL : public QOpenGLWidget,
             public QOpenGLFunctions_3_2_Core
{
    Q_OBJECT

private:
    // A timer that is set up to fire every 16 milliseconds,
    // and causes MyGL::tick() to be called each time.
    // You can use tick() to update your shader's time variable
    // if you want to animate anything.
    QTimer timer;
    float currTime;

    glm::vec2 position;
    glm::vec2 velocity;

    // A handle for our Vertex Array Object.
    // This will store the Vertex Buffer Objects
    // that we use to represent our geometry data.
    // We've already written all the code needed to
    // interact with this.
    GLuint vao;

    // These will serve as CPU-side handles to the components
    // that make up our shader program. They are given values
    // in createAndCompileShaderProgram(), which is called in
    // initializeGL(). If you want to have more than one shader
    // program, you will need more than one set of vertex shader,
    // fragment shader, and shader program handles.
    GLuint vertShader;
    GLuint fragShader;
    GLuint shaderProgram;

    // This function is designed to initialize a vertex shader
    // object, a fragment shader object, and shader program object
    // on the GPU. It stores handles to these objects in the three
    // input GLuints. It also takes in a pair of file names to indicate
    // which .glsl files you want to serve as the source text for your
    // vertex shader and fragment shader.
    // This function reads these files and buffers their contents
    // to the GPU, where they are compiled. If any GLSL compilation
    // errors occur, they will be printed to the console by this
    // function.
    // This function is called for you in initializeGL(), and it sets
    // up a shader program with passthrough.vert.glsl and coloring.frag.glsl
    // as its vertex and fragment shader source files.
    void createAndCompileShaderProgram(GLuint &vertHandle,
                                       GLuint &fragHandle,
                                       GLuint &shaderProgramHandle,
                                       QString vertFile,
                                       QString fragFile);

    /// The following two maps store the CPU-side handles
    /// for the "in" and "uniform" variables written in
    /// the shader program used in this assignment.
    /// Each key is the literal name of the variable as it
    /// is written in the shader, e.g. vs_Pos or u_ScreenDimensions.
    /// Each value is the handle to the variable named by the key.
    std::unordered_map<std::string, GLint> shaderAttribVariableHandles;
    std::unordered_map<std::string, GLint> shaderUniformVariableHandles;

    // This function uses glGetAttribLocation and glGetUniformLocation
    // to set up the key-value pairs in the maps above.
    void getHandlesForShaderVariables(GLuint shaderProgramHandle);

    // These two GLuints will serve as the CPU-side handles
    // to GPU-side arrays of vertex information, also known as
    // vertex buffers. bufferPosition will be the handle for
    // an array of vertex position data, while bufferIndex will
    // be the handle for an array of triangle indices.
    GLuint bufferPosition;
    GLuint bufferIndex;
    // We will store the number of indices that we send to our
    // index buffer. For example, if the index buffer was
    // {0, 1, 2, 0, 2, 3}, then indexBufferLength would be 6.
    // We need this information in order to tell
    // the glDrawElements function how many indices to read when
    // drawing our geometry.
    int indexBufferLength;

    // This function is called from initializeGL.
    // You must write code for it that builds a 2D
    // regular polygon.
    void initializeAndBufferGeometryData();
    // As the name would imply, you will write code
    // in this function that draws your geometry.
    // It is called from paintGL().
    void drawGeometry();


public:
    MyGL(QWidget *parent);
    ~MyGL();

    /// The following three functions are automatically called
    /// by QOpenGLWidget (MyGL's parent class)

    // Use this function to initialize all of your OpenGL
    // components, like your shader program(s), vertex buffers,
    // shader variable handles, etc.
    // Called automatically by QOpenGLWidget one time only,
    // just after MyGL is constructed.
    void initializeGL() override;
    // Use this function to pass any window size information to
    // your shaders.
    // Called automatically whenever the OpenGL window is resized,
    // including when MyGL is first initialized then sized up to the
    // dimensions set for it in the UI editor.
    void resizeGL(int w, int h) override;
    // Make all of your draw calls in this function; doing them
    // anywhere else won't work. Called *once* automatically
    // by MyGL's initialization, then you have to manually call it
    // using QOpenGLWidget's update() function. DO NOT CALL paintGL()
    // directly! Only call it by calling update()!
    void paintGL() override;

    /// The following function should be used to get the direct filepath
    /// to this Qt project so that you can read the GLSL files used for
    /// your shader
    QString getCurrentPath() const;

    /// The following function is automatically called whenever Qt detects
    /// a mouse movement. We will use it to pass out cursor's current location
    /// to our shader.
    void mouseMoveEvent(QMouseEvent *e) override;

    /// The functions below are useful for determining what's going wrong
    /// with your OpenGL code

    // Prints out the highest version of OpenGL supported by your hardware
    void debugContextVersion();
    // You should call this function in order to figure out which of your
    // OpenGL function calls are causing an OpenGL pipeline error.
    // Just invoke it after each GL function call you think is causing an error,
    // then put a breakpoint in it to see which call triggers it.
    void printGLErrorLog();
    // Prints any error messages from the shader program linking
    // process to the console.
    void printLinkInfoLog(int prog);
    // Prints any GLSL shader compiler errors to the console.
    void printShaderInfoLog(int shader);

public slots:
    void tick();
};
