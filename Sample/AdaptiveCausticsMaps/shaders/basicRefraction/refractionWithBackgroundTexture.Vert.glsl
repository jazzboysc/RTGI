// We'll be doing refraction in the fragment shader, so make
//   sure to pass down the appropriate values to the fragment
//   shader.


void main( void )
{
	vec4 eyeTmp = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Transform vertex position to clip coordinates
	gl_Position = eyeTmp;
	
	// We're also going to need the location of this vertex (and each pixel
	//   in turn in screen space.  Since CG won't let us access fragment.position
	//   in the fragment shader, do the math ourself and pass it down)
	eyeTmp.xyz = eyeTmp.xyz / eyeTmp.w;
	gl_TexCoord[0] = 0.5 * eyeTmp + 0.5;

	// Compute vertex location in eye coordinates.  This also serves
	//   as the view vector, if normalized.  We need both, so we store
	//   two values here.
    eyeTmp = gl_ModelViewMatrix * gl_Vertex; 
    gl_TexCoord[2] = eyeTmp / eyeTmp.w;
    gl_TexCoord[1].xyz = gl_NormalMatrix * gl_Normal.xyz;
    gl_TexCoord[1].w = 0.0; // gl_Normal.w;

	// Pass down input curvature data.
	gl_TexCoord[7] = gl_MultiTexCoord7;

}