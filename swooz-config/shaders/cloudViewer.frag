#version 330

layout(location = 0, index = 0) out vec4 fragColor;

in vec3 PointColor;

uniform float opacity = 1.f;

uniform int displayMode = 0;

in vec2 TextureCoord;
uniform sampler2D texture2d;

void main( void )
{
    if(displayMode == 2)
    {
//        vec4 textureColor = texture2D(texture2d, TextureCoord);
//        fragColor = textureColor;
        fragColor =   texture2D(texture2d, TextureCoord);//vec4(PointColor, opacity);
    }
    else
    {
        fragColor = vec4(PointColor, opacity);
    }
}
