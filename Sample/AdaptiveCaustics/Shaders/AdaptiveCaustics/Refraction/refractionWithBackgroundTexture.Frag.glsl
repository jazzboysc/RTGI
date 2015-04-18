

uniform vec4 local1, local2;
uniform float useShadows, useFog, extinctionCoef;
uniform vec4 epS, epT, epR, epQ;

uniform samplerCube environmentMap;  
uniform sampler2D otherObjsEyeDepth; 
uniform sampler2D prevDistances;     
uniform sampler2D otherObjsEye;	     
uniform sampler2D backFace;	    
uniform sampler2D spotLight;    
uniform vec4 glassColor; 
uniform vec4 sceneAmbient;

// Based upon an eye-space coordinate, compute the spotlight color.
vec4 SpotLightColor( vec4 eyeSpaceCoord )
{
	vec4 smapCoord = vec4( dot( epS, eyeSpaceCoord ), dot( epT, eyeSpaceCoord ),
						   dot( epR, eyeSpaceCoord ), dot( epQ, eyeSpaceCoord ) );
	smapCoord /= smapCoord.w;
	return ( all(equal(smapCoord.xyz,clamp(smapCoord.xyz,vec3(0),vec3(1)))) ? 
					texture2D( spotLight, smapCoord.xy, 0.0 ): 
					vec4( 0.25 ) );
}

// Based upon an eye-space coordinate, compute an output caustic map (image-space)
//    coordinate.  This is a simple application of the projection matrix followed
//    by the homogeneous divide.
inline vec2 ProjectToTexCoord( vec4 eyeSpacePos )
{
	vec4 projLoc = gl_ProjectionMatrix * eyeSpacePos;
	return ( 0.5*(projLoc.xy / projLoc.w) + 0.5 );
}

// A simple refraction shader similar to the built in GLSL one (only this one is real)
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

// Approximate the fresnel coefficients using Schlick's approximation
vec2 fresnelApprox( float cosAng )
{
	float oneMinus = 1.0-cosAng;
	float approx = 0.05 + 0.95*(oneMinus*oneMinus*oneMinus*oneMinus*oneMinus);
	return vec2( approx, 1-approx );
}

void main( void )
{
	vec2 Dist, fresnel;
	vec4 tmp, reflectedColor;

	// Stuff that we know from the beginning 
	vec3 N_1 = normalize( gl_TexCoord[1].xyz );   // Surface Normal
	vec3 V   = normalize( gl_TexCoord[2].xyz  );   // View direction
	float NdotV = dot( -V, N_1 );
	
	// Compute a direction for the light (to use for a Phong reflected component)
	vec3 toLight = normalize(  gl_LightSource[0].position.xyz - gl_TexCoord[2].xyz );
	vec3 half = normalize( toLight - V );
	float NdotH = max( 0.0, dot( N_1, half ) );
	vec4 reflectedLightColor = pow( NdotH, 50.0 ) * ( useShadows < 0.5 ? vec4(1.0) : SpotLightColor(gl_TexCoord[2]) );
	
	// Find the relective (.x) and refractive (.y) fresnel coefficients 
	fresnel = fresnelApprox( NdotV );

	// Find the distance to front & back surface, first as normalized [0..1] values, than unprojected
	Dist.x = texture2D( prevDistances, gl_TexCoord[0].xy ).z;	
	Dist.y = length( gl_TexCoord[2].xyz );
	Dist.x = 2 * local1.x / (Dist.x * local1.y - local1.z );

	// Distance between front & back surfaces
	float d_V = -Dist.y - Dist.x;
	
	// compute the reflection direction and the reflection color
	//    we do a matrix multiply to account for (potential) user rotation of the environment
	tmp = vec4( reflect( V, N_1 ), 0 );
	reflectedColor = fresnel.x * (20.0 * reflectedLightColor + 2.0*sceneAmbient);
	
	// find the refraction direction
	vec3 T_1 = refraction( V, N_1, local2.x, local2.y ).xyz;

	// We need to find the angle betwen T_1 & N and T_1 & V
	// float angle_T_N = max( 0.0, acos( -dot( T_1, N_1 ) ) );  
	// float angle_T_V = max( 0.0, acos( dot( T_1, V ) ) ); 
	// float angleSum = angle_T_N + angle_T_V;

	// out approx distance is: ( obj_scale * precomputed_d_N * angle_T_V +
    //    d_V * angle_T_N ) / sumOfAngles_TV_and_TN 
	float d_tilde = d_V;   //(2 * gl_TexCoord[1].w * angle_T_V + d_V * angle_T_N) / angleSum;
	// float d_tilde = d_V * angle_T_N / angleSum;
	// float d_tilde = (4 * angle_T_V + d_V * angle_T_N) / angleSum;

	// Compute approximate exitant location & surface normal
	vec4 P_2_tilde = vec4( T_1 * d_tilde + gl_TexCoord[2].xyz, 1.0);
	vec3 N_2 = texture2D( backFace, ProjectToTexCoord( P_2_tilde ) ).xyz;
	float dotN2 = dot( N_2.xyz, N_2.xyz );
	N_2 = normalize( 2*N_2-1 );

	// What happens if we lie in a black-texel?  Means no normal!  (d_tilde is too big...)
	if ( dotN2 == 0.0 )
	{
		// Conceptually, we pass thru the "side" of the object (not front/back)
		// Use a 'normal' perpindicular to view direction (but generally along same
		//     direction as our refracted direction T_1)
		tmp = vec4( T_1.x, T_1.y , 0.0, dot(T_1.xy,T_1.xy) );	
		N_2 = tmp.xyz / tmp.w;
	}

	// Refract at the second surface
	vec4 T_2 = refraction( T_1, -N_2, local2.z, local2.w );
	bool TIR = T_2.w < 0;
	T_2.w = 0;

	// Scale the vector so that it's got a unit-length z-component
	vec4 tmpT2 = T_2 / -T_2.z;

	// Compute the texture locations of ctrPlusT2 and refractToNear.
	float index, minDist = 1000.0, deltaDist = 1000.0;
	for (index = 0.0; index < 2.0; index += 1.0)
	{
		float texel = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + tmpT2 * index ) ).x;
		float distA = -(local1.x / (texel * local1.y - local1.w)) + P_2_tilde.z;
		
		if (abs(distA-index) < deltaDist)
		{
			deltaDist = abs(distA-index);
			minDist = index ;
		}
	}
		
	float distOld = minDist;	
	
	for (index = 0.0; index < 10.0; index += 1.0)
	{
		float texel1 = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + distOld * tmpT2 ) ).x;
		distOld = -(local1.x / (texel1 * local1.y - local1.w)) + P_2_tilde.z;
	}
	
	vec4 transmitColor = vec4( exp(-d_V*glassColor.a)*glassColor.rgb, 1.0);
	vec4 refractedColor = transmitColor*texture2D( otherObjsEye, ProjectToTexCoord( P_2_tilde + distOld * tmpT2 ) );
	
	
	// Now we know the reflected color and refracted color... Add for final result
	gl_FragColor = vec4( reflectedColor.xyz + fresnel.y * refractedColor.xyz, 1.0); 
}