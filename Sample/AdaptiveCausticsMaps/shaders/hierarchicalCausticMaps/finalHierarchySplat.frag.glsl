// Now that we've selected the layer of the texture-array / render-to-mipmap
//    that we want to draw in, checked to see that the photons do not converge,
//    etc...  We get to actually splat.

// The resolution of the (maximum mipmap layer of the) caustic map
uniform vec2 causticMapRes;

// Depending on the ultimate # of photons we plan to use, we need to modifiy
//    the intensity of each.  Clearly, if a light's intensity is spread over
//    16 photons, they each have a brighter color than if we divide the light
//    amongst 256 photons!
uniform float photonResolutionMultiplier;


void main( void )
{
	// We are varying our splat size based upon the value chosen in the geometry shader
	float splatSize        = gl_TexCoord[4].z;
    float sizeSqr          = splatSize*splatSize;
	float isInsideGaussian = 0.0;
 
	// We need to compute how far this fragment is from the center of the splat.  
	//    We could do this using point sprites, but our experience is the final 
	//    framerate is significantly faster this way.  You may find differently.
	vec2 fragLocation  = gl_FragCoord.xy;
	vec2 pointLocation = gl_TexCoord[4].xy * causticMapRes.x;
	vec2 deltaLocation = fragLocation - pointLocation;
    
    // Gaussian from Graphics Gems I, "Convenient anti-aliaseing filters that minimize bumpy sampling"
    float alpha   = 0.918;
    float beta_x2 = 3.906;       // == beta*2 == 1.953 * 2; 
    float denom   = 0.858152111; // == 1 - exp(-beta);
   
    // Because these splats are frequently shrunken when rendered into smaller layers in 
    //   the mipmap, it introduces banding artifacts.  To help avoid this, we add some
    //   multisampling of the Gaussian to avoid some possible artifacts.
    vec4 distSqrToSplatCtr;
    distSqrToSplatCtr.x = dot(deltaLocation + gl_TexCoord[5].xy, 
                              deltaLocation + gl_TexCoord[5].xy);
    distSqrToSplatCtr.y = dot(deltaLocation - gl_TexCoord[5].xy, 
                              deltaLocation - gl_TexCoord[5].xy);
    distSqrToSplatCtr.z = dot(deltaLocation + gl_TexCoord[5].zw, 
                              deltaLocation + gl_TexCoord[5].zw);
    distSqrToSplatCtr.w = dot(deltaLocation - gl_TexCoord[5].zw, 
                              deltaLocation - gl_TexCoord[5].zw);
    float texelToSplatDistSqr = dot(deltaLocation, deltaLocation);
    vec4 results = exp( -beta_x2*distSqrToSplatCtr/sizeSqr );
    float expResults = 0.25*(results.x + results.y + results.z + results.w);
    
    //  Are we even inside the Gaussian?
    isInsideGaussian = ( texelToSplatDistSqr/sizeSqr < 0.25 ? gl_TexCoord[4].w : 0.0 ) ;
    
    // Make sure the Gaussian intensity is properly normalized.
	float normalizeFactor = 0.42 * sizeSqr; 
    
    // Compute the Gaussian intensity
    expResults = alpha + alpha*((expResults-1.0)/denom);
    
    // Now compute the fragment color.  Note:  gl_TexCoord[6] is the spotlight color.
    //    Combine the intensity modifier, the check if we're inside the gaussian, 
    //    and the normalized splat intensity.
    gl_FragData[0] = gl_TexCoord[6] * vec4( photonResolutionMultiplier * isInsideGaussian * expResults / normalizeFactor );
}

