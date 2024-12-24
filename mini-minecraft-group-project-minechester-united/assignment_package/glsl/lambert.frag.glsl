#version 150

// uniform vec4 u_Color;
uniform vec3 u_Player;
uniform sampler2D u_Texture;
uniform float u_Time;
uniform float u_FogTime;

// Inputs from the vertex shader
in vec4 fs_Pos;
in vec4 fs_UV;
in vec4 fs_Nor;
in vec4 fs_LightVec;

// Output color
out vec4 out_Col;

// Constants
const float PI = 3.14159265359;

// Get the smooth fog color based on sun height
vec3 getFogColor(float sunHeight) {
    // Define colors for different periods
    vec3 dawnFog = vec3(1.0, 0.7, 0.4);  // Warm orange for dawn
    vec3 dayFog = vec3(0.8, 0.8, 0.8);   // White-ish fog for daytime
    vec3 duskFog = vec3(1.0, 0.5, 0.2);  // Deep orange for dusk
    vec3 nightFog = vec3(0.1, 0.1, 0.2); // Deep blue-black for night

    // Normalize sunHeight (-1.0 to 1.0 range)
    float t = (sunHeight + 1.0) * 0.25; // 0.0 = Night, 1.0 = Day

    // Create smooth transitions between times
    vec3 fogColor;
    if (t < 0.25) {
        // Transition: Night -> Dawn
        float blend = smoothstep(0.0, 0.25, t);
        fogColor = mix(nightFog, dawnFog, blend);
    } else if (t < 0.5) {
        // Transition: Dawn -> Day
        float blend = smoothstep(0.25, 0.5, t);
        fogColor = mix(dawnFog, dayFog, blend);
    } else if (t < 0.75) {
        // Transition: Day -> Dusk
        float blend = smoothstep(0.5, 0.75, t);
        fogColor = mix(dayFog, duskFog, blend);
    } else {
        // Transition: Dusk -> Night
        float blend = smoothstep(0.75, 1.0, t);
        fogColor = mix(duskFog, nightFog, blend);
    }

    return fogColor;
}

void main()
{
    vec4 texColor = texture(u_Texture, vec2(fs_UV.x + (fs_UV[2] == 1 ? u_Time / 256 : 0), fs_UV.y));
    vec4 diffuseColor = texColor;

    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;
    float lightIntensity = diffuseTerm + ambientTerm;

    // Calculate sun direction and height
    vec3 sunDir = normalize(vec3(0.0, sin(u_FogTime * 0.001), -cos(u_FogTime * 0.001)));
    float sunHeight = dot(sunDir, vec3(0, 1, 0));

    // Get fog color based on sun height
    vec3 fogColor = getFogColor(sunHeight);
    float dist = length(fs_Pos.xz - u_Player.xz) * 0.01;

    // Compute final shaded color
    // out_Col = vec4(mix(vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a), fog, pow(smoothstep(0, 1, min(1, dist)), 2)).rgb, diffuseColor.a);
    // Blend between object color and fog color
    float fogFactor = pow(smoothstep(0, 1, min(1, dist)), 2);
    vec3 finalColor = mix(diffuseColor.rgb * lightIntensity, fogColor, fogFactor);

    out_Col = vec4(finalColor, diffuseColor.a);
}
