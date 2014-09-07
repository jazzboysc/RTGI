#version 430 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 vNormalWorld;
out vec4 vPositionWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    vPositionWorld = World * vPosition;
	vNormalWorld = World * vec4(vNormal, 0.0);
    gl_Position = Proj * View * vPositionWorld;
}
