#version 410 core

layout(vertices = 3) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 tcPositionWorld[];
out vec4 tcNormalWorld[];

void main(void)
{
	gl_TessLevelOuter[0] = 4.0;
	gl_TessLevelOuter[1] = 4.0;
	gl_TessLevelOuter[2] = 4.0;

	gl_TessLevelInner[0] = 8.0;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcPositionWorld[gl_InvocationID] = vPositionWorld[gl_InvocationID];
    tcNormalWorld[gl_InvocationID] = vNormalWorld[gl_InvocationID];
}
