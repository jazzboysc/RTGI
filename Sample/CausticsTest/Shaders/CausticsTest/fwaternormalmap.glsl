#version 120

uniform sampler2D WaterNormalBumpMapTexture;

uniform float ODWNBMTR, WMSDWNBMTRM2;

void main()
{
	float y[5];

	y[0] = texture2D(WaterNormalBumpMapTexture, vec2(gl_TexCoord[0].s + ODWNBMTR, gl_TexCoord[0].t)).a;
	y[1] = texture2D(WaterNormalBumpMapTexture, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t + ODWNBMTR)).a;
	y[2] = texture2D(WaterNormalBumpMapTexture, vec2(gl_TexCoord[0].s - ODWNBMTR, gl_TexCoord[0].t)).a;
	y[3] = texture2D(WaterNormalBumpMapTexture, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t - ODWNBMTR)).a;

	y[4] = texture2D(WaterNormalBumpMapTexture, gl_TexCoord[0].st).a;

	vec3 Normal = normalize(vec3(y[2] - y[0], WMSDWNBMTRM2, y[1] - y[3]));

	gl_FragColor = vec4(Normal, y[4]);
}
