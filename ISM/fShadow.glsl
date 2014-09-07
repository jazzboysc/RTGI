#version 430 core

in vec4 teNormalWorld;

void main()
{
    vec3 normal = normalize(teNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

    gl_FragData[0] = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
	//gl_FragData[0] = vec4(normal, 1.0);
}
