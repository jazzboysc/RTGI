// This shader takes a look at all photons in a stream, compares
//    their divergence (computed in adaptiveCaustics_detectConvergingPhotons.geom.glsl)
//    with a specified threshold (here 0.003) to see if it diverges.  This
//    effectively does the opposite of the ConvergedPhotons shader, allowing
//    us to split a stream of photons into 2 new streams: converged and unconverged.
//    Also note that clusters that miss the refractor are ignored, and not passed to
//    either stream, with this splitting approach.

#version 120 
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform float intensityModifier;

void main( void )
{	
	// Does this photon fall below the convergence criteria?  Then we handled it before. 
	// Does this photon belong to a quad that misses the refractor?  Ignore it.
	if ( gl_PositionIn[0].w < 0.003 ) return;

	// Output the unconverged photons to a stream
	gl_Position = vec4( gl_PositionIn[0].xyz, intensityModifier);
	EmitVertex();
	EndPrimitive();
}



