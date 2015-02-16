// The traversal of the photon hierarchy involves starting
//    from a regularly-sampled seed set of photon locations, 
//    checking if those locations in the hierarchy contain
//    valid photons.  If they do, we need to recurse down
//    the hierarchy to process them, otherwise we can discard
//    them en mass.

// The vertex shader is a placeholder.  All the important work
//    goes on the geometry shader.

#version 120 
#extension GL_EXT_gpu_shader4 : enable


void main( void )
{  
	gl_Position = gl_Vertex;
}