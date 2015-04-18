//  We're going to output the object's surface normal
//     in the fragment shader, so make sure to pass down
//     the eye-space surface normal to the fragment level.
//
//  However, since we're going to output BOTH front and back
//     facing surfaces in the same pass, we need to determine
//     if this certex is front-facing or back-facing 
//     (the dot produt)

void main( void )
{ 
	vec3 eyeSpaceNorm  = gl_NormalMatrix * gl_Normal;
	vec4 eyeSpacePos   = gl_ModelViewMatrix * gl_Vertex;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0].xyz = eyeSpaceNorm;
	gl_TexCoord[0].w     = dot( normalize(eyeSpaceNorm), -normalize(eyeSpacePos.xyz) );
	gl_TexCoord[1]       = length( eyeSpacePos.xyz );
}

