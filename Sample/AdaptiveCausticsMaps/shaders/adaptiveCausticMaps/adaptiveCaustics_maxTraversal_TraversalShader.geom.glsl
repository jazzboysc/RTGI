// What this shader does is perform the traversal of the photon hierarchy,
//    outputting only valid photons.  "Vaild" photons are defined as those
//    that hit the refractor, thus to do this traversal, we do not actually
//    need to know where the photons end up....  We just need to know if
//    the photons hit the refractor.  We can test this by indexing into
//    the buffer that stores the refractor's surface normal.  If there is
//    a valid surface normal (as opposed to a unrendered, background pixel),
//    then we know we have a valid photon.
//
// So, each pass through the shader, we look up into the surface normal 
//    mipmap at the appropriate level.  If *any* pixels are valid, the
//    alpha component will be < 1 (the background has alpha = 1), and we
//    output the photons for further subdivision.  If all photons are
//    invalid, we do not output anything.

#version 120 
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray refractSurface;
uniform float currHierarchyLevel;
uniform float deltaHierRes;


void main( void )
{
	// Check if photon is even valid...  (If all child rays are invalid,
	//    then all have alpha of 1, and the mipmap level also has alpha = 1)
	vec4 position = texture2DArrayLod( refractSurface, vec3(gl_PositionIn[0].xy,1), currHierarchyLevel );
	if ( position.a > 0.99 ) return;
	
	// At least some rays are valid (i.e., alpha=1 in input buffer), need to subdivide
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