#version 430 core

#include "VPLviaSVOGI/sVirtualPointLight.glsl"

layout(local_size_x = 1) in;

layout (binding = 0, rgba32f) uniform image1D VPLSamplePattern;
layout (binding = 1, rgba32f) uniform image1D VPLSampleTest;

uniform sampler2DArray RSMPosition;
uniform sampler2DArray RSMNormal;
uniform sampler2DArray RSMFlux;

void main()
{
    vec4 sampleValue = imageLoad(VPLSamplePattern, int(gl_GlobalInvocationID.x));

    ivec3 rsmSize = textureSize(RSMPosition, 0);
    int x = int(sampleValue.x * rsmSize.x);
    int y = int(sampleValue.y * rsmSize.y);
    int z = int(sampleValue.z * rsmSize.z);
    imageStore(VPLSampleTest, int(gl_GlobalInvocationID.x), vec4(x, y, z, 0.0));

    VPL tempVPL;
    tempVPL.WorldPosition = texture(RSMPosition, vec3(sampleValue.x, sampleValue.y, z));
    tempVPL.WorldNormal   = texture(RSMNormal, vec3(sampleValue.x, sampleValue.y, z));
    tempVPL.Flux          = texture(RSMFlux, vec3(sampleValue.x, sampleValue.y, z));

    vec3 normal = tempVPL.WorldNormal.xyz*2.0 - 1.0;
    tempVPL.WorldPosition.xyz = tempVPL.WorldPosition.xyz + normal*0.000001;  // Avoid self-intersection.

    vec3 E = tempVPL.WorldPosition.xyz;
    vec3 D = -normal;
    tempVPL.View = GetVPLViewTransform(E, D);

    VPLBuffer.vpls[gl_GlobalInvocationID.x] = tempVPL;
}