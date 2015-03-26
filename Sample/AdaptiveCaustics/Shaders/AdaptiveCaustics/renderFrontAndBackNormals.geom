#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;



in vec4 vPositionWorld[];
in vec4 vPositionView[];
in vec4 vPositionProj[];

in vec4 vNormalWorld[];
in vec4 vNormalView[];

in float distanceView[];

out vec4 gNormalView[];

// Output the surface normal to one of two different
//    rendering layers, based upon if it is front or back geometry.
//    Also, pass down the surface normal to the frag shader and
//    store the eye-space distance to the fragment in the
//    alpha channel of this texture (we'll negate this distance
//    if we're back facing, as a flag).

void main( void )
{ 
	gl_Layer       = vNormalView[0].w < 0.0 ? 1 : 0;
	float back     = vNormalView[0].w < 0.0 ? -1 : 1;
	
	//causticsDebugBuffer.second = vNormalView[1].w;
	//causticsDebugBuffer.third = vNormalView[2].w;

	gl_Position    = vPositionProj[0];
	// View space normal
	gNormalView[0].xyz = vNormalView[0].xyz;
	// Facing direction flag
	gNormalView[0].w = back * distanceView[0].x;
	EmitVertex();

	gl_Position    = vPositionProj[1];
	// View space normal
	gNormalView[0].xyz = vNormalView[1].xyz;
	// Facing direction flag
	gNormalView[0].w = back * distanceView[1].x;
	EmitVertex();

	gl_Position    = vPositionProj[2];
	// View space normal
	gNormalView[0].xyz = vNormalView[2].xyz;
	// Facing direction flag
	gNormalView[0].w = back * distanceView[2].x;
	EmitVertex();

	EndPrimitive();	
}

