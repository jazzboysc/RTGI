#version 430 core

layout(local_size_x = 16) in;

layout (binding = 0, rgba32f) uniform image1D VPLSamplePattern;
layout (binding = 1, rgba32f) uniform image1D VPLSampleTest;

void main()
{
    vec4 sampleValue = imageLoad(VPLSamplePattern, int(gl_LocalInvocationID.x));
    imageStore(VPLSampleTest, int(gl_LocalInvocationID.x), sampleValue);
}