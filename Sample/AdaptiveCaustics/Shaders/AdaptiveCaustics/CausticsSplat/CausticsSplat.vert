// This shader computes a photon's 2D (u,v) position in the caustic map
//    based upon an input screen position (in [0..1] based upon the
//    2D (u,v) position in the shadow map/light view frustum).
//
// This is used by the associated shader "addaptiveCaustics_maxTraversal_SplatIntoCMap.frag.glsl"
//    during splatting of photons.  It is also used during the 2nd traversal pass in
//    the maximum error metric traversal prior to detection of converged photons.
//
// Basically, this shader is the equivalent of the deferred refraction
//    shader used to render refractive objects seen from the eye.  The 
//    main difference is that this is a vertex shader, not a fragment
//    shader, so input are screen-space photon locations as vertices
//    rather than screen-space fragment positions.

#version 430 core

in vec4 vPosition;
in vec2 vTCoord;

out vec4 Temp[6];

uniform float TanLightFovy2;
uniform vec4 NearFarInfo, RefractionIndexInfo;

uniform sampler2DArray RefractorNorm;
uniform sampler2DArray RefractorDepth;

uniform sampler2D ReceiverDepth;    

// This takes an eye-space (actually light-space since
//    our "eye" is at the light here) position and converts
//    it into a image-space (u,v) position.  This simply
//    applies the GL projection matrix and homogeneous
//    divide.
vec2 ProjectToTexCoord( vec4 eyeSpacePos )
{
	vec4 projLoc = gl_ProjectionMatrix * eyeSpacePos;
	return ( 0.5*(projLoc.xy / projLoc.w) + 0.5 );
}

// A simple refraction function (similar to the GLSL refraction, except
//    is is correct, not some pseudo refraction!)
vec4 refraction( vec3 incident, vec3 normal, float ni_nt, float ni_nt_sqr )
{
    vec4 returnVal;
    float tmp = 1.0;
	float IdotN = dot( -incident, normal );
	float cosSqr = 1.0 - ni_nt_sqr*(1.0 - IdotN*IdotN);
	return ( cosSqr <= 0.0 ? 
			 vec4( normalize(reflect( incident, normal )), -1.0 ) :
			 vec4( normalize( ni_nt * incident + (ni_nt* IdotN - sqrt( cosSqr )) * normal ), 1.0 ) );
}

// Takes a screen coordinate and turns it into a 3D vector pointing
//   in the correct direction (in eye-space)
vec3 DirectionFromScreenCoord( vec2 texPos )
{
	vec3 dir = vec3(TanLightFovy2 * 2.0 * texPos - TanLightFovy2, -1.0 )
	return normalize( dir );
}

// Code to compute photon position per vertex
void main( void )
{
	float outside = 0.0, noBackNorm=0.0;
	vec2 Dist;

	// Get the front facing surface normal based upon the screen-space
	//    vertex position.	
	vec4 tmp = texture( RefractorNorm, vec3(vTCoord, 0) );

	// Get front facing refractor position
	vec4 P_1 = vec4( tmp.w*DirectionFromScreenCoord( vTCoord ), 1.0 );
	
	// Check if this pixel has refractive materials or not.
	outside = dot(tmp.xyz,tmp.xyz) < 0.5 ? 1.0 : 0.0;
	
	// Compute normalized V and N_1 values.
	vec3 N_1 = normalize( tmp.xyz );   // Surface Normal
	vec3 V   = normalize( P_1.xyz  );   // View direction

	// Find the distance to front & back surface, first as normalized [0..1] values, than unprojected
	Dist.x = (texture( RefractorDepth, vec3(vTCoord, 1) ).z) * 2.0f - 1.0f;	
	Dist.y = (texture( RefractorDepth, vec3(vTCoord, 0) ).z) * 2.0f - 1.0f;	
	// TIMES2??????
	Dist = NearFarInfo.x / (Dist * NearFarInfo.y - NearFarInfo.z );

	// Distance between front & back surfaces
	// MINUS SIGN????
	float d_V = Dist.y - Dist.x;
	
	// find the refraction direction
	vec3 T_1 = refraction( V, N_1, RefractionIndexInfo.x, RefractionIndexInfo.y ).xyz;

	// Right now, we're using a hacked hack to avoid requiring d_N
	float d_tilde = d_V;   

	// Compute approximate exitant location & surface normal
	vec4 P_2_tilde = vec4( T_1 * d_tilde + P_1.xyz, 1.0);
	vec3 N_2 = texture( RefractorNorm, vec3( ProjectToTexCoord( P_2_tilde ), 1.0) ).xyz;
	float dotN2 = dot( N_2.xyz, N_2.xyz );

	// What happens if we lie in a black-texel?  Means no normal!  Conceptually,
	//   this means we pass thru "side" of object.  Use norm perpindicular to view
	if ( dotN2 == 0.0 )
		N_2 = normalize(vec3( T_1.x, T_1.y, 0.0 ) );

	// Refract at the second surface
	vec4 T_2 = refraction( T_1, -N_2, RefractionIndexInfo.z, RefractionIndexInfo.w );
	bool invalidPhoton = T_2.w < 0.0 || outside > 0.5;
	T_2.w = 0.0;

	// Scale the vector so that it's got a unit-length z-component
	vec4 tmpT2 = T_2 / -T_2.z;

	// Compute the texture locations of ctrPlusT2 and refractToNear.
	float index, minDist = 1000.0, deltaDist = 1000.0;
	for (index = 0.0; index < 2.0; index += 1.0)
	{
		float texel = texture2D( ReceiverDepth, ProjectToTexCoord( P_2_tilde + tmpT2 * index ) ).x;
		float distA = -(NearFarInfo.x / (texel * NearFarInfo.y - NearFarInfo.w)) + P_2_tilde.z;
		if ( abs(distA-index) < deltaDist )
		{
			deltaDist = abs(distA-index);
			minDist = index;
		}
	}
		
	// Do our final iteration to home in on the final photon position.
	//   10 iterations is usually enough.
	for (float index = 0.0; index < 10.0; index += 1.0)
	{
		float texel1 = texture( ReceiverDepth, ProjectToTexCoord( P_2_tilde + minDist * tmpT2 ) ).x;
		minDist = -(NearFarInfo.x / (texel1 * NearFarInfo.y - NearFarInfo.w)) + P_2_tilde.z;
	}	
	
	// OK, find the projected photon position in the caustic map.
	vec4 photonPosition  = P_2_tilde + minDist * tmpT2;
	vec4 projectedPhoton = gl_ProjectionMatrix * vec4( photonPosition.xyz, 1.0 );
	projectedPhoton /= projectedPhoton.w;
	
	// Pass down this data (either to the splatting code or to the
	//    code that checks adjacent photons for convergence)
	gl_Position = projectedPhoton;
	Temp[1] = photonPosition; 
	Temp[0] = invalidPhoton ? vec4(1.0) : vec4(0.0); 
	Temp[2] = projectedPhoton;
	Temp[3] = texture( spotLight, gl_Position.xy );
	Temp[3].w = floor(512.0*gl_Position.y)+gl_Position.x;
	
	gl_PointSize = invalidPhoton ? 0.0 : 3.0;
	Temp[4].z = 3.0;
	Temp[4].xy = projectedPhoton.xy * 0.5 + 0.5; 
	Temp[4].w = 4.0;
	Temp[5] = vec4(0.0);
}














