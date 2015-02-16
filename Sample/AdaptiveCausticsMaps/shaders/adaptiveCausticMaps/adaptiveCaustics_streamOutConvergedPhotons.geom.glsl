// This shader takes a look at all photons in a stream, compares
//    their divergence (computed in adaptiveCaustics_detectConvergingPhotons.geom.glsl)
//    with a specified threshold (here 0.003) to see if it converges.  If the
//    divergence is negative, that means the photon is part of a 2x2 cluster
//    that (partially) misses the refractor.  Clearly those cannot converge.

#version 120 
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform float intensityModifier;

void main( void )
{	
	// Does this photon exceed the threshold specified for converging photon quads?
	// Does this photon belong to a quad that (partially) misses the refractor?
	// In either case, this photon is NOT a converging photon, so move to the next one.
	if ( gl_PositionIn[0].w > 0.003 || gl_PositionIn[0].w < 0.0 ) return;

	// Ah ha!  We found one.  Output the converged photons to the stream
	gl_Position = vec4( gl_PositionIn[0].xyz, intensityModifier);
	EmitVertex();
	EndPrimitive();
}



