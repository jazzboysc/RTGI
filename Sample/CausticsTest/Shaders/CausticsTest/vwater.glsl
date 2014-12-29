#version 120

uniform sampler2D WaterNormalBumpMapTexture;

uniform float WaterLevel, ODWMS;

varying vec3 Position;

void main()
{
	Position.xz = gl_Vertex.xy;

	gl_TexCoord[0].st = vec2(gl_Vertex.x, -gl_Vertex.y) * ODWMS + 0.5;

	Position.y = WaterLevel + texture2D(WaterNormalBumpMapTexture, gl_TexCoord[0].st).a;

	gl_TexCoord[1] = gl_ModelViewProjectionMatrix * vec4(Position, 1.0);

	gl_Position = gl_TexCoord[1];
}
