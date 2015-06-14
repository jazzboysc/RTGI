#version 430 core

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    vPositionWorld = World * vPosition;
    vNormalWorld = World * vec4(vNormal, 0.0);
    vec4 vPositionH = Proj * View * vPositionWorld;
    gl_Position = vPositionH;
}