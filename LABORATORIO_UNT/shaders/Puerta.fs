#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
float random(float seed) {
    return fract(sin(seed) * 43758.5453123);
}
void main()
{ 
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * vec3(1.0);

    // Diffuse 
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diff * vec3(1.0);

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularColor = specularStrength * spec * vec3(1.0);

    // Final color calculation
    vec3 resultColor = (ambientColor + diffuseColor + specularColor);
    FragColor = vec4(resultColor, 1.0) * texture(texture_diffuse1, TexCoords); 
}
