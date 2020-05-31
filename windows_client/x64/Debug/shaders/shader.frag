#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;
in vec3 fragNormal;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 color;
uniform vec3 LightDirection[2] = vec3 [2] (normalize(vec3(1, 5, 2)), normalize(vec3(0, 3, 0)));
uniform vec3 LightColor[2] = vec3[2](vec3(1, 0, 0), vec3(0, 0, 1));

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    // Compute irradiance (sum of ambient & direct lighting)
    vec3 irradiance1 = AmbientColor + LightColor[0] * max(0, dot(LightDirection[0], fragNormal));
    vec3 irradiance2 = AmbientColor + LightColor[1] * max(0, dot(LightDirection[1], fragNormal));
    
    // Diffuse reflectance
    vec3 reflectance = irradiance1 * color + irradiance2 * color;
    
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
    fragColor = vec4(color, sampleExtraOutput);
}
