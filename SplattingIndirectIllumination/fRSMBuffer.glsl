#version 410

in vec4 vPositionWorld;
in vec4 vNormalWorld;

uniform vec3 materialColor;

void main()
{
    vec3 vNormal = normalize(vNormalWorld.xyz);

    gl_FragData[0] = vPositionWorld;
	gl_FragData[1] = vec4(vNormal, 0.0);
	gl_FragData[2] = vec4(materialColor, 1.0);
}
