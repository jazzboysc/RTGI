#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

layout(binding = 0, rgba32f) uniform image3D voxelImage;

void main()
{
	vec3 normal = normalize(gNormalWorld).xyz;
	normal = normal*0.5 + 0.5;
}
