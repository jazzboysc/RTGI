#version 120

uniform sampler2D WaterHeightMap;

uniform float DropRadius;
uniform vec2 Position;

void main()
{
	vec2 vh = texture2D(WaterHeightMap, gl_TexCoord[0].st).rg;

	float d = distance(gl_TexCoord[0].st, Position);

	gl_FragColor = vec4(vh.r, vh.g - 4.0f * max(DropRadius - d, 0.0), 0.0, 0.0);
}
