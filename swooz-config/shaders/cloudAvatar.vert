#version 330


uniform mat4 mvpMatrix;
uniform vec3 uniColor;

in vec3 vertex;
in vec3 color;

out vec3 c;

void main( void )
{
	gl_Position = mvpMatrix * vec4(vertex,1.0);

        if(uniColor.r < -0.5f || uniColor.g < -0.5f ||  uniColor.b < -0.5f)
        {
            c = color;
        }
        else
        {
            c = uniColor;
        }
}
