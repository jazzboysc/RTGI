#version 430 core

layout(local_size_x = 16) in;

layout (binding = 0, rgba32f) uniform image1D VPLSamplePattern;
layout (binding = 1, rgba32f) uniform image1D VPLSampleTest;

uniform sampler2DArray RSMPosition;
uniform sampler2DArray RSMNormal;
uniform sampler2DArray RSMFlux;

struct VPL
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec4 Flux;
};

layout (std430, binding = 0)  buffer _VPLBuffer
{
    VPL vpls[];
} VPLBuffer;

void main()
{
    vec4 sampleValue = imageLoad(VPLSamplePattern, int(gl_LocalInvocationID.x));

    int x = int(sampleValue.x * 512.0);
    int y = int(sampleValue.y * 512.0);
    int z = int(sampleValue.z * 5.0);
    imageStore(VPLSampleTest, int(gl_LocalInvocationID.x), vec4(x, y, z, 0.0));

    VPL tempVPL;
    tempVPL.WorldPosition = texture(RSMPosition, vec3(sampleValue.x, sampleValue.y, z));
    tempVPL.WorldNormal   = texture(RSMNormal, vec3(sampleValue.x, sampleValue.y, z));
    tempVPL.Flux          = texture(RSMFlux, vec3(sampleValue.x, sampleValue.y, z));
    VPLBuffer.vpls[gl_LocalInvocationID.x] = tempVPL;
}