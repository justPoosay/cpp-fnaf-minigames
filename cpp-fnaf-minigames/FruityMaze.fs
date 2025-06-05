#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;

out vec4 finalColor;

// --- CRT Effect Parameters ---
// Scanlines
const float scanline_thickness_ratio = 0.4;
const float scanline_frequency_scale = 1.0;
const float scanline_brightness_multiplier = 1.05;
const float scanline_darkness_multiplier = 0.75;

// Screen overlay parameters
const float screen_border_width = 0.02;  // How thick the screen border is (reduced from 0.08)
const float screen_corner_radius = 0.01; // Rounded corners (reduced from 0.03)
const vec3 screen_border_color = vec3(0.2, 0.2, 0.2); // Dark gray border
const vec3 screen_inner_color = vec3(0.05, 0.05, 0.05); // Very dark inner border
const float screen_reflection_intensity = 0.15; // Subtle screen reflection

// --- Helper Functions ---
float ScanlineEffect(vec2 uv, vec2 screen_resolution) {
    float line_cycle_height = 4.0 / scanline_frequency_scale;
    float v_pos_in_cycle = mod(uv.y * screen_resolution.y, line_cycle_height);

    if (v_pos_in_cycle < line_cycle_height * scanline_thickness_ratio) {
        return scanline_brightness_multiplier;
    } else {
        return scanline_darkness_multiplier;
    }
}

// Rounded rectangle distance function
float roundedRectDistance(vec2 uv, vec2 size, float radius) {
    vec2 d = abs(uv) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

// Screen bezel/border effect
float screenBorder(vec2 uv) {
    // Convert to centered coordinates (-0.5 to 0.5)
    vec2 centered = uv - 0.5;
    
    // Define the screen area (slightly inset from edges)
    vec2 screenSize = vec2(0.5 - screen_border_width);
    
    // Calculate distance to rounded rectangle
    float dist = roundedRectDistance(centered, screenSize, screen_corner_radius);
    
    // Create smooth border transition
    float border = smoothstep(-0.01, 0.01, dist);
    
    return 1.0 - border;
}

// Subtle screen reflection effect
vec3 screenReflection(vec2 uv) {
    // Create a subtle diagonal reflection pattern
    float reflection = sin((uv.x + uv.y) * 20.0 + time * 2.0) * 0.5 + 0.5;
    reflection = pow(reflection, 4.0) * screen_reflection_intensity;
    
    // Fade reflection towards screen edges
    float edgeFade = smoothstep(0.0, 0.2, min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y)));
    reflection *= edgeFade;
    
    return vec3(reflection);
}

void main()
{
    vec2 currentTexCoord = fragTexCoord;
    vec3 sampledColor = vec3(0.0);
    
    // Calculate screen border mask
    float screenMask = screenBorder(currentTexCoord);
    
    // Only process pixels inside the screen area
    if (screenMask > 0.0 && currentTexCoord.x >= 0.0 && currentTexCoord.x <= 1.0 && 
        currentTexCoord.y >= 0.0 && currentTexCoord.y <= 1.0) {
        
        // Sample the game texture
        sampledColor = texture(texture0, currentTexCoord).rgb * fragColor.rgb;
        
        // Apply scanline effect
        float scanlineFactor = ScanlineEffect(currentTexCoord, resolution);
        sampledColor *= scanlineFactor;
        
        // Add subtle screen reflection
        sampledColor += screenReflection(currentTexCoord);
        
        // Apply screen mask (fades to black at screen edges)
        sampledColor *= screenMask;
        
    } else {
        // Outside screen area - draw the bezel/border
        float borderGradient = smoothstep(0.0, screen_border_width * 2.0, 
            min(min(currentTexCoord.x, 1.0 - currentTexCoord.x), 
                min(currentTexCoord.y, 1.0 - currentTexCoord.y)));
        
        // Mix between border color and inner color
        sampledColor = mix(screen_border_color, screen_inner_color, borderGradient);
        
        // Add subtle highlight to the border for depth
        float highlight = pow(1.0 - borderGradient, 2.0) * 0.1;
        sampledColor += vec3(highlight);
    }
    
    finalColor = vec4(clamp(sampledColor, 0.0, 1.0), fragColor.a);
}