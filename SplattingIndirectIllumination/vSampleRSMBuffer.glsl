#version 410

in vec4 vPosition;

uniform sampler2D RSMPositionSampler;
uniform sampler2D RSMNormalSampler;
uniform sampler2D RSMFluxSampler;
uniform sampler1D RSMSampleSampler;

void main()
{
    vec2 samplePosition = texelFetch(RSMSampleSampler, int(gl_InstanceID), 0).xy;
    gl_Position = vPosition*0.01;
    gl_Position.xy += samplePosition;
    gl_Position.w = 1.0;
}
