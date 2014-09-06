#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec4 tcPositionWorld[];
in vec4 tcNormalWorld[];

out vec4 tePositionWorld;
out vec4 teNormalWorld;

void main()
{	
	gl_Position.xyzw =	gl_in[0].gl_Position.xyzw * gl_TessCoord.x +
						gl_in[1].gl_Position.xyzw * gl_TessCoord.y +
						gl_in[2].gl_Position.xyzw * gl_TessCoord.z;

    tePositionWorld.xyzw = tcPositionWorld[0].xyzw * gl_TessCoord.x +
        tcPositionWorld[1].xyzw * gl_TessCoord.y +
        tcPositionWorld[2].xyzw * gl_TessCoord.z;

    teNormalWorld.xyzw = tcNormalWorld[0].xyzw * gl_TessCoord.x +
        tcNormalWorld[1].xyzw * gl_TessCoord.y +
        tcNormalWorld[2].xyzw * gl_TessCoord.z;
}
