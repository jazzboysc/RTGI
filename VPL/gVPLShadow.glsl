#version 430 core

layout (triangles) in;
layout (triangle_strip) out;

out float gViewPosZ;
out float gDepth;

uniform vec2 LightProjectorNearFar;

struct VPL
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec4 Flux;
    mat4 View;
};

layout(std430, binding = 0)  buffer _VPLBuffer
{
    VPL vpls[];
} VPLBuffer;

void main()
{
    mat4 View;
    const int VPLCount = 64;

    for( int i = 0; i < VPLCount; ++i )
    {
        View = VPLBuffer.vpls[i].View;

        for( int j = 0; j < gl_in.length(); ++j )
        {
            vec4 viewPos = View * gl_in[j].gl_Position;
            vec3 viewDir = viewPos.xyz;
            float len = length(viewDir);
            viewDir = viewDir / len;
            vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
            halfDir.x = -halfDir.x / halfDir.z;
            halfDir.y = -halfDir.y / halfDir.z;
            halfDir.z = (len - LightProjectorNearFar.x) /
                (LightProjectorNearFar.y - LightProjectorNearFar.x);

            gViewPosZ = viewPos.z;
            gDepth = halfDir.z;
            gl_Position.xyz = halfDir;
            gl_Position.w = 1.0;

            gl_Layer = i;
            EmitVertex();
        }

        EndPrimitive();
    }
}