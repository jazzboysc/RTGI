#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

uniform vec3 LightPositionWorld;
uniform vec3 MaterialColor;

void main()
{
    vec3 normal = normalize(gNormalWorld.xyz);
    vec3 lightDir = LightPositionWorld - gPositionWorld.xyz;
    lightDir = normalize(lightDir);
    float d = max(0.0, dot(lightDir, normal));

    normal.xyz = (normal.xyz + 1.0) * 0.5;
    gl_FragData[0] = gPositionWorld;
    gl_FragData[1] = vec4(normal, 0.0);
    gl_FragData[2] = vec4(d*MaterialColor, 1.0);
}