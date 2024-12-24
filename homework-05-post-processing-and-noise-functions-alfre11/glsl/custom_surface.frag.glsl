#version 330

uniform sampler2D u_Texture; // The texture to be read from by this shader
uniform float u_Time;

in vec3 fs_Pos;
in vec3 fs_Nor;
in vec2 fs_UV;

layout(location = 0) out vec3 out_Color;

void main()
{
    out_Color = texture(u_Texture, fs_UV).rgb;

    float waveFreq = 2.0f;
    float waveAmp = 0.1;

    vec2 animatedUV = fs_UV + vec2(0.0, sin(fs_Pos.x * waveFreq + u_Time) * waveAmp);

    out_Color = texture(u_Texture, animatedUV).rgb;

    // out_Color = texture(u_Texture, fs_UV).rgb;
}
