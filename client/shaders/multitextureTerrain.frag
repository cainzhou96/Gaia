#version 330

layout(location = 0) out vec4 glFragColor;

//! <group name="Camera Properties"/>
uniform vec3 CameraPosition = vec3 (120, 158, 42); //!slider[(-100,-100,-100), (0, 0,0), (100,100,100)]

//! <group name="Object Properties"/>
uniform float SpecularPower = 300.0; //! slider[10, 300, 1000]
uniform vec3 Ka = vec3(0.7, 0.7, 0.7); //! color[0.7, 0.7, 0.7]
uniform vec3 Kd = vec3(1.0, 1.0, 1.0); //! color[(0.9, 0.8, 1.0]
uniform vec3 Ks = vec3(0.5, 0.5, 0.5); //! color[0.7, 0.7, 0.7]
uniform vec3 Ke = vec3(0.0, 0.0, 0.0); //! color[0.7, 0.7, 0.7]
uniform vec3 baseColor = vec3(0.7); //! color[0.7, 0.7, 0.7]

//! <group name="Light Properties"/>
uniform vec3 LightPosition = vec3 (125.5f, 40.0, -125.5f);
uniform vec3 Ia = vec3 (0.3, 0.3, 0.3); //! color[0.3, 0.3, 0.3]
uniform vec3 Id = vec3 (0.7, 0.7, 0.7); //! color[1.0, 1.0, 1.0]
uniform vec3 Is = vec3 (0.5, 0.5, 0.5); //! color[0.7, 0.7, 0.7]

//! <group name="Border Properties"/>
uniform vec3 borderColor = vec3(0.0); //! color[0.0, 0.0, 0.0]
uniform float borderTolerance = 0.0001; //! slider[0.0001, 0.5, 0.9999]

//! <group name="Color and Specular Steps"/>
uniform int colorSteps = 5; //! slider[1, 5, 10]
uniform int specularSteps = 10; //! slider[1, 5, 10]


//! <group name="Usage Properties"/>
uniform bool manualCamPos = true;  //! checkbox[false]
uniform bool shading = true;  //! checkbox[true]
uniform bool usePhongBlinn = true; //! checkbox[true]
uniform bool seeOutLine = true; //! checkbox[true]
uniform bool seeColorSteps = true; //! checkbox[false]
uniform bool seeSpecularSteps = true; //! checkbox[true]

// From Vertex shader
in vec3 vPosition;
in vec3 vNormal;
in vec2 pass_textureCoord;

uniform sampler2D backgroundTexture;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D cracks;
uniform sampler2D heightMap;
uniform sampler2D snow;

vec3 N;
vec3 V;
vec3 L;
float LN;
float CamOBJNorm;

// Methods
// Phong Shade declaration
vec3 shadePhong(vec3 diffColor);
// Phong-Blinn Shade declaration
vec3 shadePhongBlinn(vec3 diffColor);

vec4 getFinalColor(vec3 diffColor);

void main()
{

    float height = texture(heightMap, pass_textureCoord).r;
    float border = smoothstep(0.4, 0.6, height);

    float border2 = smoothstep(0.7, 1.0, height);

    vec3 color1 = texture(grass, pass_textureCoord * 3.0).rgb;
	vec3 color2 = texture(rock, pass_textureCoord * 7.0).rgb;
    vec3 color3 = texture(cracks, pass_textureCoord * 5.0).rgb;
    vec3 color4 = texture(snow, pass_textureCoord * 6.0).rgb;

    // seperate grass and rock
	vec3 color = color1 * (1.0 - border) + color2 * border;

    // add crack and seperate grass, rock, cracks with snow
    color *= 0.5 + 0.5 * color3.r;
    color = color * (1-border2) +  color * border2;

    // sperate snow
    color = color * (1.0 - border2) + color4 * border2;

	color *= 0.5 + 0.5 * height;

    glFragColor = getFinalColor(color);

    //glFragColor = vec4(color, 1.0f);
}

vec4 getFinalColor(vec3 diffColor)
{
    if (manualCamPos)
    {
        V = normalize(CameraPosition-vPosition);
    }
    else
    {
        V = normalize(-vPosition);
    }


    N = normalize (vNormal);
    L = normalize(LightPosition - vPosition);
    CamOBJNorm = max(0.0f, dot(N,V));
    LN = dot(L,N);

    if (seeColorSteps)
    {
        LN = ceil(LN * colorSteps) / colorSteps;
    }

    if (seeOutLine && CamOBJNorm <= borderTolerance)
    {
        return vec4(borderColor, 1.0);
//        return vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }
    else if (shading)
    {
        if (usePhongBlinn)
        {
            return vec4(shadePhongBlinn(diffColor), 1.0);
        }
        else
        {
            return vec4(shadePhong(diffColor), 1.0);
        }
    }

    return vec4(baseColor * diffColor, 1.0);

}

vec3 shadePhong(vec3 diffColor)
{
    vec3 c = vec3(0.0);

    // Ambiental: Ka * Ambient Color (Ia)
    c = Ia * Ka;

    // Diffuse: Kd * LightColorDiffuse (Id) * max(0, dot(N,L))
    vec3 diffuse = Id * Kd * max(0.0, LN);
    c += clamp(diffuse, 0.0, 1.0) * diffColor;

    // Specular: Ks * LightColorSpecular (Is) * pow(max (0, dot(R,V)), SpecularPower)
    vec3 R = reflect(-L, N);
    float RV = dot (R,V);

    if (seeSpecularSteps)
    {
        RV = ceil(RV * specularSteps) / specularSteps;
    }

    float factor = max (RV, 0.01); // en apuntes: max (0.0, RV);
    vec3 specular = Is*Ks*pow(factor, SpecularPower);

    c += clamp(specular, 0.0, 1.0);

    // Emissive: Ke
    c+=Ke;

    return c;
}

vec3 shadePhongBlinn(vec3 diffColor)
{
    vec3 c = vec3(0.0);

    // Ambiental: Ka * Ambient Color (Ia)
    c = Ia * Ka;

    // Diffuse: Kd * LightColorDiffuse (Id) * max(0, dot(N,L))
    vec3 diffuse = Id * Kd * max(0.0, LN);
    c += clamp(diffuse, 0.0, 1.0) * diffColor;

    // Phong-Blinn changes specular
    // Specular: Ks * LightColorSpecular (Is) * pow(max (0, dot(R,V)), SpecularPower)
    vec3 halfAngleVector = normalize(L + V);
    float HAVN= dot (N, halfAngleVector);

    if (seeSpecularSteps)
    {
        HAVN = ceil(HAVN * specularSteps) / specularSteps;
    }

    float factor = max (HAVN, 0.01);
    vec3 specular = Is*Ks*pow(factor, SpecularPower);
    c += clamp(specular, 0.0, 1.0);

    // Emissive: Ke
    c+=Ke;

    return c;
}
