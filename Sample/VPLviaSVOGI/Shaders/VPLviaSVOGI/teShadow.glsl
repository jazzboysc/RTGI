#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

layout(triangles, equal_spacing, ccw) in;

in vec4 tcNormalWorld[];
out float teViewPosZ;
out float teDepth;

void main()
{	
    // Fetch light info.
    uint curLightIndex = sceneLightUniformBuffer.info.CurLightIndex;
    SceneLight curLight = sceneLightUniformBuffer.lights[curLightIndex];

	vec4 worldPos =	gl_in[0].gl_Position.xyzw * gl_TessCoord.x +
				    gl_in[1].gl_Position.xyzw * gl_TessCoord.y +
					gl_in[2].gl_Position.xyzw * gl_TessCoord.z;

    vec4 viewPos = curLight.LightProjectorView * worldPos;
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    halfDir.x = -halfDir.x / halfDir.z;
    halfDir.y = -halfDir.y / halfDir.z;
    halfDir.z = (len - curLight.LightProjectorNearFar.x) /
        (curLight.LightProjectorNearFar.y - curLight.LightProjectorNearFar.x);

    teViewPosZ = viewPos.z;
    teDepth = halfDir.z;
    gl_Position.xyz = halfDir;
    gl_Position.w = 1.0;
}
