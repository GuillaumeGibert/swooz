#version 330


uniform mat4 mvpMatrix;
uniform vec3 uniColor;

uniform int displayMode = 0;

in vec3 vertex;
in vec3 color;

out vec3 PointColor;


void main( void )
{
	gl_Position = mvpMatrix * vec4(vertex,1.0);       

        if(displayMode == 0)
        {
            PointColor = color;
        }
        else
        {
            PointColor = uniColor;
        }
}
