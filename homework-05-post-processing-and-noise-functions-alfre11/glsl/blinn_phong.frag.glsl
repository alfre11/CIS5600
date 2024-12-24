#version 330 core

uniform sampler2D u_Texture; // An object that lets us access
                             // the texture file we project onto
                             // our model's surface
uniform vec3 u_CamLook; // The camera's forward vector
uniform vec3 u_CamPos; // The camera's position

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

in vec3 fs_Pos;
in vec3 fs_Nor;
in vec2 fs_UV;

out vec3 out_Color;

void main()
{
    // TODO: Implement Blinn-Phong reflection
    out_Color = texture(u_Texture, fs_UV).rgb;

    vec3 N = normalize(fs_Nor);
    vec3 V = u_CamPos - fs_Pos;
    vec3 L = -u_CamLook;
    vec3 H = normalize(V + L);

    float specIntens = max(pow(dot(H, N), 100.f), 0);

    float diffuse = max(dot(fs_Nor, L), 0.0);

    vec3 diffColor = out_Color * diffuse;

    vec3 specColor = specIntens * vec3(1.f);

    vec3 ambient = .1 * out_Color;

    out_Color = ambient + diffColor + specColor;

}
