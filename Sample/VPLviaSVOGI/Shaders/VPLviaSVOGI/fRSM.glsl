#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

out vec4[3] Output;

uniform vec3 LightColor;
uniform vec3 LightPositionWorld;
uniform vec3 MaterialColor;

void main()
{
    vec3 normal = normalize(gNormalWorld.xyz);
    vec3 lightDir = LightPositionWorld - gPositionWorld.xyz;
    lightDir = normalize(lightDir);
    float d = max(0.0, dot(lightDir, normal));

    normal.xyz = (normal.xyz + 1.0) * 0.5;
    Output[0] = gPositionWorld;
    Output[1] = vec4(normal, 0.0);
    Output[2] = vec4(d*MaterialColor*LightColor, 1.0);
}