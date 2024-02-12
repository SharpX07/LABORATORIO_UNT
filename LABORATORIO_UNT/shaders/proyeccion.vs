#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normals;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


mat3 getRotationMatrix(mat4 m) {
    return mat3(m[0].xyz, m[1].xyz, m[2].xyz);
}

void main()
{
    mat4 matriz;
    

    // Inicializar la matriz
    matriz[0] = vec4(0.0, 0.0, 0.0, 0.0);
    matriz[1] = vec4(0.0, 1.0, 0.0, 0.0);
    matriz[2] = vec4(0.0, 0.0, 1.0, 0.0);
    matriz[3] = vec4(0.0, 0.0, 0.0, 1.0);

    



    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Obtener la rotaci�n directamente de la matriz de modelo
    Normals = mat3(transpose(inverse(model))) * aNormal;
    vec4 prev = model * vec4(0,0,0, 1.0);
    vec4 nnn =matriz * model * vec4(aPos, 1.0);
    
    nnn.x = prev.x;
    gl_Position = projection * view*nnn;
}