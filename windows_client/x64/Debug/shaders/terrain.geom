#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// From Vertex shader
//in vec3 vPos[];
//in vec3 vN[];

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec3 tNormal;
out vec3 vPosition;
//out vec3 vNormal;

vec3 calcTriangleNormal(){
    vec3 tangent1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 tangent2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = cross(tangent1, tangent2);
    return normalize(-normal);
}

void main()
{
    mat4 modelView = view * model;
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    
    for (int i = 0; i < 3; i++){
        vPosition = (modelView * gl_in[i].gl_Position).xyz;
        tNormal = normalMatrix * calcTriangleNormal();
        
        gl_Position =  projection * vec4(vPosition, 1.0);
    
        EmitVertex();
    }
    
    EndPrimitive();
}


