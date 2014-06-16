#version 330



uniform mat4 mvpMatrix;
uniform mat4 transformation;
uniform vec3 uniColor;
uniform vec3 translationToCenter = vec3(0,0,0);


uniform bool applyTransformation = false;
uniform int displayMode = 0;

in vec3 vertex;
in vec3 color;

out vec3 PointColor;

void main( void )
{        
    if(displayMode == 0)
    {
        PointColor = color;
    }
    else
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
