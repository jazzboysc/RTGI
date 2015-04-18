#version 430 core

in float teViewPosZ;
in float teDepth;

out vec4 Output;

void main()
{
    if( teViewPosZ >= 0.0 )
    {
        discard;
    }
    
    Output = vec4(teDepth, teDepth, teDepth, 1.0);
    //Output = vec4(1.0, 0, 0, 1.0);
}
