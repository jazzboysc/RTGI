#version 410 core

uniform mat4 View;
uniform mat4 Proj;

layout(triangles, equal_spacing, ccw) in;

in vec4 tcNormalWorld[];
out float teViewPosZ;

void main()
{	
	vec4 worldPos =	gl_in[0].gl_Position.xyzw * gl_TessCoord.x +
				    gl_in[1].gl_Position.xyzw * gl_TessCoord.y +
					gl_in[2].gl_Position.xyzw * gl_TessCoord.z;


    vec4 viewPos = View * worldPos;
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    halfDir.x = -halfDir.x / halfDir.z;
    halfDir.y = -halfDir.y / halfDir.z;
    halfDir.z = (len - 0.0) / 40.0;

    teViewPosZ = viewPos.z;
    gl_Position.xyz = halfDir;
    gl_Position.w = 1.0;
}
