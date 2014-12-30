#version 120

uniform samplerCube PoolSkyCubeMap;

void main()
{
	gl_FragColor = textureCube(PoolSkyCubeMap, gl_TexCoord[0].stp);
}
