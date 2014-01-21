#version 330


uniform mat4 mvpMatrix;

in vec3 vertex;
in vec3 normal;
in vec2 texture_coord;

uniform sampler2D texture2d;

out vec3 N;
out vec3 V;
out vec4 T;
out vec2 TC;

void main( void )
{	
    N  = normal;
    V  = vertex;
    T  = texture(texture2d, texture_coord.st);
    TC = texture_coord;

    gl_Position = mvpMatrix * vec4(vertex,1.0);
}
