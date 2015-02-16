// Output the surface normal to texture

void main( void )
{ 
	gl_FragData[0].rgb = normalize(gl_TexCoord[0].xyz);
	gl_FragData[0].a = max( gl_TexCoord[0].w, 0.0 );
}

