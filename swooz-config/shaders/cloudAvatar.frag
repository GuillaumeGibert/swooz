#version 330

layout(location = 0, index = 0) out vec4 fragColor;


in vec3 c;

uniform float opacity = 1.f;

void main( void )
{
    fragColor = vec4(c, opacity);
}
