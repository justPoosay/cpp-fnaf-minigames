#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;

out vec4 finalColor;

// --- CRT Effect Parameters ---
// Curvature (parametry pozostają, ale efekt będzie zakomentowany w main)
const float warpX = 0.031;
const float warpY = 0.041;

// Scanlines
const float scanline_thickness_ratio = 0.4;
const float scanline_frequency_scale = 1.0;
const float scanline_brightness_multiplier = 1.05;
const float scanline_darkness_multiplier = 0.75;

// Vignette (parametry pozostają, ale efekt będzie zakomentowany w main)
const float vignetteIntensity = 0.5;

// Global brightness boost (opcjonalny)
const float global_brightness_boost = 1.0;


// --- Helper Functions (pozostają zdefiniowane, na wypadek gdybyś chciał je włączyć z powrotem) ---
vec2 Warp(vec2 pos)
{
    pos = pos * 2.0 - 1.0;
    pos.x *= 1.0 + (pos.y * pos.y) * warpX;
    pos.y *= 1.0 + (pos.x * pos.x) * warpY;
    return pos * 0.5 + 0.5;
}

float Vignette(vec2 uv) {
    uv = (uv - 0.5) * 2.0;
    return clamp(pow(cos(uv.x * 0.5 * 3.14159265) * cos(uv.y * 0.5 * 3.14159265), vignetteIntensity), 0.0, 1.0);
}

float ScanlineEffect(vec2 uv, vec2 screen_resolution) {
    float line_cycle_height = 4.0 / scanline_frequency_scale;
    float v_pos_in_cycle = mod(uv.y * screen_resolution.y, line_cycle_height);

    if (v_pos_in_cycle < line_cycle_height * scanline_thickness_ratio) {
        return scanline_brightness_multiplier;
    } else {
        return scanline_darkness_multiplier;
    }
}

void main()
{
    // vec2 warpedCoord = Warp(fragTexCoord); // ZAKOMENTOWANE - brak zniekształcenia beczkowego
    vec2 currentTexCoord = fragTexCoord; // Używamy oryginalnych koordynat tekstury

    vec3 sampledColor = vec3(0.0);

    // Sprawdzamy koordynaty (choć bez Warpa jest to mniej krytyczne, ale dobra praktyka)
    if (currentTexCoord.x >= 0.0 && currentTexCoord.x <= 1.0 && currentTexCoord.y >= 0.0 && currentTexCoord.y <= 1.0)
    {
        // sampledColor = texture(texture0, warpedCoord).rgb * fragColor.rgb; // Oryginalnie z warpedCoord
        sampledColor = texture(texture0, currentTexCoord).rgb * fragColor.rgb; // Używamy currentTexCoord

        // Zastosuj tylko Scanlines
        float scanlineFactor = ScanlineEffect(currentTexCoord, resolution); // Używamy currentTexCoord dla prostych linii
        sampledColor *= scanlineFactor;

        // sampledColor *= Vignette(fragTexCoord); // ZAKOMENTOWANE - brak winiety

        // (Opcjonalny) Zastosuj globalny mnożnik jasności
        // sampledColor *= global_brightness_boost;

    } else {
        // Kolor poza obszarem tekstury (mniej prawdopodobne bez warp)
        sampledColor = vec3(0.0, 0.0, 0.0);
    }

    finalColor = vec4(clamp(sampledColor, 0.0, 1.0), fragColor.a);
}