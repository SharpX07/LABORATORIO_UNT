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
    // vec3 lightPos=vec3(0,0,50);
    vec3 lightColor=vec3(1.0);   
    float ambientStrength = 0.1;
    vec3 lightDir = normalize(-lightPos);  
    float diffuseLight = max(dot(Normals, lightDir), 0.0);

    // specular
    float specularStrength = 1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightPos, Normals);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 ambient = (ambientStrength+diffuseLight) * lightColor;
    //FragColor = vec4(0.5,0.5,0.5,1); 
    FragColor = vec4(ambient,1.0) * texture(texture_diffuse1, TexCoords); 

    //FragColor = texture(texture_diffuse1, TexCoords); 
}