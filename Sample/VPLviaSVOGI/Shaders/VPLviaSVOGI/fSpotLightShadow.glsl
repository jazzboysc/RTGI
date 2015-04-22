#version 430 core

in float gViewPosZ;

out float Output;

void main()
{
    Output = gViewPosZ;
}
