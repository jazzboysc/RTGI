#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

void main()
{
	vec3 normal = normalize(vNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

	gl_FragData[0] = vec4(normal, 1.0);
}
