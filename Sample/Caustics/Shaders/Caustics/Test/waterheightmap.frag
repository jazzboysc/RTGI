#version 120

uniform sampler2D WaterHeightMap;

uniform float ODWHMR;

void main()
{
	vec2 vh = texture2D(WaterHeightMap, gl_TexCoord[0].st).rg;

	float force = 0.0;

	force += 0.707107 * (texture2D(WaterHeightMap, gl_TexCoord[0].st - vec2(ODWHMR, ODWHMR)).g - vh.g);
	force += texture2D(WaterHeightMap, gl_TexCoord[0].st - vec2(0.0, ODWHMR)).g - vh.g;
	force += 0.707107 * (texture2D(WaterHeightMap, gl_TexCoord[0].st + vec2(ODWHMR, -ODWHMR)).g - vh.g);

	force += texture2D(WaterHeightMap, gl_TexCoord[0].st - vec2(ODWHMR, 0.0)).g - vh.g;
	force += texture2D(WaterHeightMap, gl_TexCoord[0].st + vec2(ODWHMR, 0.0)).g - vh.g;

	force += 0.707107 * (texture2D(WaterHeightMap, gl_TexCoord[0].st + vec2(-ODWHMR, ODWHMR)).g - vh.g);
	force += texture2D(WaterHeightMap, gl_TexCoord[0].st + vec2(0.0, ODWHMR)).g - vh.g;
	force += 0.707107 * (texture2D(WaterHeightMap, gl_TexCoord[0].st + vec2(ODWHMR, ODWHMR)).g - vh.g);

	force *= 0.125;

	vh.r += force;
	vh.g += vh.r;
	vh.g *= 0.99;

	gl_FragColor = vec4(vh, 0.0, 0.0);
}
