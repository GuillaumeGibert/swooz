#version 330

layout(location = 0, index = 0) out vec4 fragColor;


in vec3 N;
in vec3 V;
in vec4 T;
in vec2 TC;

uniform float opacity = 1.f;

uniform vec3 lAmbiant       = vec3(0.3,0.3,0.3);
uniform vec3 lDiffus        = vec3(1.0,1.0,1.0);
uniform vec3 lSpeculaire    = vec3(1.0,1.0,1.0);
uniform vec3 lSourcePos     = vec3(0.0,0.0,-2.0);
//uniform vec3 eyePosition;

uniform float kd    = 0.5;
uniform float ks    = 0.3;

void main(void)
{
    /////////////////////////////// blinn phong


    vec3 L = normalize(lSourcePos-V);
//    vec3 E = normalize(eyePosition);
    vec3 R = normalize(-reflect(L,N));
    float alpha = dot(normalize(N), L);
    alpha = smoothstep(0, 1,alpha);

    float lDist =  length(L);

    ////////////////////////////// ambiant

    vec3 ambiantLight = lAmbiant * kd;// / lDist;

    ////////////////////////////// diffuse

    vec3 diffusLight = lDiffus * alpha * kd;// / lDist;

    ////////////////////////////// specular

    vec3 specularLight = vec3(0,0,0);//lSpeculaire * ks * pow(max(dot(R,E),0.0),0.3);// / lDist;

    /////////////////////////////// total

    fragColor = vec4(ambiantLight + diffusLight + specularLight, opacity);
//    fragColor = vec4(specularLight, opacity);
}


//struct gl_LightSourceParameters
//{
//   vec4 ambient;              // Aclarri
//   vec4 diffuse;              // Dcli
//   vec4 specular;             // Scli
//   vec4 position;             // Ppli
//   vec4 halfVector;           // Derived: Hi
//   vec3 spotDirection;        // Sdli
//   float spotExponent;        // Srli
//   float spotCutoff;          // Crli
//                              // (range: [0.0,90.0], 180.0)
//   float spotCosCutoff;       // Derived: cos(Crli)
//                              // (range: [1.0,0.0],-1.0)
//   float constantAttenuation; // K0
//   float linearAttenuation;   // K1
//   float quadraticAttenuation;// K2
//};


//uniform gl_LightSourceParameters gl_LightSource[gl_MaxLights];



//struct gl_MaterialParameters
//{
//   vec4 emission;    // Ecm
//   vec4 ambient;     // Acm
//   vec4 diffuse;     // Dcm
//   vec4 specular;    // Scm
//   float shininess;  // Srm
//};


//uniform gl_MaterialParameters gl_FrontMaterial;
//uniform gl_
//struct gl_LightModelProducts
//{
//   vec4 sceneColor; // Derived. Ecm + Acm * Acs
//};


//uniform gl_LightModelProducts gl_FrontLightModelProduct;
//uniform gl_LightModelProducts gl_BackLightModelProduct;


//struct gl_LightProducts
//{
//   vec4 ambient;    // Acm * Acli
//   vec4 diffuse;    // Dcm * Dcli
//   vec4 specular;   // Scm * Scli
//};


//uniform gl_LightProducts gl_FrontLightProduct[gl_MaxLights];
//uniform gl_LightProducts gl_BackLightProduct[gl_MaxLights];


//varying vec3 N;
//varying vec3 v;

//void main (void)
//{
//   vec3 L = normalize(gl_LightSource[0].position.xyz - v);
//   vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
//   vec3 R = normalize(-reflect(L,N));

//   //calculate Ambient Term:
//   vec4 Iamb = gl_FrontLightProduct[0].ambient;

//   //calculate Diffuse Term:
//   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
//   Idiff = clamp(Idiff, 0.0, 1.0);

//   // calculate Specular Term:
//   vec4 Ispec = gl_FrontLightProduct[0].specular
//                * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
//   Ispec = clamp(Ispec, 0.0, 1.0);

//   // write Total Color:
//   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;
//}


