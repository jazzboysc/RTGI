#version 120

varying vec2 pTCoord;

uniform int showMode;
uniform vec2 nearFarPlane;
uniform sampler2D tempSampler;

float GetLinearDepth(vec2 uv)
{
    float n = nearFarPlane.x;
    float f = nearFarPlane.y;
    float z = texture2D(tempSampler, uv).x;
    
    return (2.0 * n) / (f + n - z * (f - n));
}

void main()
{
    if( showMode == 4 )
    {
        float depth = GetLinearDepth(pTCoord);        
        gl_FragData[0] = vec4(depth, depth, depth, 1.0);
    }
    else
    {
        gl_FragData[0] = texture2D(tempSampler, pTCoord);
    }
}
