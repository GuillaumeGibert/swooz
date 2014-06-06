#version 330


uniform mat4 mvpMatrix;
uniform vec3 uniColor;

uniform int displayMode = 0;


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

    gl_Position = mvpMatrix * vec4(vertex,1.0);
}
