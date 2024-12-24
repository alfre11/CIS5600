#version 330 core

uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 vs_Pos;
in vec3 vs_Nor;

out vec2 fs_UV;

void main()
{
    // TODO: Map your surface normal to camera space,
    // then map the camera-space normal to UV coordinates
    // for sampling the matcap texture in the fragment shader

    gl_Position = u_Proj * u_View * vec4(vs_Pos, 1.);

    vec3 camNormal = normalize(mat3(u_View) * vs_Nor);
    fs_UV = camNormal.xy * 0.5 + 0.5;
}
