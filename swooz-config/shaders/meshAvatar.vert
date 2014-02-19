#version 330


uniform mat4 mvpMatrix;

in vec3 vertex;
in vec3 normal;
in vec2 texture_coord;

uniform sampler2D texture2d;

out vec3 Normal;
out vec3 Vertex;
out vec2 TextureCoord;

void main( void )
{	
    Normal  = normal;
    Vertex  = vertex;
    TextureCoord = texture_coord;

    gl_Position = mvpMatrix * vec4(vertex,1.0);
}
