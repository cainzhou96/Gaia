#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 vPosition;
out vec3 vNormal;

void main()
{
    mat4 modelView = view * model;
    
    vPosition = (modelView * vec4(position, 1.0)).xyz;
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vNormal = normalMatrix * normal;

    gl_Position =  projection * vec4(vPosition, 1.0);
}
