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
uniform bool isWater;
uniform vec3 viewPos;
void main()
{
    vec4 fragColorTemp = 0.6*texture(theTexture,texCoord)*texture(shadowTex,texCoordShadow);
    float specularWeight = .6;
    vec3 viewdir = normalize(viewPos-fragPos.xyz);
    float spec = 0;
    if(isWater)
    {
        fragColorTemp = color;
        spec = pow(max(dot(viewdir,-sunRayDirn),0.0),20);
    }
    vec4 specular = specularWeight*spec*vec4(1,1,1,1);
    float ambient = 0.1;
    vec3 norm = normalize(normal);
    float diffuse = .8*max(dot(norm,-sunRayDirn),0.0);
    vec3 result = (ambient+diffuse)*fragColorTemp.xyz+specular.xyz;
    fragColor = vec4(result,fragColorTemp.a+specular.a);
}