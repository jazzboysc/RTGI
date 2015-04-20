#version 430 core

in vec4 Temp[6];
out vec4 Output;

// splatResolutionModifier is an intensity modifier that results
//   from changing various resolution parameters using the user
//   interface, scene file, or simply traversal through the hierarchy.
//   In order to keep the correct splat intensity, we need to know
//   what level of subdivision has been applied to this photon.
uniform float splatResolutionModifier;

// Since our gl_FragCoord is in image space, we need to know how
//   big this image is if we plan to use the gl_FragCoord to do
//   useful things independent of resolution.
uniform float renderBufRes;

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

void main(void)
{
	// We'll fix our Gaussian splat size at just under 3 pixels.  Set 
	//    Gaussian distribution parameters.
    float splatSize = 2.5;
    float sizeSqr = splatSize*splatSize;
	float isInsideGaussian = 0.0;
 
	// We need to compute how far this fragment is from the center of the splat.  
	//    We could do this using point sprites, but our experience is the final 
	//    framerate is significantly faster this way.  You may find differently.
	vec2 fragLocation = gl_FragCoord.xy;
	vec2 pointLocation = (Temp[2].xy * 0.5 + 0.5) * renderBufRes;
    
    // Gaussian from Graphics Gems I, "Convenient anti-aliasing filters that minimize bumpy sampling"
    float alpha = 0.918;
    float beta_x2 = 3.906;     // == beta*2 == 1.953 * 2; 
    float denom = 0.858152111; // == 1 - exp(-beta); 
    float distSqrToSplatCtr = dot(fragLocation - pointLocation, fragLocation - pointLocation);
    float expResults = exp( -beta_x2*distSqrToSplatCtr/sizeSqr );
    
    //  Are we even inside the Gaussian?
    isInsideGaussian = ( distSqrToSplatCtr/sizeSqr < 0.25 ? 1.0 : 0.0 ) ;
    
    // Make sure the Gaussian intensity is properly normalized.
	float normalizeFactor = 10.5 * sizeSqr / 25.0;
    
    // Compute the Gaussian intensity
    expResults = alpha + alpha*((expResults-1.0)/denom);
    
	vec4 a = Temp[3];
    // Now compute the fragment color.  Note:Temp[3] is the spotlight color.
    //    Combine the intensity modifier, the check if we're inside the gaussian, 
    //    and the normalized splat intensity.
    Output = Temp[3] * vec4( splatResolutionModifier * isInsideGaussian * expResults / normalizeFactor );

}