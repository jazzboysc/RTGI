// This shader takes in a 2 x 2 group of photons and checks if
//    they converge.  There is no GL_QUAD type of input to the
//    geometry shader (which would be ideal for this method), so
//    we fake it using a GL_LINE_ADJACENCY_EXT which also requires
//    4 inputs.

// The associated vertex shader actually computes a 2D (u,v) image
//    space coordinate for the final caustic map (that we could,
//    and will, use to directly splat into the map).  Using this
//    coordinate for each, we can compute some sort of maximum
//    divergence in this quad of photons and check to make sure
//    all four photons are valid (i.e., hit the refractor).  We'll
//    actually worry about detecting convergence when we stream
//    out the relevant photons. 

#version 120 
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable


void main( void )
{	
	// Compute a maximum divergence in any direction of this quad.  We found
	//    this set of distances (not all 6) was sufficient to capture the behavior
	float maxDist = max( distance(gl_PositionIn[0].xy,gl_PositionIn[1].xy), 0 );
	maxDist = max( distance(gl_PositionIn[0].xy,gl_PositionIn[3].xy), maxDist );
	maxDist = max( distance(gl_PositionIn[1].xy,gl_PositionIn[2].xy), maxDist );
	maxDist = max( distance(gl_PositionIn[2].xy,gl_PositionIn[3].xy), maxDist );
	
	// If any of the photons are invalid, then by definition they cannot converge!
	//    In this case, we really have no hope of converging...  Our gl_TexCoord[1]
	//    stores whether each photon is valid (i.e., hits the refractor)
	float invalidSum = gl_TexCoordIn[0][0].x + gl_TexCoordIn[1][0].x + 
	                   gl_TexCoordIn[2][0].x + gl_TexCoordIn[3][0].x;
	float converged = invalidSum > 0.5 ? -1.0 : maxDist;
		
	// Find the position of the photon in light-space.  The 3rd component is a
	//    splat intensity modifier based on how far we've traversed the hierarchy 
	//    thus far.
	gl_Position = vec4(gl_PositionIn[0].xy, gl_TexCoordIn[0][3].w, converged);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4(gl_PositionIn[1].xy, gl_TexCoordIn[1][3].w, converged);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4(gl_PositionIn[2].xy, gl_TexCoordIn[2][3].w, converged);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4(gl_PositionIn[3].xy, gl_TexCoordIn[3][3].w, converged);
	EmitVertex();
	EndPrimitive();
}



