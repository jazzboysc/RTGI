#version 120

uniform sampler2D WaterNormalBumpMapTexture, ReflectionTexture, RefractionTexture;

uniform float WaterLevel;
uniform vec3 CameraPosition;
uniform mat3x3 NormalMatrix;

varying vec3 Position;

void main()
{
	vec2 TexCoord = gl_TexCoord[1].st / gl_TexCoord[1].w * 0.5 + 0.5;

	vec3 Normal = NormalMatrix * normalize(texture2D(WaterNormalBumpMapTexture, gl_TexCoord[0].st).rgb);

	vec2 Offset = Normal.xz * vec2(0.05, -0.05);

	if(CameraPosition.y > WaterLevel)
	{
		vec3 Reflection = texture2D(ReflectionTexture, TexCoord + Offset).rgb;

		vec3 Refraction;

		Refraction.r = texture2D(RefractionTexture, TexCoord - Offset * 0.8).r;
		Refraction.g = texture2D(RefractionTexture, TexCoord - Offset * 0.9).g;
		Refraction.b = texture2D(RefractionTexture, TexCoord - Offset * 1.0).b;

		vec3 LightDirection = normalize(Position - gl_LightSource[0].position.xyz);
		vec3 LightDirectionReflected = reflect(LightDirection, Normal);
		vec3 CameraDirection = normalize(CameraPosition - Position);
		float CDdotLDR = max(dot(CameraDirection, LightDirectionReflected), 0.0);
		vec3 Specular = gl_LightSource[0].specular.rgb * pow(CDdotLDR, 128.0);

		float NdotCD = max(dot(Normal, CameraDirection), 0.0);

		gl_FragColor.rgb = mix(Reflection, Refraction, NdotCD) + Specular;
	}
	else
	{
		gl_FragColor.r = texture2D(RefractionTexture, TexCoord - Offset * 0.8).r;
		gl_FragColor.g = texture2D(RefractionTexture, TexCoord - Offset * 0.9).g;
		gl_FragColor.b = texture2D(RefractionTexture, TexCoord - Offset * 1.0).b;
	}
}
