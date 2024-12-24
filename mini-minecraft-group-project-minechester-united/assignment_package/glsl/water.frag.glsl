#version 330 core

uniform sampler2D u_Texture;

uniform int u_InLiquid;

uniform float u_Time;

in vec2 fs_UV;
out vec4 out_Col;

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

void main() {
    float warpStrength = 0.05;
    float noiseScale = 5.0;
    // vec4 nonTex = vec4(1.0, 1.0, 1.0, 1.0);

    // out_Col = vec4(normalColor.r, normalColor.g, normalColor.b, 1.0f);
    // out_Col = vec4(fs_UV, 0.0f, 1.0f);
    // out_Col = vec4(0.0f, 1.0f, 0.0f, 1.0f);

    vec2 noiseUV = fs_UV * noiseScale + vec2(u_Time * 0.1, u_Time * 0.1);
    vec2 warpOffset = vec2(noise(noiseUV), noise(noiseUV + vec2(100.0, 100.0))) * warpStrength;

    vec2 warpedUV = fs_UV + warpOffset;

    vec4 normalColor = texture(u_Texture, fs_UV);

    vec4 blueTint = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 redTint = vec4(1.0, 0.0, 0.0, 1.0);
    if(u_InLiquid == 2) {
        vec4 normalColor = texture(u_Texture, warpedUV);
        out_Col = mix(normalColor, redTint, 0.4);
    } else if(u_InLiquid == 1) {
        vec4 normalColor = texture(u_Texture, warpedUV);
        out_Col = mix(normalColor, blueTint, 0.4);
    } else {
        out_Col = normalColor;
    }

    // out_Col = mix(normalColor, blueTint, 0.1);

    // out_Col = blueTint;

}
