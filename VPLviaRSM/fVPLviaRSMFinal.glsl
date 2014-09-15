#version 410

in vec2 pTCoord;

uniform sampler2D DirectLightingSampler;
uniform sampler2D IndirectLightingSampler;

void main()
{
    vec3 color1 = texture(DirectLightingSampler, pTCoord).rgb;
    vec3 color2 = texture(IndirectLightingSampler, pTCoord).rgb;
    gl_FragData[0].rgb = color1 + color2;
    gl_FragData[0].a = 1.0;
}
