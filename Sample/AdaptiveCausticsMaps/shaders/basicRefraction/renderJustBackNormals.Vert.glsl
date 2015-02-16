//  We're going to output the object's surface normal
//     in the fragment shader, so make sure to pass down
//     the eye-space surface normal to the fragment level.

void main( void )
{ 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0].xyz = gl_NormalMatrix * gl_Normal;
	gl_TexCoord[1]     = gl_ModelViewMatrix * gl_Vertex;
}

