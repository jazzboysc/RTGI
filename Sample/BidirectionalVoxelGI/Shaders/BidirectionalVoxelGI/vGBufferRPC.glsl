#version 430 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 vNormalWorld;
out vec4 vPositionWorld;
out vec4 vPositionNDC;
out vec4 vPositionNDCCache;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;
uniform mat4 WorldCache;
uniform mat4 ViewCache;
uniform mat4 ProjCache;

void main()
{
    vPositionWorld = World * vPosition;
    vPositionNDC = Proj * View * vPositionWorld;
    vPositionNDCCache = ProjCache * ViewCache * WorldCache * vPosition;
    vNormalWorld = World * vec4(vNormal, 0.0);
    gl_Position = vPositionNDC;
}
