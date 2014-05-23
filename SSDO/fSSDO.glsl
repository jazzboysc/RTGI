#version 120

const float PI = 3.141592;

varying vec2 pTCoord;

uniform mat4 Proj;

// The radius in world units the samples that come from a unit sphere are scaled to.
uniform float sampleRadius;

// Strength of the occlusion effect itself.
uniform float strength;

// Singularity for distance.
uniform float singularity;

uniform float depthBias;

// Strength of the indirect bounce.
uniform float bounceStrength;

uniform float bounceSingularity;

// The number of samples for one pixel.
uniform int sampleCount;

// The size in number of pixels of the quasi-random pattern used.
uniform int patternSize;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D colorSampler;
uniform sampler2D directLightingSampler;
uniform sampler2D randomSampler;
uniform sampler2D envMapSampler;

// Given the normal of a pixel, coumpute tangent space coordinate frame.
mat3 GetTBNFrame(const vec3 n)
{
	mat3 localFrame;
	vec3 u, v;
	if( n.x > n.y )
	{
		u = vec3(n.z, 0.0, -n.x);
	}
	else
	{
		u = vec3(0.0, n.z, -n.y);
	}
	u = normalize(u);
	v = cross(u, n);
	localFrame = mat3(u, v, n);

	return localFrame;
}

void main()
{
	vec3 normalView = texture2D(normalSampler, pTCoord).rgb;
	normalView = normalView*2.0 - 1.0;
	vec4 positionView = texture2D(positionSampler, pTCoord);
	vec4 pixelColor = texture2D(colorSampler, pTCoord);
    vec4 outColor = vec4(0.0);

	// Skip undefined pixels.
	if( pixelColor.a > 0.0 )
	{
		// Accumulation over radiance.
		vec3 directRadianceSum = vec3(0.0);
		vec3 occluderRadianceSum = vec3(0.0);
		vec3 ambientRadianceSum = vec3(0.0);
		float ambientOcclusion = 0.0;
		
		// Get TBN space frame along this normal.
		mat3 localMatrix = GetTBNFrame(normalView);
		
		// The index of the current pattern.
		// We use one out of patternSize * patternSize pre-defined hemisphere patterns.
		// The i'th pixel in every sub-rectangle uses always the same i-th sub-pattern.
		//int patternIndex = int(gl_FragCoord.x) % patternSize + (int(gl_FragCoord.y) % patternSize) * patternSize;
        float mod1 = mod(gl_FragCoord.x, float(patternSize));
        float mod2 = mod(gl_FragCoord.y, float(patternSize));
        int patternIndex = int(mod1) + int(mod2)*patternSize;
					
		// Loop over all samples from the current pattern.
		float invSampleCount = 1.0 / sampleCount;
		float invPatternSizeSqr = 1.0 / (patternSize*patternSize);
		for( int i = 0; i < sampleCount ; i++ )
		{
			// Get the i-th sample direction and tranfrom it to local space.
			float s = i * invSampleCount;
			float t = patternIndex * invPatternSizeSqr;
			vec3 sample = localMatrix * texture2D(randomSampler, vec2(s, t)).rgb;
			
			vec3 normalizedSample = normalize(sample);
			
			// Go sample-radius steps along the sample direction, starting at the current pixel view space location.
			vec4 viewSampleOccluderPosition = positionView + sampleRadius * vec4(sample.x, sample.y, sample.z, 0);
			
			// Project this view occluder position in the current eye space using the projection matrix.
			vec4 occluderSamplePosition = Proj * viewSampleOccluderPosition;
			
			// Do a division by w here and map to window coords usign usual GL rules.
			vec2 occlusionTexCoord = (occluderSamplePosition.xy / occluderSamplePosition.w)*0.5 + 0.5;
						
			// Read the occluder position and the occluder normal at the occluder texcoord.
			vec4 viewOccluderPosition = texture2D(positionSampler, occlusionTexCoord);
			vec3 viewOccluderNormal = texture2D(normalSampler, occlusionTexCoord).rgb;
			
			// Remove influence of undefined pixels.
			if( length(viewOccluderNormal) == 0 )
			{
				viewOccluderPosition = vec4(100000.0, 100000.0, 100000.0, 1.0);
			}
			
			// Compute blocking from this occluder.
			float depth = viewSampleOccluderPosition.z;
			float sampleDepth = viewOccluderPosition.z + depthBias;
					
			// First compute a delta towards the blocker, its length and its normalized version.
			float distanceTerm = abs(depth - sampleDepth) < singularity ? 1.0 : 0.0;
			
			float visibility = 1.0 - strength * (sampleDepth > depth ? 1.0 : 0.0) * distanceTerm;
									
			// Geometric term of the current pixel towards the current sample direction.
			float receiverGeometricTerm = max(0.0, dot(normalizedSample, normalView));

			// Get the irradiance in the current direction.

			float theta = acos(normalizedSample.y);              
			float phi = atan(normalizedSample.z, normalizedSample.x);
			if( phi < 0.0 )
			{
				phi += 2.0 * PI;
			}
			if( phi > 2.0 * PI )
			{
				phi -= 2.0 * PI;
			}
						
			vec3 senderRadiance = texture2D(envMapSampler, vec2(phi / (2.0*PI), 1.0 - theta / PI)).rgb;

			// Compute radiance as the usual triple product of visibility, irradiance and BRDF.
			// Note, that we are not limited to diffuse illumination.
			// For practical reasons, we post-multiply with the diffuse color.

			vec3 radiance = visibility * receiverGeometricTerm * senderRadiance;
			
			// Accumulate the radiance from this sample
			directRadianceSum += radiance;
			
			vec3 ambientRadiance = senderRadiance * receiverGeometricTerm;
			ambientRadianceSum += ambientRadiance;
			ambientOcclusion += visibility;

			// Compute indirect bounce radiance.
			// First read sender radiance from occluder.
			vec3 directRadiance = texture2D(directLightingSampler, occlusionTexCoord).rgb;
			
			// Compute the bounce geometric term towards the blocker.
			vec3 delta = positionView.xyz - viewOccluderPosition.xyz;
			vec3 normalizedDelta = normalize(delta);
			float unclampedBounceGeometricTerm = 
				max(0.0, dot(normalizedDelta, -normalView)) * 
				max(0.0, dot(normalizedDelta, viewOccluderNormal)) /
				max(dot(delta, delta), bounceSingularity);
			float bounceGeometricTerm = min(unclampedBounceGeometricTerm, bounceSingularity);
			
			// The radiance due to bounce.
			vec3 bounceRadiance = bounceStrength * directRadiance * bounceGeometricTerm;
			
			// Compute radiance from this occcluder (mixing bounce and scatter)
			//vec3 occluderRadiance = bounceRadiance * receiverGeometricTerm;
			vec3 occluderRadiance = bounceRadiance;
			
			// Finally, add the indirect light to the light sum.
			occluderRadianceSum += occluderRadiance;			
		}
		
		// Clamp to zero.
		// Althought there should be nothing negative here it is suitable to allow single samples do DARKEN with their contribution.
		// This can be used to exaggerate the directional effect and gives nicely colored shadows (instead of AO).
		directRadianceSum = max(vec3(0), directRadianceSum);
		occluderRadianceSum = max(vec3(0), occluderRadianceSum);

		// Add direct and indirect radiance.
		vec3 radianceSum = directRadianceSum + occluderRadianceSum;
		
		// Multiply by solid angle for one sample.
		radianceSum *= 2.0 * PI / sampleCount;
				
		// Store final radiance value in the framebuffer.
		outColor.rgb = radianceSum;
		outColor.a = 1.0;

	}
    else
	{
		// In case we came across an invalid deferred pixel.
		outColor = vec4(0.0);
	}
    
    gl_FragData[0] = outColor;
}
