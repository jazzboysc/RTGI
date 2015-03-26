#version 430 core

in vec4 gNormalView[];

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
	causticsDebugBuffer.first = gNormalView[0].w;
	causticsDebugBuffer.second = gl_Layer;

	// Eye space normal
	Output.rgb = vec3(1,0,0);//normalize(gNormalView[0].xyz);
	// Facing direction flag
	Output.a = 1; //max( gNormalView[0].w, 1.0 );
}

