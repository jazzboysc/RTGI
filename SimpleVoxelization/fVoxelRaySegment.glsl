#version 430 core

uniform vec3 MaterialColor;

void main()
{
    gl_FragData[0] = vec4(MaterialColor, 1.0);
}