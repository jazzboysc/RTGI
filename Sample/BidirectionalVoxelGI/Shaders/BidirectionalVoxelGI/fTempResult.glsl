#version 410 core

in vec2 pTCoord;

uniform int ShowMode;
uniform int TextureArrayIndex;

uniform vec3 SceneBBMin;
uniform vec3 SceneBBExtension;

uniform sampler2D tempSampler;
uniform sampler2D tempSampler2;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if( ShowMode == 0 )
    {
        gl_FragData[0] = texture(tempSampler, pTCoord);
        
    }
    else if( ShowMode == 1 )
    {
        gl_FragData[0] = texture(tempSamplerArray, vec3(pTCoord, TextureArrayIndex));
    }
    else if( ShowMode == 2 )
    {
        vec4 result = texture(tempSampler, pTCoord) + texture(tempSampler2, pTCoord);

        // Simple gamma tone mapper.
        float greyRadiance = max(0.001, 0.3 * result.r + 0.6 * result.g + 0.1 * result.b);
        float mappedRadiance = pow(min(greyRadiance / 4.5, 1.0), 1.0 / 2.2);
        result.rgb = (mappedRadiance / greyRadiance) * result.rgb;
        result.w = 1.0;

        gl_FragData[0] = result;
    }
    else if( ShowMode == 3 )
    {
        vec3 worldPosition = texture(tempSampler, pTCoord).rgb;
        vec3 normalizedPosition = (worldPosition - SceneBBMin) / (2.0*SceneBBExtension);
        gl_FragData[0] = vec4(normalizedPosition, 1.0);
    }
}
