// We're doing a deferred refraction were all the data
//    is stored in textures.  Thus our vertex shader is dead
//    simple, we just need to do an orthographical projection
//    and make sure the texture ranges from [0..1] over the quad

#extension GL_EXT_gpu_shader4 : enable

uniform mat4 gluOrtho;  

void main( void )
{	
	gl_Position = gluOrtho * gl_Vertex;
	gl_TexCoord[0] = gl_Vertex; 	
}