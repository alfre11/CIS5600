#version 330 core

// uniform ivec2 u_Dimensions;
// uniform float u_Time;

in vec2 fs_UV;

out vec3 color;

uniform sampler2D u_Texture;
uniform vec2 u_Dimensions;
uniform float u_Time;

float random(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(random(i + vec2(0.0, 0.0)), random(i + vec2(1.0, 0.0)), u.x),
               mix(random(i + vec2(0.0, 1.0)), random(i + vec2(1.0, 1.0)), u.x), u.y);
}

void main()
{
    // color = texture(u_Texture, fs_UV).rgb;

    float warpStrength = 0.05;
    float noiseScale = 5.0;

    vec2 noiseUV = fs_UV * noiseScale + vec2(u_Time * 0.1, u_Time * 0.1);
    vec2 warpOffset = vec2(noise(noiseUV), noise(noiseUV + vec2(100.0, 100.0))) * warpStrength;

    vec2 warpedUV = fs_UV + warpOffset;

    color = texture(u_Texture, warpedUV).rgb;
}
