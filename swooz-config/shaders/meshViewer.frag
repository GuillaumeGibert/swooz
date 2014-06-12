#version 330

layout(location = 0, index = 0) out vec4 fragColor;


uniform int displayMode = 0;

// from vertex shader
in vec3 Normal;
in vec3 Vertex;
in vec2 TextureCoord;

// colors
uniform float opacity = 1.0;
vec4 defaultColor = vec4(1.0,1.0,1.0,0.0);
in vec3 PointColor;



// lights
//  coeffs
uniform float alpha    = 1.0; // shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
uniform float kAmbiant = 1.0; // ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered
uniform float kDiffus  = 0.5;// diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance)
uniform float kSpecular= 0.3; // specular reflection constant, the ratio of reflection of the specular term of incoming light
//  values
uniform vec3 lAmbiant  = vec3(0.3,0.3,0.3);
uniform vec3 lDiffus   = vec3(1.0,1.0,1.0);
uniform vec3 lSpecular = vec3(1.0,1.0,1.0);
//  sources
uniform vec3 lSourcePos = vec3(0.0,0.0,-0.9);

// texture
uniform bool applyTexture = false;
uniform sampler2D texture2d;

// camera
//uniform vec3 eyePosition;


void main(void)
{
    // blinn phong
        // ambiant
            vec3 ambiantLight = lAmbiant * kAmbiant;

        // diffus
            vec3 lightDir = lSourcePos - Vertex;
            float distance = length(lightDir);
            lightDir = normalize(lightDir);
            vec3 diffusLight = lDiffus * clamp(dot(normalize(Normal), lightDir),0.0, 1.0) * kDiffus / (distance*distance);
        // specular
            // ...

    //  texture
        vec4 textureColor = texture2D(texture2d, TextureCoord);

        fragColor = vec4(PointColor, 1.0) * vec4(ambiantLight + diffusLight , opacity);

//        if(displayMode != 3)
//        {
//            fragColor = vec4(PointColor, 1.0) * vec4(ambiantLight + diffusLight , opacity);
//        }
//        else
//        {
//            fragColor = textureColor * vec4(ambiantLight + diffusLight , opacity); // + specularLight
//        }
}

////Calculate the half vector between the light vector and the view vector.
////This is faster than calculating the actual reflective vector.
//float3 H = normalize( lightDir + viewDir );

////Intensity of the specular light
//float NdotH = dot( normal, H );
//intensity = pow( saturate( NdotH ), specularHardness );

////Sum up the specular light factoring
//OUT.Specular = intensity * light.specularColor * light.specularPower / distance;
