#version 330 core
out vec4 fragColor;
in vec4 color;
in vec2 texCoord;
uniform sampler2D theTexture;
void main()
{
    vec4 temp = vec4(color.rgb,texture(theTexture,texCoord).a);
    if(temp.a<.5f)
        discard;
    fragColor = temp;
}
