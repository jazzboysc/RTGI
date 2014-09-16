#version 410

in vec4 vPositionWorld;
in vec4 vNormalWorld;

uniform vec3 MaterialColor;

void main()
{
    vec3 vNormal = normalize(vNormalWorld.xyz);
    vNormal.xyz = vNormal.xyz*0.5 + 0.5;

    gl_FragData[0] = vPositionWorld;
    gl_FragData[1] = vec4(vNormal, 0.0);
    gl_FragData[2] = vec4(MaterialColor, 1.0);
}
