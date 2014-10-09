#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

layout(binding = 0, rgba32f) uniform image3D voxelImage;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;

ivec3 GetImageCoords(vec3 worldPosition)
{
    vec3 imageDim = imageSize(voxelImage);
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) / (2 * SceneBBExtension);

    ivec3 res = ivec3(offsets * imageDim);
    return res;
}

void main()
{
    ivec3 coords = GetImageCoords(vPositionWorld.xyz);
    vec4 color = imageLoad(voxelImage, coords);

    vec3 normal = normalize(vNormalWorld).xyz;
    normal = normal*0.5 + 0.5;

    gl_FragData[0] = color;
}
