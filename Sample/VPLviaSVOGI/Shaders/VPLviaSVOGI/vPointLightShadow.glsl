#version 430 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 vNormalWorld;

uniform mat4 World;

void main()
{
	vNormalWorld = World * vec4(vNormal, 0.0);
    gl_Position = World * vPosition;
}
