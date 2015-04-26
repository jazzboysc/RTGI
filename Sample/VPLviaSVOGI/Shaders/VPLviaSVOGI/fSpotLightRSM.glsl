#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

out vec4[3] Output;

uniform vec3 MaterialColor;

void main()
{
    vec3 normal = normalize(vNormalWorld.xyz);
    normal.xyz = (normal.xyz + 1.0) * 0.5;
    Output[0] = vPositionWorld;
    Output[1] = vec4(normal, 0.0);
    Output[2] = vec4(MaterialColor, 1.0);
}