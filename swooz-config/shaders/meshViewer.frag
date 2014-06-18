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
//uniform float alpha    = 1.0; // shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
uniform float kAmbiant = 1.0; // ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered
uniform float kDiffus  = 0.5;// diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance)
uniform float kSpecular= 1.0; // specular reflection constant, the ratio of reflection of the specular term of incoming light
uniform float pSpecular= 100.0;
//  values
uniform vec3 lAmbiant  = vec3(0.3,0.3,0.3);
uniform vec3 lDiffus   = vec3(1.0,1.0,1.0);
uniform vec3 lSpecular = vec3(1.0,1.0,1.0);
//  sources
uniform vec3 lSourcePos = vec3(0.0,0.0,-0.5);

// texture
uniform bool applyTexture = false;
uniform sampler2D texture2d;

// camera
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
//        if(dot(viewDirection, normal) >= 0.0)
//        {
            iDiffus = clamp( dot( normal, lightDir ) , 0.0 ,1.0);
//        }

        vec3 diffusLight = iDiffus * lDiffus * kDiffus / distance;

    // specular
        vec3 h = normalize(lightDir + viewDirection); // compute half vector
        float iSpecular = pow( clamp( dot( normal, h), 0.0, 1.0 ), pSpecular ); // intensity specular
        vec3 specularLight = iSpecular * lSpecular * kSpecular / distance;

    //  texture
        vec4 textureColor = texture2D(texture2d, TextureCoord);

        if(displayMode != 2)
        {
            fragColor = vec4(PointColor, 1.0) * vec4(ambiantLight + diffusLight + specularLight, opacity);
        }
        else
        {
            fragColor = textureColor * vec4(ambiantLight + diffusLight + specularLight, opacity);
        }
}

////Calculate the half vector between the light vector and the view vector.
////This is faster than calculating the actual reflective vector.
//float3 H = normalize( lightDir + viewDir );

////Intensity of the specular light
//float NdotH = dot( normal, H );
//intensity = pow( saturate( NdotH ), specularHardness );

////Sum up the specular light factoring
//OUT.Specular = intensity * light.specularColor * light.specularPower / l_distance;

////////////////////////////// specular

//    vec3 v = normalize(eyePosition-Vertex);

//    vec3 temp = lightDir+v;
////    if(temp[0] < 0) temp[0] = - temp[0];
////    if(temp[1] < 0) temp[1] = - temp[1];
////    if(temp[2] < 0) temp[2] = - temp[2];

//    vec3 h = (lightDir+v)/temp;


// blinn phong
    // ambiant
//            vec3 ambiantLight = lAmbiant * kAmbiant / l_distance;
//        // diffus
//            vec3 diffusLight = lDiffus * clamp(dot(normalize(Normal), lightDir),0.0, 1.0) * kDiffus / l_distance; //(l_distance*l_distance);
//        // specular
//            vec3 specularLight = pow(smoothstep(0, 1,dot(Normal, h)), specularCoeff) * kSpecular * lSpecular / l_distance;
