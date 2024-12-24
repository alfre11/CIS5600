#version 330 core

// The width and height of your canvas in pixels
uniform ivec2 u_ScreenDimensions;
// A constantly-increasing value updated in MyGL::tick()
uniform float u_Time;
// Only necessary for extra credit; the mouse's X and Y coords
uniform ivec2 u_MousePosition;

// The output color of this shader.
out vec4 out_Col;

uniform mat3 u_Model;

void main() {
    // TODO: Do something cool using mouse position



    // out_Col = vec4(u_MousePosition.x, u_MousePosition.y, 1, 1);
    // out_Col = vec4(gl_FragCoord.x, gl_FragCoord.y, 1, 1);

    vec2 fragCoordNorm = gl_FragCoord.xy / vec2(u_ScreenDimensions);

    vec3 fragCoordNorm3 = vec3(fragCoordNorm, 0.5 + 0.5 * sin(u_Time));

    out_Col = vec4(fragCoordNorm3, 1.0);

    // Example code: Sets the final fragment color
    // to white with 100% opacity.
    // out_Col = vec4(1,1,1,1);
}
