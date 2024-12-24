#version 330 core

// The position of the vertex being
// processed by this instance of the vertex shader
in vec3 vs_Pos;
// The width and height of your canvas in pixels
uniform ivec2 u_ScreenDimensions;
// A homogeneous 2D transformation matrix
uniform mat3 u_Model;
// A constantly-increasing value updated in MyGL::tick()
uniform float u_Time;

void main()
{
    // TODO: Add code that scales your vertex's X coordinate
    // based on your screen's aspect ratio to correct for the
    // fact that screen space is uniformly [-1,1] in X and Y
    // regardless of aspect ratio.

    vec3 p = u_Model * vs_Pos;

    vec4 pos = vec4(p[0] / (float(u_ScreenDimensions.x) / u_ScreenDimensions.y), p[1], p[2], 1.0);

    // Example code: Passes the input vertex position down
    // the pipeline with a W coordinate of 1.
    gl_Position = pos;
}
