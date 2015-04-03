#version 430 core

out vec4 Output;

// This should never be executed....
//    Stuff should be discarded using:  
//    glEnable( GL_RASTERIZER_DISCARD_NV );
void main( void )
{
    Output = vec4( 1.0 ); 
}

