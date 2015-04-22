#version 430 core

in float gViewPosZ;
in float gDepth;

out vec4 Output;

void main()
{
    if( gViewPosZ >= 0.0 )
    {
        discard;
    }
    
    Output = vec4(gDepth, gDepth, gDepth, 1.0);
}
