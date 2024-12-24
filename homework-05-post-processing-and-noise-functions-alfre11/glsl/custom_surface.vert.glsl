#version 330 core

uniform mat4 u_View;
uniform mat4 u_Proj;

uniform mat4 u_Model;
uniform float u_Time;
uniform float u_NoiseScale;
uniform float u_Amplitude;

in vec3 vs_Pos;
in vec3 vs_Nor;
in vec2 vs_UV;

out vec3 fs_Pos;
out vec3 fs_Nor;
out vec2 fs_UV;

// float interpNoise(float x, float y) {
//     int intX = int(floor(x));
//     float fracX = fract(x);
//     int intY = int(floor(y));
//     float fracY = fract(y);

//     float v1 = noise1(x);
//     float v2 = noise1(y);
//     float v3 = noise1(x+1);
//     float v4 = noise1(y+1);

//     float i1 = mix(v1, v2, fracX);
//     float i2 = mix(v3, v4, fracY);

//     return mix(i1, i2, fracY);
// }

// //FBM
// float noiseFunc(vec3 pos) {
//     float total = 0;
//     float persistence = 0.5f;
//     int octaves = 8;
//     float freq = 2.f;
//     float amp = 0.5f;
//     for(int i = 1; i <= octaves; i++) {
//         total += interpNoise(pos.x * freq, pos.y * freq) * amp;
//         freq *= 2.f;
//         amp*=persistence;
//     }
//     return total;
// }

void main()
{
    vec4 worldPos = u_Model * vec4(vs_Pos, 1.0);

    // float noiseVal = noiseFunc(worldPos.xyz * u_NoiseScale + u_Time);

    // vec3 displacedPos = vs_Pos + vs_Nor * noiseVal * u_Amplitude;

    float factor = clamp(.5 * sin(.01 * u_Time) + .5, 0.0, 1.0);
    factor *= factor;

    fs_Nor = vs_Nor;
    fs_UV = vs_UV;
    fs_Pos = mix(3 * normalize(vs_Pos), vs_Pos, factor);

    gl_Position = u_Proj * u_View * vec4(fs_Pos, 1.0);

    // vec4 worldPos = u_Model * vec4(vs_Pos, 1.0);


}
