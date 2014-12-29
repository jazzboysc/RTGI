#version 120

uniform int ClipType, Texturing, Caustic;
uniform float WaterLevel, ODWMS;
uniform sampler2D Texture, WaterNormalBumpMapTexture;

varying vec3 Position, Normal;

void main()
{
	if(ClipType == 1) if(Position.y < WaterLevel) discard;
	if(ClipType == 2) if(Position.y > WaterLevel) discard;

	vec3 LightDirection = gl_LightSource[0].position.xyz - Position;

	float LightDistance2 = dot(LightDirection, LightDirection);
	float LightDistance = sqrt(LightDistance2);

	LightDirection /= LightDistance;

	float NdotLD = max(0.0, dot(Normal, LightDirection));

	float Attenuation = gl_LightSource[0].constantAttenuation;
	Attenuation += gl_LightSource[0].linearAttenuation * LightDistance;
	Attenuation += gl_LightSource[0].quadraticAttenuation * LightDistance2;

	vec3 Light = (gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb * NdotLD) / Attenuation;

	if(Caustic == 1 && NdotLD > 0.0 && gl_LightSource[0].position.y != WaterLevel)
	{
		vec2 TexCoord = vec2(Position.x * ODWMS + 0.5, 0.5 - Position.z * ODWMS);
		float waterlevel = WaterLevel + texture2D(WaterNormalBumpMapTexture, TexCoord).a;

		if((gl_LightSource[0].position.y > WaterLevel && Position.y > waterlevel) || (gl_LightSource[0].position.y < WaterLevel && Position.y < waterlevel))
		{
			LightDirection.xz = gl_LightSource[0].position.xz - Position.xz;
			LightDirection.y = WaterLevel * 2.0 - gl_LightSource[0].position.y - Position.y;

			LightDistance2 = dot(LightDirection, LightDirection);
			LightDistance = sqrt(LightDistance2);

			LightDirection /= LightDistance;

			NdotLD = max(0.0, dot(Normal, LightDirection));

			Attenuation = gl_LightSource[0].constantAttenuation;
			Attenuation += gl_LightSource[0].linearAttenuation * LightDistance;
			Attenuation += gl_LightSource[0].quadraticAttenuation * LightDistance2;
		}

		float Distance = (WaterLevel - Position.y) / LightDirection.y;

		TexCoord = LightDirection.xz * Distance + Position.xz;

		TexCoord.s = TexCoord.s * ODWMS + 0.5;
		TexCoord.t = 0.5 - TexCoord.t * ODWMS;

		vec3 WaterNormal = texture2D(WaterNormalBumpMapTexture, TexCoord).rgb;

		float WNdotLD = abs(dot(WaterNormal, LightDirection));
		float Caustic = NdotLD * pow(WNdotLD, 4.0) / Attenuation;
		Light += gl_LightSource[0].specular.rgb * Caustic;
	}

	gl_FragColor.rgb = gl_Color.rgb;
	if(Texturing == 1) gl_FragColor.rgb *= texture2D(Texture, gl_TexCoord[0].st).rgb;
	gl_FragColor.rgb *= Light;
}
