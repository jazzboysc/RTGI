#version 430 core

in float gViewPosZ;
in float gDepth;

void main()
{
    if( gViewPosZ >= 0.0 )
    {
        discard;
    }
    
    gl_FragData[0] = vec4(gDepth, gDepth, gDepth, 1.0);
}
