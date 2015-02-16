// This shader is used for splatting the converged/unconverged 
//   photons resulting from our adaptive traversal using the 
//   maximum caustic error metric.

// For reasons involving not wanting to use more than 4 components
//   per photon when storing intra-pass temporaries, we pre-project
//   these photons into the 2D (u,v) coordinate space of the 
//   caustic map, which is why we simply pass down the x & y 
//   coordinates of the photon to the rasterizer.

void main( void )
{
	gl_Position = vec4( gl_Vertex.xy, 0.0, 1.0 );
	gl_TexCoord[2] = gl_Vertex;
}



