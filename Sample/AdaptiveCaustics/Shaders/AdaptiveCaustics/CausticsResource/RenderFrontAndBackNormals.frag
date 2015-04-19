#version 430 core

in vec4 gNormalView;

layout(std430, binding = 0)  buffer _causticsDebugBuffer
{
    // Debug.
    float first;
    float second;
    float third;
    float fourth;

} causticsDebugBuffer;

// Output the surface normal to texture
out vec4 Output;

void main( void )
{ 
	causticsDebugBuffer.first = 10.0f;

	// Eye space normal
	Output.rgb = normalize(gNormalView.xyz);
	// Facing direction flag
	Output.a = max( gNormalView.w, 0.0 );
}

