#version 430 core

in vec4 vPosition;
in vec2 vTCoord;

out vec4 Temp[6];

uniform float TanLightFovy2;
uniform vec4 NearFarInfo;
uniform vec4 RefractionIndexInfo;
uniform mat4 LightProj;

uniform sampler2DArray RefractorNorm;
uniform sampler2DArray RefractorDepth;
uniform sampler2D ReceiverDepth;

// An image to store data into.
//layout(binding = 0, offset = 0) uniform atomic_uint writeCount;
//layout(binding = 1, offset = 4) uniform atomic_uint storageCounter2;

//layout (rgba32f, binding = 0) uniform image2D data;
//layout (binding = 1) uniform sampler2DArray refractorNormal;

layout(std430, binding = 0) buffer _ACMSharedCommandBuffer
{
	uint mipmapLevel; // 3
	uint readOffset; // 0
	uint readCount; // 64 * 64 = 4096
	uint writeOffset; // 4096
	uint width;
	uint height;
	float deltaX;
	float deltaY;
} ACMSharedCommandBuffer;

layout(std430, binding = 1)  buffer _ACMBuffer
{
	vec4 debug;

    // photon uv buffer. Must be big enough.
    vec4 uv[];
} ACMBuffer;

// This takes an eye-space (actually light-space since
//    our "eye" is at the light here) position and converts
//    it into a image-space (u,v) position.  This simply
//    applies the GL projection matrix and homogeneous
//    divide.
vec2 ProjectToTexCoord( vec4 eyeSpacePos )
{
	vec4 projLoc = LightProj * eyeSpacePos;
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
	vec3 dir = vec3(TanLightFovy2 * 2.0 * texPos - TanLightFovy2, -1.0 );
	return normalize( dir );
}

void main(void)
{
	vec2 vTCoord = ACMBuffer.uv[ACMSharedCommandBuffer.readOffset + gl_VertexID].xy;

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
	vec4 projectedPhoton = LightProj * vec4( photonPosition.xyz, 1.0 );
	projectedPhoton /= projectedPhoton.w;
	
	// Pass down this data (either to the splatting code or to the
	//    code that checks adjacent photons for convergence)
	gl_Position = projectedPhoton;
	Temp[1] = photonPosition; 
	Temp[0] = invalidPhoton ? vec4(1.0) : vec4(0.0); 
	Temp[2] = projectedPhoton;
	Temp[3] = vec4(1.0, 1.0, 1.0, 1.0);//texture( spotLight, gl_Position.xy );
	//Temp[3].w = floor(512.0*gl_Position.y)+gl_Position.x;
	
	gl_PointSize = invalidPhoton ? 0.0 : 3.0;
	Temp[4].z = 3.0;
	Temp[4].xy = projectedPhoton.xy * 0.5 + 0.5; 
	Temp[4].w = 4.0;
	Temp[5] = vec4(0.0);
}

/*
void main(void)
{
	gl_PointSize = 3.0;

	vec2 uv = ACMBuffer.uv[ACMSharedCommandBuffer.readOffset + gl_VertexID].xy;


	float x = gl_VertexID % 768;
	x = (x / 768 * 2) - 1;
	float y = gl_VertexID / 768 * 5;
	y = (y / 768 * 2) - 1;
	gl_Position = vec4(x, y, 0.0, 1.0);
	gl_Position = vec4(uv, 0.0, 1.0);
	gl_Position = gl_Position * 2 - 1;
	float debug = 0;
}
*/