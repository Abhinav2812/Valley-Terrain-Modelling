#version 330 core
layout (location=0) in vec4 position;
layout (location=1) in vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out vec2 texCoord;
void main()
{
    gl_Position = proj* view * model * position;
    //if(gl_Position.x>1 || gl_Position.x<-1 || gl_Position.y>1 || gl_Position.y<-1 || gl_Position.z>1 || gl_Position.z<-1)
      //  gl_Position = position.xzyw;
    texCoord = TexCoord;
}