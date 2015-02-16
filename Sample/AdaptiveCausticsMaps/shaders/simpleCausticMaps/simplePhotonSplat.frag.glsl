// Actually splat the photon into the caustic map

uniform float causticMapResolution, photonResolutionMultiplier;


void main( void )
{
    float splatSize = 2.5;
    float sizeSqr = splatSize*splatSize;
	float isInsideGaussian = 0.0;
 
	// We need to compute how far this fragment is from the center of the point.  We could do
	//    this using point sprites, but the __final__ framerate is 10x faster this way.
	vec2 fragLocation = gl_FragCoord.xy;
	vec2 pointLocation = (gl_TexCoord[4].xy * 0.5 + 0.5) * causticMapResolution;
    
    // Gaussian from Graphics Gems I, "Convenient anti-aliaseing filters that minimize bumpy sampling"
    float alpha = 0.918;
    float beta_x2 = 3.906;     // == beta*2 == 1.953 * 2; 
    float denom = 0.858152111; // == 1 - exp(-beta);
    
    float distSqrToSplatCtr = dot(fragLocation - pointLocation, fragLocation - pointLocation);
    float expResults = exp( -beta_x2*distSqrToSplatCtr/sizeSqr );
    
    isInsideGaussian = ( distSqrToSplatCtr/sizeSqr < 0.25 ? 1.0 : 0.0 ) ;
	float normalizeFactor = 10.5 * sizeSqr / 25.0;
    
    expResults = alpha + alpha*((expResults-1.0)/denom);
    gl_FragData[0] = gl_TexCoord[5] * vec4( photonResolutionMultiplier * isInsideGaussian * expResults / normalizeFactor );
}

