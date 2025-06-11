#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;

// Output fragment color
out vec4 finalColor;

// NOTE: Render size values must be passed from code
const float renderWidth = 800;
const float renderHeight = 450;
float offset = 0.0;

// Screen parameters - big black margin and rounded corners
const float screen_margin = 0.04;  // Large black border
const float corner_radius = 0.03;  // Rounded screen corners

// --- Helper Functions ---

// Puffy/bulge effect - pushes edges outward but keeps rectangular shape
vec2 puffyDistortion(vec2 coord) {
    vec2 centered = coord - 0.5;
    
    // Create a subtle outward push at the edges
    float edgeDistance = max(abs(centered.x), abs(centered.y));
    float puffFactor = 1.0 + (edgeDistance * edgeDistance * 0.15);
    
    return 0.5 + centered * puffFactor;
}

// Check if coordinate is within the rounded screen area
float isInScreen(vec2 coord) {
    vec2 centered = coord - 0.5;
    vec2 screenSize = vec2(0.5 - screen_margin);
    
    // Distance to rounded rectangle
    vec2 d = abs(centered) - screenSize + corner_radius;
    float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - corner_radius;
    
    return smoothstep(0.01, -0.01, dist);
}

// White ray glare at corners (like the reference image)
vec3 cornerGlare(vec2 uv) {
    vec3 glare = vec3(0.0);
    
    // Bottom-left corner glare
    vec2 blCorner = uv - vec2(0.0, 1.0);
    float blGlare = 1.0 - length(blCorner * vec2(1.5, 0.8));
    blGlare = pow(max(blGlare, 0.0), 3.0) * 0.3;
    
    // Top-right corner glare  
    vec2 trCorner = uv - vec2(1.0, 0.0);
    float trGlare = 1.0 - length(trCorner * vec2(0.8, 1.5));
    trGlare = pow(max(trGlare, 0.0), 3.0) * 0.3;
    
    // Subtle edge highlights
    float leftEdge = smoothstep(0.05, 0.0, uv.x) * 0.1;
    float rightEdge = smoothstep(0.95, 1.0, uv.x) * 0.1;
    float topEdge = smoothstep(0.05, 0.0, uv.y) * 0.1;
    float bottomEdge = smoothstep(0.95, 1.0, uv.y) * 0.1;
    
    glare = vec3(blGlare + trGlare + leftEdge + rightEdge + topEdge + bottomEdge);
    
    return glare;
}

void main()
{
    vec2 currentCoord = fragTexCoord;
    
    // Check if we're in the screen area
    float screenMask = isInScreen(currentCoord);
    
    if (screenMask > 0.0) {
        // Apply puffy distortion to texture coordinates
        vec2 puffyCoord = puffyDistortion(currentCoord);
        
        // Check if puffy coordinates are still valid
        bool validCoord = puffyCoord.x >= 0.0 && puffyCoord.x <= 1.0 && 
                          puffyCoord.y >= 0.0 && puffyCoord.y <= 1.0;
        
        if (validCoord) {
            float frequency = renderHeight/3.0;
            
            // Scanlines using puffy coordinates
            float globalPos = (puffyCoord.y + offset) * frequency;
            float wavePos = cos((fract(globalPos) - 0.5)*3.14);
            
            // Sample texture with puffy distortion
            vec4 texelColor = texture(texture0, puffyCoord);
            vec4 scanlineColor = mix(vec4(0.0, 0.3, 0.0, 0.0), texelColor, wavePos);
            
            // Add corner glare using original coordinates
            vec3 glare = cornerGlare(currentCoord);
            scanlineColor.rgb += glare;
            
            // Apply screen mask for smooth edges
            finalColor = scanlineColor * screenMask;
        } else {
            finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    } else {
        // Big black margin area
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}