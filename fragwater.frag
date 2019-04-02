#version 330 core
out vec4 fragColor;
in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec2 texCoordShadow;
in vec4 fragPos;
uniform sampler2D theTexture;
uniform sampler2D shadowTex;
uniform vec3 sunRayDirn;
void main()
{
    fragColor = color;
}