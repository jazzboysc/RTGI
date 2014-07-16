#version 120

varying vec2 pTCoord;

uniform sampler2D renderSampler;

void main()
{
	gl_FragData[0] = texture2D(renderSampler, pTCoord);
    //gl_FragData[0] = vec4(0, 1, 0 ,1);
}
