#version 330


uniform mat4 mvpMatrix;

uniform mat4 transformation;

uniform vec3 uniColor;
//uniform

uniform int displayMode = 0;
uniform vec3 translationToCenter = vec3(0,0,0);
uniform bool applyTransformation = false;

in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 texture_coord;

uniform sampler2D texture2d;

out vec3 Normal;
out vec3 Vertex;
out vec2 TextureCoord;
out vec3 PointColor;

void main( void )
{	
    Normal  = normal;
    Vertex  = vertex;
    TextureCoord = texture_coord;

    if(displayMode == 0)
    {
        PointColor = color;
    }
    else if(displayMode == 1)
    {
        PointColor = uniColor;
    }

    vec4 newPosVertex = vec4(translationToCenter + vertex, 1.0);

    if(applyTransformation)
    {
        newPosVertex = newPosVertex * transformation;
        newPosVertex.x += transformation[3][0];
        newPosVertex.y += transformation[3][1];
        newPosVertex.z += transformation[3][2];
    }

    newPosVertex = newPosVertex - vec4(translationToCenter,1.0);


    gl_Position = mvpMatrix * vec4(newPosVertex.xyz,1.0);
}
