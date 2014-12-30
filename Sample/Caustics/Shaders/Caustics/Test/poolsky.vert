#version 120

void main()
{
	gl_TexCoord[0].stp = vec3(gl_Vertex.x, -gl_Vertex.yz);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
