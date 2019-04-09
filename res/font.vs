#version 330 core
layout (location=0) in vec4 position;
layout (location=1) in vec4 Color;
layout (location=2) in vec4 Normal;
layout (location=3) in vec2 TexCoord;
out vec4 color;
out vec2 texCoord;
out vec4 normal;
void main()
{
    gl_Position = position;
    color = Color;
    texCoord = TexCoord;
    normal = Normal;
}
