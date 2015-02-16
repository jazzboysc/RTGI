#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform sampler2D photonPositions, lightColor;
uniform float photonBufferResolution;

void main( void )
{
	// Find the photon's location.
	float primID = gl_PrimitiveIDIn;
	vec2 coords = vec2( gl_PositionIn[0].x, primID*(1.0/photonBufferResolution) );	
	vec4 position = texture2D( photonPositions, coords );

	// Photon is not valid...
	if( position.a < 0.0 ) return;
		
	// Find the position of the photon in light-space.
	vec4 eyeTmp = gl_ModelViewProjectionMatrix * vec4( position.xyz, 1.0 );
	gl_Position = eyeTmp;

	// Compute the normalized device coordinates of the photon.
	gl_TexCoord[4] = eyeTmp / eyeTmp.w;
	
	// Determine the color of the light, pass down.
	gl_TexCoord[5] =  texture2D( lightColor, coords );

	// Ok!  We're ready to create our photon splat.
	EmitVertex();
	EndPrimitive();
}