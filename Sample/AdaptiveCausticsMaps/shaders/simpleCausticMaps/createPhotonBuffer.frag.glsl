

uniform vec4 local1, local2;
 
uniform sampler2D otherObjsEyeDepth; 
uniform sampler2D prevDistances;     
uniform sampler2D backFace;	         


inline vec2 ProjectToTexCoord( vec4 eyeSpacePos )
{
	vec4 projLoc = gl_ProjectionMatrix * eyeSpacePos;
	return ( 0.5*(projLoc.xy / projLoc.w) + 0.5 );
}

vec4 refraction( vec3 incident, vec3 normal, float ni_nt, float ni_nt_sqr )
{
    vec4 returnVal;
	float IdotN = dot( -incident, normal );
	float cosSqr = 1.0 - ni_nt_sqr*(1.0 - IdotN*IdotN);
	return ( cosSqr <= 0.0 ? 
			 vec4( reflect( incident, normal ).xyz, -1 ) : 
			 vec4( normalize( ni_nt * incident + (ni_nt * IdotN - sqrt( cosSqr )) * normal ).xyz, 1) 
		   ); 
}

void main( void )
{
	// Stuff that we know from the beginning
	vec3 N_1 = normalize( gl_TexCoord[1].xyz );   // Surface Normal
	vec3 V   = normalize( gl_TexCoord[2].xyz  );   // View direction
	
	// Find the relective (.x) and refractive (.y) fresnel coefficients 
	//vec2 fresnel = fresnelApprox( dot( -V, N_1 ) );

	// Find the distance to front & back surface, first as normalized [0..1] values, than unprojected
	vec2 Dist = vec2( texture2D( prevDistances, gl_TexCoord[0].xy ).z, gl_TexCoord[0].z );
	Dist = local1.x / (Dist * local1.y - local1.z );
	
	// find the refraction direction
	vec3 T_1 = refraction( V, N_1, local2.x, local2.y ).xyz; 

	float d_V = Dist.y - Dist.x;

	// Compute approximate exitant location & surface normal
	//vec4 P_2_tilde = vec4( T_1 * d_V * multiplier + gl_TexCoord[2].xyz, 1.0);
	vec4 P_2_tilde = vec4( T_1 * d_V + gl_TexCoord[2].xyz, 1.0);
	vec3 N_2 = texture2D( backFace, ProjectToTexCoord( P_2_tilde ) ).xyz;
	float dotN2 = dot( N_2.xyz, N_2.xyz );
	N_2 = normalize( 2.0*N_2-1.0 );

	// What happens if we lie in a black-texel?  Means no normal!  Conceptually,
	//   this means we pass thru "side" of object.  Use norm perpindicular to view
	if ( dotN2 == 0.0 )
		N_2 = normalize( vec3( T_1.x, T_1.y, 0 ) );

	// Refract at the second surface
	vec4 T_2 = refraction( T_1, -N_2, local2.z, local2.w );
	float TIR = T_2.w;
	
	// Scale the vector so that it's got a unit-length z-component
	vec4 scaled_T_2 = vec4(T_2.xyz,0) / -T_2.z;

	// Compute the texture locations of ctrPlusT2 and refractToNear.
	float index, minDist = 1000.0, deltaDist = 1000.0;
	for (index = 0.0; index < 2.0; index += 1.0)
	{
		float texel = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + scaled_T_2 * index ) ).x;
		float distA = -(local1.x / (texel * local1.y - local1.w)) + P_2_tilde.z;
		if ( abs(distA-index) < deltaDist )
		{
			deltaDist = abs(distA-index);
			minDist = index;
		}
	}
	
	float distOld = minDist;	
	for (index = 0.0; index < 10.0; index += 1.0)
	{
		float texel1 = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + distOld * scaled_T_2 ) ).x;
		distOld = -(local1.x / (texel1 * local1.y - local1.w)) + P_2_tilde.z;
	}
	
	gl_FragData[0].xyz = P_2_tilde.xyz + distOld * scaled_T_2.xyz;
	gl_FragData[0].w   = TIR; 
	gl_FragData[1].xyz = P_2_tilde.xyz;
	gl_FragData[1].w   = length ( distOld * scaled_T_2.xyz );
}