#version 430 core

layout(triangles, equal_spacing, ccw) in;

in vec4 tcNormalWorld[];

void main()
{	
	vec4 worldPos =	gl_in[0].gl_Position.xyzw * gl_TessCoord.x +
				    gl_in[1].gl_Position.xyzw * gl_TessCoord.y +
					gl_in[2].gl_Position.xyzw * gl_TessCoord.z;

    gl_Position = worldPos;
}
