#version 430 core

in vec2 pTCoord;

uniform sampler2D ColorTex;
uniform sampler2D DepthTex;

void main( void )
{
	//gl_FragColor = texture( ColorTex, pTCoord );
	gl_FragDepth = texture( DepthTex, pTCoord ).z;
}

