// The traversal of the photon hierarchy involves starting
//    from a regularly-sampled seed set of photon locations, 
//    checking if those locations in the hierarchy contain
//    valid photons.  If they do, we need to recurse down
//    the hierarchy to process them, otherwise we can discard
//    them en mass.

#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform sampler2D photonPositions;
uniform float currHierarchyLevel;
uniform float deltaHierRes;

void main( void )
{
	// The alpha component of the photon buffer contains a flag that notes if
	//    the fragment is valid (i.e., hits the refractor) or not.  Thus the
	//    mipmap contains an average of this.  If the average is -10 (which is
	//    the alpha value for invalid photons) then we know all the photons
	//    "below" this in the hierarchy are also invalid and need no processing.
	vec2 coords = gl_PositionIn[0].xy;
	vec4 position = texture2DLod( photonPositions, coords, currHierarchyLevel );
	if( position.a == -10.0 ) return;
	
	// Well, at least some of the photons might be valid, so we need to subdivide
	
	gl_Position = vec4( coords.x, coords.y, 0, 0 );
	EmitVertex();
	EndPrimitive();	
	
	gl_Position = vec4( coords.x+deltaHierRes, coords.y, 0, 0 );
	EmitVertex();
	EndPrimitive();	
	
	gl_Position = vec4( coords.x+deltaHierRes, coords.y+deltaHierRes, 0, 0 );
	EmitVertex();
	EndPrimitive();
	
	gl_Position = vec4( coords.x, coords.y+deltaHierRes, 0, 0 );
	EmitVertex();
	EndPrimitive();
		
}