#version 430 core

in vec4 vNormalWorld;
in vec4 vPositionWorld;
in vec4 vPositionNDCCache;

out vec4[4] Output;

uniform vec3 MaterialColor;

void main()
{
    vec3 vNormal = normalize(vNormalWorld.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;
    vec3 vRPC = vPositionNDCCache.xyz / vPositionNDCCache.w;
    vRPC = (vRPC + 1.0) * 0.5;

    Output[0] = vPositionWorld;
    Output[1] = vec4(vNormal, 0.0);
    Output[2] = vec4(MaterialColor, 1.0);
    Output[3] = vec4(vRPC.xyz, 1.0);
}
