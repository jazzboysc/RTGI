#version 430 core

in float teViewPosZ;

void main()
{
    if( teViewPosZ >= 0.0 )
    {
        discard;
    }
    
    gl_FragData[0] = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
