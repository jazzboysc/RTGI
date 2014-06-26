#version 120

varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform vec3 materialColor;

uniform mat4 lightProjectorView;
uniform mat4 lightProjectorProj;

uniform float sampleRadius;
uniform int sampleCount;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D fluxSampler;
uniform sampler2D samplingPatternSampler;

void main()
{
	// Get light view space and homogeneous space position.
	vec4 positionView = lightProjectorView * vPositionWorld;
	vec4 positionH = lightProjectorProj * positionView;
	positionH = positionH / positionH.w;

	// Get light view space normal.
	vec4 normalWorld = normalize(vNormalWorld);
	vec4 normalView = lightProjectorView * normalWorld;

	// Get uv coordinates.
	vec2 uv = positionH.xy*0.5 + 0.5;

	float pixelWieght = 0.0;
	float pixelWeightSum = 0.0;
	vec3 outColor = vec3(0.0);

	for( int i = 0; i < sampleCount ; i++ )
	{
		float s = float(i) / float(sampleCount);
		vec3 randomNumbers = texture2D(samplingPatternSampler, vec2(s, 0.0)).rgb;
		uv += randomNumbers.xy * sampleRadius;
		pixelWieght = randomNumbers.z;

		vec3 pixelLightPositionView = texture2D(positionSampler, uv).rgb;
		vec3 pixelLightNormalView = texture2D(normalSampler, uv).rgb;
		vec3 pixelLightFlux = texture2D(fluxSampler, uv).rgb;

		vec3 incidentDir = pixelLightPositionView - positionView.xyz;
		float cosi = max(0.0, dot(normalView.xyz, incidentDir));
		float coso = max(0.0, dot(pixelLightNormalView, -incidentDir));
		float distance = length(incidentDir);

		outColor = outColor + pixelWieght*pixelLightFlux*cosi*coso/(distance*distance);
		pixelWeightSum = pixelWeightSum + pixelWieght;
	}
	outColor = outColor / pixelWeightSum;

	gl_FragData[0] = vec4(outColor, 1.0);
}
