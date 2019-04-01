#version 330 core
out vec4 fragColor;
in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec4 fragPos;
uniform sampler2D theTexture;
uniform vec3 sunRayDirn;
void main()
{
    vec4 fragColorTemp = mix(texture(theTexture,texCoord),color,0);
    float ambient = 0.5;
    vec3 norm = normalize(normal);
    float diffuse = max(dot(norm,-sunRayDirn),0.0);
    vec3 result = (ambient+diffuse)*fragColorTemp.xyz;
    fragColor = vec4(result,fragColorTemp.a);
}