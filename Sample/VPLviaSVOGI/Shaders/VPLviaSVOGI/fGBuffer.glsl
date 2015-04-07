#version 430 core

in vec4 vNormalWorld;
in vec4 vPositionWorld;

out vec4[3] Output;

uniform vec3 MaterialColor;

void main()
{
    vec3 vNormal = normalize(vNormalWorld.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;

    Output[0] = vPositionWorld;
    Output[1] = vec4(vNormal, 0.0);
    Output[2] = vec4(MaterialColor.rgb / 3.141593, 1.0);
}
