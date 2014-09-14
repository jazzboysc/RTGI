#version 410

in vec4 vPositionWorld;
in vec4 vNormalWorld;

uniform vec3 MaterialColor;

void main()
{
    vec3 vNormal = normalize(vNormalWorld.xyz);
    vec3 lightDir = vec3(0.0, 10.0, 0.0) - vPositionWorld.xyz;
    float len = length(lightDir);
    lightDir = lightDir / len;
    float d = max(0.0, dot(lightDir, vNormal));
    vNormal = vNormal*0.5 + 0.5;

    gl_FragData[0] = vPositionWorld;
	gl_FragData[1] = vec4(vNormal, 0.0);
    gl_FragData[2] = vec4(MaterialColor * d, 1.0);
}
