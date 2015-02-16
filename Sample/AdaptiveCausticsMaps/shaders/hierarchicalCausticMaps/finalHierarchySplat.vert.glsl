// OK!  We've got our photons and we're ready to splat
//    them into the caustic map.  

// All of the work goes on in the geometry and fragment shaders.

#version 120 
#extension GL_EXT_gpu_shader4 : enable

void main( void )
{
	gl_Position = gl_Vertex;
}