#version 430 core

in float teViewPosZ;
in float teDepth;

void main()
{
    if( teViewPosZ >= 0.0 )
    {
        discard;
    }
    
    gl_FragData[0] = vec4(teDepth, teDepth, teDepth, 1.0);
}
