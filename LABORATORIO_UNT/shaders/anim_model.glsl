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
    vec3 lightColor=vec3(1.0);   
    float ambientStrength = 0.2;
    float diffuse = max(dot(normalize(Normals),normalize(lightPos-FragPos)),0.0);
    vec3 ambient = (ambientStrength+diffuse) * lightColor;
    
    float specularStrength = 0.6;
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(lightPos - FragPos);  
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 

    //FragColor = vec4(0.5,0.5,0.5,1); 
    
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0)*texture(texture_diffuse1, TexCoords);
    //FragColor = texture(texture_diffuse1, TexCoords);
}
