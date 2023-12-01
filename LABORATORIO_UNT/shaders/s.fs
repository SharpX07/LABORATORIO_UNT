#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals; 
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos; 

void main()
{
    vec3 lightColor = vec3(1.0);
    float ambientStrength = 0.1;
    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normals);  
    
    float diffuseLight = max(dot(Normals, lightDir), 0.0);
        
    float specularStrength = 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
        
    vec3 ambient = ambientStrength * lightColor;   
    vec3 result = (ambient + diffuseLight * lightColor + specular) * vec4(texture(texture_diffuse1, TexCoords)).rgb;
    
    FragColor = vec4(result, 1.0);
}