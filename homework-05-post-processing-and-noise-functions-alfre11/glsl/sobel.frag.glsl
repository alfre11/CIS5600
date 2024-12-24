#version 330 core

in vec2 fs_UV;
out vec3 color;

uniform sampler2D u_Texture;
uniform vec2 u_ScreenDimensions;


void main() {
    // TODO: Use texelFetch to sample u_Texture
    // at each of the nine pixels at and surrounding
    // this shader's fragment. Multiply each sample by the
    // associated cell in Gx, and add it to a sum of horizontal
    // color deltas. Do the same for Gy and a sum of vertical
    // color deltas. Then, output the square root of the sum
    // of both deltas squared.

    // color = texture(u_Texture, fs_UV).rgb;
    vec3 samp[9];

    const int Gx[9] = int[9]( 3,  0, -3,
                        10, 0, -10,
                        3,  0, -3);

    const int Gy[9] = int[9]( 3,  10,  3,
                        0,   0,   0,
                        -3, -10, -3);


    samp[0] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2(-1, -1)), 0).rgb;
    samp[1] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 0, -1)), 0).rgb;
    samp[2] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 1, -1)), 0).rgb;

    samp[3] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2(-1,  0)), 0).rgb;
    samp[4] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 0,  0)), 0).rgb;
    samp[5] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 1,  0)), 0).rgb;

    samp[6] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2(-1,  1)), 0).rgb;
    samp[7] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 0,  1)), 0).rgb;
    samp[8] = texelFetch(u_Texture, ivec2(gl_FragCoord.xy + vec2( 1,  1)), 0).rgb;

    vec3 Gx_sum = vec3(0.0);
    vec3 Gy_sum = vec3(0.0);

    for (int i = 0; i < 9; i++) {
        Gx_sum += samp[i] * float(Gx[i]);
        Gy_sum += samp[i] * float(Gy[i]);
    }

    vec3 edge_magnitude = sqrt((Gx_sum * Gx_sum) + (Gy_sum * Gy_sum));

    color = vec3(edge_magnitude);
}
