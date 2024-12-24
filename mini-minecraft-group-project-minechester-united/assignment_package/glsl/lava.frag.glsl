#version 330 core

uniform sampler2D u_Texture;

in vec2 fs_UV;
out vec4 out_Col;

void main() {
    vec4 normalColor = texture(u_Texture, fs_UV);

    // out_Col = vec4(normalColor.r + 0.2, normalColor.g, normalColor.b, normalColor.a);
    out_Col = vec4(fs_UV, 0.0f, 1.0f);
}
