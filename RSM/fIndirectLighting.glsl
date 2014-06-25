#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;
varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform vec3 materialColor;

uniform mat4 lightProjectorView;
uniform mat4 lightProjectorProj;

uniform float sampleRadius;
uniform int sampleCount;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D fluxSampler;
uniform sampler2D samplingPatternSampler;

void main()
{
    vec3 normalView = vNormalView.xyz;
    normalView = normalize(normalView);
    normalView  = normalView*0.5 + 0.5;
	gl_FragData[0] = vec4(normalView, 1.0);
}
