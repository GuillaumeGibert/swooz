#version 330

layout(location = 0, index = 0) out vec4 fragColor;


// from vertex shader
in vec3 Normal;
in vec3 Vertex;
in vec2 TextureCoord;

uniform int displayMode = 0;

// colors
uniform float opacity = 1.0;
vec4 defaultColor = vec4(1.0,1.0,1.0,0.0);

// lights
//  coeffs
uniform float alpha    = 1.0; // shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
uniform float kAmbiant = 0.5; // ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered
uniform float kDiffus  = 0.7;// diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance)
uniform float kSpecular= 0.3; // specular reflection constant, the ratio of reflection of the specular term of incoming light
uniform float pSpecular= 100.0;
//  values
uniform vec3 lAmbiant  = vec3(0.3,0.3,0.3);
uniform vec3 lDiffus   = vec3(1.0,1.0,1.0);
uniform vec3 lSpecular = vec3(1.0,1.0,1.0);
//  sources
uniform vec3 lSourcePos = vec3(0.1,0.1,-0.13);

// texture
uniform bool applyTexture = false;
uniform sampler2D texture2d;

// camera
//uniform vec3 eyePosition;
uniform vec3 viewDirection;


void main(void)
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = lSourcePos - Vertex; // light direction
    float distance = length(lightDir);   // distance
    lightDir = lightDir / distance;      // normalize light direction
    distance = distance * distance;

    // ambiant
    vec3 ambiantLight = lAmbiant * kAmbiant;// / sqrt(distance);

    // diffus
    float iDiffus = 0.0; // intensity diffus
    iDiffus = clamp( dot( normal, lightDir ) , 0.0 ,1.0);
    vec3 diffusLight = iDiffus * lDiffus * kDiffus / distance;

    // specular
    vec3 h = normalize(lightDir + viewDirection); // compute half vector
    float iSpecular = pow( clamp( dot( normal, h), 0.0, 1.0 ), pSpecular ); // intensity specular
    vec3 specularLight = iSpecular * lSpecular * kSpecular / distance;

    //  texture
    vec4 textureColor = texture2D(texture2d, TextureCoord);

    if(displayMode != 2)
    {
        fragColor = defaultColor * vec4(ambiantLight + diffusLight + specularLight, opacity);
    }
    else
    {
        fragColor = textureColor * vec4(ambiantLight + diffusLight , opacity);
    }
}
