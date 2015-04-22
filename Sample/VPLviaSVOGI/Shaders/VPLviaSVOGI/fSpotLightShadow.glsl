#version 430 core

in float gViewPosZ;

out vec4 Output;

void main()
{
    Output = vec4(gViewPosZ, gViewPosZ, gViewPosZ, 1.0);
}
