#version 330


uniform mat4 mvpMatrix;

in vec3 vertex;
in vec2 texture_coord;

uniform sampler2D texture2d;

out vec2 TextureCoord;

void main( void )
{
    TextureCoord = texture_coord;
    gl_Position = mvpMatrix *vec4(vertex.xyz,1.0);
}



