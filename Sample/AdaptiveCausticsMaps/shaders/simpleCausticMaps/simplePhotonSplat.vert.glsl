#version 120 
#extension GL_EXT_gpu_shader4 : enable

uniform float photonBufferResolution;


void main( void )
{  
	gl_Position = vec4( gl_InstanceID / photonBufferResolution, gl_VertexID, 0, 0 );
}