// The traversal of the photon hierarchy involves starting
//    from a regularly-sampled seed set of photon locations, 
//    checking if those locations in the hierarchy contain
//    valid photons.  If they do, we need to recurse down
//    the hierarchy to process them, otherwise we can discard
//    them en mass.


// This should never be executed....
//    Photons should be discarded using:  
//    glEnable( GL_RASTERIZER_DISCARD_NV );
void main( void )
{
    gl_FragData[0] = vec4( 1.0 ); 
}

