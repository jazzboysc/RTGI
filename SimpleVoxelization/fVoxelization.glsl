#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

layout(binding = 0, rgba32f) uniform image3D voxelImage;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform vec3 MaterialColor;

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
	vec3 normal = normalize(gNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

    ivec3 coords = GetImageCoords(gPositionWorld.xyz);
    imageStore(voxelImage, coords, vec4(MaterialColor, 0.0));
}
