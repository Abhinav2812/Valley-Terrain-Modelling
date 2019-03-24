#version 330 core
layout (location=0) in vec4 position;
layout (location=1) in vec4 Color;
layout (location=2) in vec3 Normal;
layout (location=3) in vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 Trans;
out vec4 color;
out vec2 texCoord;
out vec3 normal;
out vec4 fragPos;
void main()
{
    gl_Position = proj* view * model * position;
    color = Color;
    texCoord = TexCoord;
    normal = Normal;
    fragPos = model * position;
}