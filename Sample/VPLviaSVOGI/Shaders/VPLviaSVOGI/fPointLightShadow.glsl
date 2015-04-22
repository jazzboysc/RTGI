#version 430 core

in float gViewPosZ;
in float gDepth;

out float Output;

void main()
{
    if( gViewPosZ >= 0.0 )
    {
        discard;
    }
    
    Output = gDepth;
}
