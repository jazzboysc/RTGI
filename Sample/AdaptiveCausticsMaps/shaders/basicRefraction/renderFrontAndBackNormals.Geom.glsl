
#extension GL_EXT_geometry_shader4: enable

// Output the surface normal to one of two different
//    rendering layers, based upon if it is front or back geometry.
//    Also, pass down the surface normal to the frag shader and
//    store the eye-space distance to the fragment in the
//    alpha channel of this texture (we'll negate this distance
//    if we're back facing, as a flag).

void main( void )
{ 
	gl_Layer       = gl_TexCoordIn[0][0].w < 0.0 ? 1 : 0;
	float back     = gl_TexCoordIn[0][0].w < 0.0 ? -1 : 1;
	
	gl_Position    = gl_PositionIn[0];
	gl_TexCoord[0].xyz = gl_TexCoordIn[0][0].xyz;
	gl_TexCoord[0].a = back*gl_TexCoordIn[0][1].x;
	EmitVertex();
	gl_Position    = gl_PositionIn[1];
	gl_TexCoord[0].xyz = gl_TexCoordIn[1][0].xyz;
	gl_TexCoord[0].a = back*gl_TexCoordIn[1][1].x;
	EmitVertex();
	gl_Position    = gl_PositionIn[2];
	gl_TexCoord[0].xyz = gl_TexCoordIn[2][0].xyz;
	gl_TexCoord[0].a = back*gl_TexCoordIn[2][1].x;
	EmitVertex();
	EndPrimitive();	
}

