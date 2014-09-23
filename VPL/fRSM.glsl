#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

void main()
{
    vec3 normal = normalize(gNormalWorld.xyz);
    normal.xyz = (normal.xyz + 1.0) * 0.5;

    gl_FragData[0] = gPositionWorld;
    gl_FragData[1] = vec4(normal, 0.0);
}