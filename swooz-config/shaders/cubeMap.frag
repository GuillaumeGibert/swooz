#version 330

layout(location = 0, index = 0) out vec4 fragColor;


// from vertex shader
in vec2 TextureCoord;

uniform sampler2D texture2d;

void main(void)
{
    //  texture
        vec4 textureColor = texture2D(texture2d, TextureCoord);
        fragColor = textureColor;
//        fragColor = vec4(2,0,1, 1);
}
