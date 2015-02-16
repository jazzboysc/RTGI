// What this shader does is perform the traversal of the photon hierarchy,
//    outputting only valid photons.  "Vaild" photons are defined as those
//    that hit the refractor.  Unlike the traversal done using the maximum
//    error metric, we actually have more information -- we know by this
//    point where the photons end up.  So there are actually more cases
//    here, but we do not need to perform a texture lookup (all the data
//    has been passed down to the shader).
//
// If this photon is part of a converging 2x2 quad of photons, we won't
//    need to subdivide it further, so don't bother to output.  If the
//    "convergence" value is negative, our prior shader noted that the
//    fragment missed the refractor.  We can skip outputting that one.


#version 120 
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform float deltaHierRes;

void main( void )
{
	// Check if the photon already converged, in which case exit quickly
	// Check if the photon is invalid, in which case we'll have problems anyways, so exit now.
	if ( gl_TexCoordIn[0][0].a < 0.003  && gl_TexCoordIn[0][0].a > -0.5 ) return;
	
	// At least some rays are valid, need to subdivide
	gl_Position = vec4( gl_PositionIn[0].x, gl_PositionIn[0].y, 0, 0 );
	EmitVertex();
	EndPrimitive();	
	
	gl_Position = vec4( gl_PositionIn[0].x+deltaHierRes, gl_PositionIn[0].y, 0, 0 );
	EmitVertex();
	EndPrimitive();	
	
	gl_Position = vec4( gl_PositionIn[0].x+deltaHierRes, gl_PositionIn[0].y+deltaHierRes, 0, 0 );
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4( gl_PositionIn[0].x, gl_PositionIn[0].y+deltaHierRes, 0, 0 );
	EmitVertex();
	EndPrimitive();
}