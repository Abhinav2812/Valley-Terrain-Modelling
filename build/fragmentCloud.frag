#version 330 core
out vec4 fragColor;
in vec2 texCoord;
uniform sampler2D theTexture;
void main()
{
    fragColor = texture(theTexture,texCoord);
}