#version 330 core
layout (location=0) in vec4 position;
layout (location=1) in vec4 Color;
layout (location=2) in vec3 Normal;
layout (location=3) in vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 Trans;
uniform bool isWater;
uniform float t;
out vec4 color;
out vec2 texCoord;
out vec2 texCoordShadow;
out vec3 normal;
out vec4 fragPos;
void main()
{
    vec4 tmpposition = position;
    color = Color;
    texCoord = TexCoord;
    normal = Normal;
    if(isWater)
    {
        // first pattern
        /*
        tmpposition.y = position.y+.3*sin(position.x+t);
        normal.y = .1f;
        normal.x = -.1*cos(position.x+t);
        normal.z = 0;
        normal = normalize(normal);
        */
        //second pattern
        tmpposition.y = position.y+.15*(sin(position.x+position.z+t)+sin(position.x-position.z+t)+sin(-position.x+position.z+t));
        normal.y = .1f;
        normal.x = -.1*(cos(position.x+position.z+t)+cos(position.x-position.z+t)-cos(-position.x+position.z+t));
        normal.z = -.1*(cos(position.x+position.z+t)-cos(position.x+position.z+t)+cos(-position.x+position.z+t));
        normal = normalize(normal);
    }
    fragPos = model * position;
    vec2 TexCoordShadow = TexCoord/12.f;
    texCoordShadow = TexCoordShadow;
    gl_Position = proj* view * model * tmpposition;
}