#version 410

in vec4 vPosition;
in vec3 vNormal;

out vec4 vPositionWorld;
out vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalWorld = World * vec4(vNormal, 0.0);
    vPositionWorld = World * vPosition;
	gl_Position = Proj * View * vPositionWorld;
}
