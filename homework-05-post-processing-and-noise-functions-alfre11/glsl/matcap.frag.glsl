#version 330 core

uniform sampler2D u_Texture;

in vec2 fs_UV;

layout(location = 0) out vec3 out_Col;

void main()
{
    // TODO: Use the input UV coordinates to
    // sample the input matcap texture

    // out_Col = vec3(0,0,0);

    vec3 matcapColor = texture(u_Texture, fs_UV).rgb;

    out_Col = matcapColor;
}
