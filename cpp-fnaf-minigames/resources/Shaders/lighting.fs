#version 330 core

in vec2 fragTexCoord;
in vec3 fragPosition;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.3 * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    finalColor = vec4(result, texColor.a);
}