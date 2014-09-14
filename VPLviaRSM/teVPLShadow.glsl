#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform int VPLPass;
uniform sampler2D RSMPositionSampler;
uniform sampler2D RSMNormalSampler;
uniform sampler1D RSMSampleSampler;

in vec4 tcNormalWorld[];
out float teViewPosZ;
out float teDepth;

void main()
{	
    vec2 sampleLoc = texelFetch(RSMSampleSampler, VPLPass, 0).rg;
    vec3 VPLPosition = texture(RSMPositionSampler, sampleLoc).rgb;
    vec3 VPLNormal = texture(RSMNormalSampler, sampleLoc).rgb;
    VPLNormal = VPLNormal*2.0 - 1.0;
    mat4 View;

    vec4 worldPos = gl_in[0].gl_Position.xyzw * gl_TessCoord.x +
        gl_in[1].gl_Position.xyzw * gl_TessCoord.y +
        gl_in[2].gl_Position.xyzw * gl_TessCoord.z;

    vec4 viewPos = View * worldPos;
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    halfDir.x = -halfDir.x / halfDir.z;
    halfDir.y = -halfDir.y / halfDir.z;
    halfDir.z = (len - 0.01) / (50.0 - 0.01);

    teViewPosZ = viewPos.z;
    teDepth = halfDir.z;
    gl_Position.xyz = halfDir;
    gl_Position.w = 1.0;
}
