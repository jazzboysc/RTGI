// This simple shader is used quite a bit.

// Basically, it's a no-op in the vertex shader.
//    We could use the standard GL pipeline, but because
//    of the simplicity here, it may be the graphics card
//    can optimize this down to a really quick little shader.

// Note not all GLSL programs using this shader need the
//    texture coord passed down, but in the interests of
//    one less shader to write/load well combine both types
//    into one shader.

// Evidently, some driver revions require the GLSL version to
//    be identical across all shader types.  Whee!

#version 430 core

in vec4 vPosition;
in vec2 vTCoord;

out vec2 pTCoord;
out vec4 vPositionWorld;

void main()
{
	vPositionWorld = vPosition;
	pTCoord = vTCoord;
}
