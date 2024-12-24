#version 150

// Uniforms and outputs
uniform mat4 u_ViewProj;         // View-Projection matrix for transforming points in view space
uniform vec2 u_Dimensions;       // Dimensions of the viewport (e.g., window or screen)
uniform vec3 u_Eye;              // Position of the camera/eye in world space
uniform float u_Time;            // Elapsed time, used for animation
out vec4 outColor;               // The output color of the fragment shader

// Constants
const float PI = 3.14159265359;  // Value of Pi
const float TWO_PI = 6.28318530718; // Value of 2*Pi

// Color palettes
const vec3 sunset[5] = vec3[](
    vec3(255, 229, 119) / 255.0, // Light golden yellow
    vec3(254, 192, 81) / 255.0,  // Soft orange
    vec3(255, 137, 103) / 255.0, // Pinkish-orange
    vec3(253, 96, 81) / 255.0,   // Deeper red
    vec3(57, 32, 51) / 255.0     // Dark purple
);
const vec3 dusk[5] = vec3[](
    vec3(144, 96, 144) / 255.0,  // Soft purple
    vec3(96, 72, 120) / 255.0,   // Deeper purple
    vec3(72, 48, 120) / 255.0,   // Rich violet
    vec3(48, 24, 96) / 255.0,    // Darker violet
    vec3(0, 24, 72) / 255.0      // Almost black with blue tones
);
const vec3 daySky = vec3(0.53, 0.81, 0.98);  // Daytime sky blue
const vec3 nightSky = vec3(0.05, 0.05, 0.2); // Nighttime dark blue
const vec3 sunColor = vec3(255, 255, 190) / 255.0; // Bright yellowish-white for the sun

// Function: Convert 3D point on a sphere to UV coordinates
vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);   // Azimuthal angle
    if (phi < 0) {
        phi += TWO_PI;           // Ensure phi is in range [0, 2*Pi]
    }
    float theta = acos(p.y);     // Polar angle
    return vec2(1 - phi / TWO_PI, 1 - theta / PI); // Map angles to [0, 1] UV range
}

// Function: Map UV coordinates to a gradient of sunset colors
vec3 uvToSunset(vec2 uv) {
    if (uv.y < 0.5) {
        return sunset[0];
    } else if (uv.y < 0.55) {
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05);
    } else if (uv.y < 0.6) {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    } else if (uv.y < 0.65) {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    } else if (uv.y < 0.75) {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4]; // Darkest color at the top
}

// Function: Map UV coordinates to a gradient of dusk colors
vec3 uvToDusk(vec2 uv) {
    if (uv.y < 0.5) {
        return dusk[0];
    } else if (uv.y < 0.55) {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    } else if (uv.y < 0.6) {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    } else if (uv.y < 0.65) {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    } else if (uv.y < 0.75) {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4]; // Darkest color at the top
}

// Function: Generate 2D pseudo-random numbers based on input
vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

// Function: Generate 3D pseudo-random numbers based on input
vec3 random3(vec3 p) {
    return fract(sin(vec3(
        dot(p, vec3(127.1, 311.7, 191.999)),
        dot(p, vec3(269.5, 183.3, 765.54)),
        dot(p, vec3(420.69, 631.2, 109.21))
    )) * 43758.5453);
}

// Function: Rotate a point around the X-axis
vec4 rotateX(vec3 p, float a) {
    return vec4(p.x, cos(a) * p.y + -sin(a) * p.z, sin(a) * p.y + cos(a) * p.z, 0.0);
}

// Function: Compute 3D Worley noise (cellular noise)
float WorleyNoise3D(vec3 p) {
    vec3 pointInt = floor(p);         // Integer part of the position
    vec3 pointFract = fract(p);       // Fractional part of the position
    float minDist = 1.0;              // Minimum distance for the noise
    for (int z = -1; z <= 1; z++) {
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                vec3 point = random3(pointInt + neighbor); // Random offset for neighboring cell
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // Animation
                vec3 diff = neighbor + point - pointFract; // Vector to the random point
                float dist = length(diff); // Compute distance
                minDist = min(minDist, dist); // Update minimum distance
            }
        }
    }
    return minDist;
}

// Function: Compute fractal Brownian motion using Worley noise
float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for (int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp; // Add successive noise layers
        freq *= 2; // Increase frequency
        amp *= 0.5; // Decrease amplitude
    }
    return sum;
}

// Predefined thresholds for sunset/dusk blending
#define SUNSET_THRESHOLD 0.375
#define DUSK_THRESHOLD -0.3

void main() {
    // Map fragment coordinates to NDC (-1 to 1 range)
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0;

    // Create a large vector in view space for sky ray direction
    vec4 p = vec4(ndc.xy, 1, 1);
    p *= 1000.0;
    p = u_ViewProj * p;

    // Normalize the ray direction
    vec3 rayDir = normalize(p.xyz - u_Eye);

    // Compute UV coordinates for the sky sphere
    vec2 uv = sphereToUV(rayDir);

    // Add offsets for procedural noise
    vec2 offset = vec2(worleyFBM(rayDir));
    offset *= 2.0;
    offset -= vec2(1.0);

    // Determine sunset and dusk colors based on UV
    vec3 sunsetColor = uvToSunset(uv + offset * 0.1);
    vec3 duskColor = uvToDusk(uv + offset * 0.1);

    // Compute the sun's direction
    vec3 sunDir = normalize(vec3(rotateX(normalize(vec3(0, 0, -1.f)), u_Time * 0.001)));

    // Parameters for sun blending
    float sunSize = 30.0;           // Apparent size of the sun
    float angle = acos(dot(rayDir, sunDir)) * 360.0 / PI; // Angle between ray and sun
    float raySunDot = dot(rayDir, sunDir); // Dot product for blending
    float t = (raySunDot - SUNSET_THRESHOLD) / (DUSK_THRESHOLD - SUNSET_THRESHOLD);

    // Sky color calculation
    vec3 skyColor;
    if (raySunDot > DUSK_THRESHOLD) {
        if (angle < sunSize) {
            if (angle < 7.5) {
                skyColor = sunColor; // Sun's core
            } else {
                // Blend from sun to surrounding sunset/dusk
                skyColor = mix(sunColor, mix(sunsetColor, duskColor, t), (angle - 7.5) / 22.5);
            }
        } else {
            skyColor = mix(sunsetColor, duskColor, t); // Sunset/dusk blending
        }
    } else {
        skyColor = mix(sunsetColor, duskColor, t); // Full dusk blending
    }

    // Day-night blending based on sun height
    float sunHeight = dot(sunDir, vec3(0, 1, 0)); // Sun's height relative to the horizon
    float dayFactor = smoothstep(0.1, 0.4, sunHeight); // Day transition
    float nightFactor = smoothstep(-0.3, 0.0, sunHeight); // Night transition

    // Blend between day and night skies
    vec3 dayNightMix = mix(nightSky, daySky, dayFactor);
    skyColor = mix(dayNightMix, skyColor, 1.0 - abs(sunHeight));

    // Output the final color
    outColor = vec4(skyColor, 1);
}
