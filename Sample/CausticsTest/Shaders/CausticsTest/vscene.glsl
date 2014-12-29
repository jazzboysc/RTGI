#version 120

uniform mat3x3 NormalMatrix;
uniform mat4x4 ModelMatrix;

varying vec3 Position, Normal;

void main()
{
	Position = (ModelMatrix * gl_Vertex).xyz;
	Normal = NormalMatrix * gl_Normal;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(Position, 1.0);
}
