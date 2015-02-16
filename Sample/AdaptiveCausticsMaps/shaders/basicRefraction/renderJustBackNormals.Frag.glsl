// Simply output the surface normal to the first color buffer
// we'll scale it first to [0..1] so we can:
//       a) see it when we do debug displays
//       b) use an 8-bit RGBA buffer that clamps [0..1]
//
// If we happen to be rendering into 2 color buffers, the
// second will get the fragment's eye-space position.

void main( void )
{ 
	gl_FragData[0].rgb = 0.5 * gl_TexCoord[0].xyz + 0.5;
	gl_FragData[0].a = 0.0;
	gl_FragData[1].rgb = gl_TexCoord[1].xyz;
	gl_FragData[1].a = 1.0;
}

