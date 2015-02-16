// This shader is identical to deferredRefraction.frag.glsl except
//    that it allows a fresnel reflection of the refractive geometry's
//    surface from more than one light.  Dead simple change.  Go look
//    at the prior shader for better comments... 

#extension GL_EXT_gpu_shader4 : enable

uniform vec4 local1, local2;

uniform sampler2D otherObjsEyeDepth;   
uniform sampler2D otherObjsEye;	  
uniform sampler2D spotLight;   

uniform sampler2DArray geomPos;
uniform sampler2DArray geomNorm;
uniform sampler2DArray geomDepth;

uniform mat4 gluOrtho, shadowMatrix;  
uniform float renderBufRes, tanEyeFovy2;
uniform vec4 glassColor;
uniform vec4 sceneAmbient;

vec4 SpotLightColor( vec4 eyeSpaceCoord )
{
	vec4 smapCoord = shadowMatrix * eyeSpaceCoord;
	smapCoord /= smapCoord.w;
	return ( all(equal(smapCoord.xyz,clamp(smapCoord.xyz,vec3(0),vec3(1)))) ? 
					texture2D( spotLight, smapCoord.xy, 0.0 ): 
					vec4( 0.25 ) );
}

inline vec2 ProjectToTexCoord( vec4 eyeSpacePos )
{
	vec4 projLoc = gl_ProjectionMatrix * eyeSpacePos;
	return ( 0.5*(projLoc.xy / projLoc.w) + 0.5 );
}

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

vec2 fresnelApprox( float cosAng )
{
	float oneMinus = 1.0-cosAng;
	float approx = 0.05 + 0.95*(oneMinus*oneMinus*oneMinus*oneMinus*oneMinus);
	return vec2( approx, 1.0-approx );
}


vec3 DirectionFromScreenCoord( vec2 texPos )
{
	vec3 dir = vec3( tanEyeFovy2 * ( 2.0*texPos - 1.0 ), -1.0 );
	return normalize( dir );
}


void main( void )
{
	//gl_FragColor = vec4(1);
	//return;

	//vec3 coord = vec3( gl_FragCoord.xy/2048.0, 0.0 );
	vec3 coord = vec3( gl_TexCoord[0].xy, 0.0 );
	
	float outside = 0.0, noBackNorm=0.0;
	vec2 Dist, fresnel;
	vec4 reflectedColor;
	
	vec4 tmp = texture2DArray( geomNorm, coord );
	if (dot(tmp.xyz,tmp.xyz) < 0.1 ) discard;

	// Get front facing position and normal
	vec4 P_1 = vec4( tmp.w*DirectionFromScreenCoord( coord.xy ), 1.0 );
	
	// Check if this pixel has refractive materials or not.
	outside = dot(tmp.xyz,tmp.xyz) < 0.01 ? 1.0 : 0.0;
	
	// Compute normalized V and N_1 values.
	vec3 N_1 = normalize( tmp.xyz );   // Surface Normal
	vec3 V   = normalize( P_1.xyz  );   // View direction
	float NdotV = dot( -V, N_1 );
	
	
	// Compute a direction for the light (to use for a Phong reflected component)
	vec3 toLight0 = normalize(  gl_LightSource[0].position.xyz - P_1.xyz );
	vec3 half0 = normalize( toLight0 - V ); 
	float NdotH0 = max( 0.0, dot( N_1, half0 ) );
	vec3 toLight1 = normalize(  gl_LightSource[1].position.xyz - P_1.xyz );
	vec3 half1 = normalize( toLight1 - V ); 
	float NdotH1 = max( 0.0, dot( N_1, half1 ) );
	
	vec4 reflectedLightColor = 0.75 * pow( NdotH0, 50.0 ) * SpotLightColor( P_1 );
	reflectedLightColor += 0.75 * pow( NdotH1, 50.0 ) * SpotLightColor( P_1 );
	
	// Find the relective (.x) and refractive (.y) fresnel coefficients 
	fresnel = fresnelApprox( NdotV );

	// Find the distance to front & back surface, first as normalized [0..1] values, than unprojected
	Dist.y = length( P_1.xyz );
	Dist.x = texture2DArray( geomDepth, vec3(coord.xy,1) ).z;	
	Dist.x = 2.0 * local1.x / (Dist.x * local1.y - local1.z );

	// Distance between front & back surfaces
	float d_V = -Dist.y - Dist.x;
	
	// compute the reflection direction and the reflection color
	//    we do a matrix multiply to account for (potential) user rotation of the environment
	tmp = vec4( reflect( V, N_1 ), 0.0 );
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
	vec4 P_2_tilde = vec4( T_1 * d_tilde + P_1.xyz, 1.0);
	//vec3 N_2 = texture2D( backNorm, ProjectToTexCoord( P_2_tilde ) ).xyz;
	vec3 N_2 = texture2DArray( geomNorm, vec3( ProjectToTexCoord( P_2_tilde ), 1.0) ).xyz;
	float dotN2 = dot( N_2.xyz, N_2.xyz );

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
	bool TIR = T_2.w < 0.0;
	T_2.w = 0.0;

	// Scale the vector so that it's got a unit-length z-component
	vec4 tmpT2 = T_2 / -T_2.z;

	// Compute the texture locations of ctrPlusT2 and refractToNear.
	float index, minDist = 1000.0, deltaDist = 1000.0;
	for (index = 0.0; index < 2.0; index += 1.0)
	{
		float texel = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + tmpT2 * index ) ).x;
		float distA = -(local1.x / (texel * local1.y - local1.w)) + P_2_tilde.z;
		if ( abs(distA-index) < deltaDist )
		{
			deltaDist = abs(distA-index);
			minDist = index;
		}
	}
		
	float distOld = minDist;	
	for (float index = 0.0; index < 10.0; index += 1.0)
	{
		float texel1 = texture2D( otherObjsEyeDepth, ProjectToTexCoord( P_2_tilde + distOld * tmpT2 ) ).x;
		distOld = -(local1.x / (texel1 * local1.y - local1.w)) + P_2_tilde.z;
	}
	
	vec4 transmitColor = vec4( exp(-d_V*glassColor.a)*glassColor.rgb, 1.0);
	vec4 refractedColor = transmitColor * texture2D( otherObjsEye, ProjectToTexCoord( P_2_tilde + distOld * tmpT2 ) );
	gl_FragColor = vec4( reflectedColor.xyz + fresnel.y * refractedColor.xyz, 1.0); 
}