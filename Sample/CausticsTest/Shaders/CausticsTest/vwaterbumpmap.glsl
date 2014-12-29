#version 120

uniform float WMS, WMSD2;

varying vec2 Position;

void main()
{
	Position = vec2(gl_Vertex.x * WMS - WMSD2, WMSD2 - gl_Vertex.y * WMS);
	gl_Position = gl_Vertex * 2.0 - 1.0;
}
