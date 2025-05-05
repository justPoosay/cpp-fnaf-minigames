#version 330 core

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms passed from your C++ code
uniform sampler2D texture0; // This will be our RenderTexture
uniform vec2 resolution;   // The resolution of the RenderTexture (e.g., 1280x720)
uniform float time;        // Game time in seconds, for potential animation

// Output color for the fragment
out vec4 finalColor;

const float hardScan = -8.0;
const float hardPix = -3.0;
const float warpX = 0.031;
const float warpY = 0.041;
const float maskDark = 0.3;
const float maskLight = 1.5;
const float scanlineWeight = 2.0;
const float vignetteIntensity = 0.5;


// Function to apply barrel distortion (screen curvature)
vec2 Warp(vec2 pos)
{
    pos = pos * 2.0 - 1.0; // Remap to -1 to 1
    pos.x *= 1.0 + (pos.y * pos.y) * warpX;
    pos.y *= 1.0 + (pos.x * pos.x) * warpY;
    return pos * 0.5 + 0.5; // Remap back to 0 to 1
}

// Function to calculate vignette darkening
float vignette(vec2 uv) {
    uv = (uv - 0.5) * 2.0; // Remap to -1..1
    // Smoothly decrease brightness towards the edges
    // Using PI constant (approx) directly
    return clamp(pow(cos(uv.x * 0.5 * 3.14159) * cos(uv.y * 0.5 * 3.14159), vignetteIntensity), 0.0, 1.0);
}

void main()
{
    // Apply screen curvature to texture coordinates
    vec2 warpedCoord = Warp(fragTexCoord);

    vec3 res = vec3(0.0); // Initialize result color

    // Check if warped coordinates are within the valid texture range [0, 1]
    if (warpedCoord.x >= 0.0 && warpedCoord.x <= 1.0 && warpedCoord.y >= 0.0 && warpedCoord.y <= 1.0)
    {
        // Sample the original texture using the warped coordinates
        res = texture(texture0, warpedCoord).rgb * fragColor.rgb;

        // Calculate scanline effect
        float scanline = sin((fragTexCoord.y * resolution.y * scanlineWeight) * 3.14159 * 2.0);
        scanline = ((scanline * hardScan - maskDark) + 1.0); // Adjust intensity and darkness
        scanline = clamp(scanline, 0.0, 2.0); // Clamp scanline factor

        // Apply scanlines
        res *= scanline * maskLight;

        // Apply vignette using original UVs
        res *= vignette(fragTexCoord);

    } else {
        // Color outside the warped screen area
        res = vec3(0.0);
    }

    // Output the final color with full alpha
    finalColor = vec4(clamp(res, 0.0, 1.0), 1.0);
}