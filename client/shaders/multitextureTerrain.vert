#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 vPosition;
out vec3 vNormal;
out vec2 pass_textureCoord;

void main()
{
    mat4 modelView = view * model;

    pass_textureCoord = texCoord;
    
    vPosition = (model * vec4(position, 1.0)).xyz;
//    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
//    vNormal = normalMatrix * normal;
    vNormal = normal;

    gl_Position =  projection * view * vec4(vPosition, 1.0);
}
