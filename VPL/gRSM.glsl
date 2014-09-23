#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 15) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

uniform mat4 Proj;
uniform vec3 LightPositionWorld;

mat4 GetFaceViewTransform(int face)
{
    const vec3 PX = vec3(1.0, 0.0, 0.0);
    const vec3 NX = vec3(-1.0, 0.0, 0.0);
    const vec3 PY = vec3(0.0, 1.0, 0.0);
    const vec3 NY = vec3(0.0, -1.0, 0.0);
    const vec3 PZ = vec3(0.0, 0.0, 1.0);
    const vec3 NZ = vec3(0.0, 0.0, -1.0);
    const float EdPX = dot(LightPositionWorld, PX);
    const float EdNX = dot(LightPositionWorld, NX);
    const float EdPY = dot(LightPositionWorld, PY);
    const float EdNY = dot(LightPositionWorld, NY);
    const float EdPZ = dot(LightPositionWorld, PZ);
    const float EdNZ = dot(LightPositionWorld, NZ);

    mat4 res;
    if( face == 0 )
    {
        // face PX.
        res = mat4(vec4(PZ, -EdPZ),
                   vec4(PY, -EdPY),
                   vec4(NX, -EdNX),
                   vec4(0.0, 0.0, 0.0, 1.0));
    }
    else if( face == 1 )
    {
        // face NX.
        res = mat4(vec4(NZ, -EdNZ),
                   vec4(PY, -EdPY),
                   vec4(PX, -EdPX),
                   vec4(0.0, 0.0, 0.0, 1.0));
    }
    else if( face == 2 )
    {
        // face PY.
        res = mat4(vec4(PX, -EdPX),
                   vec4(PZ, -EdPZ),
                   vec4(NY, -EdNY),
                   vec4(0.0, 0.0, 0.0, 1.0));
    }
    else if( face == 3 )
    {
        // face NY.
        res = mat4(vec4(PX, -EdPX),
                   vec4(NZ, -EdNZ),
                   vec4(PY, -EdPY),
                   vec4(0.0, 0.0, 0.0, 1.0));
    }
    else if( face == 4 )
    {
        // face NZ.
        res = mat4(vec4(PX, -EdPX),
                   vec4(PY, -EdPY),
                   vec4(PZ, -EdPZ),
                   vec4(0.0, 0.0, 0.0, 1.0));
    }

    return res;
}

void main()
{
    mat4 View, ProjView;
    const int faceCount = 5;

    for( int i = 0; i < faceCount; ++i )
    {
        View = GetFaceViewTransform(i);
        ProjView = Proj * View;

        for( int j = 0; j < gl_in.length(); ++j )
        {
            gPositionWorld = vPositionWorld[j];
            gNormalWorld = vNormalWorld[j];

            gl_Layer = i;
            gl_Position = ProjView * gl_in[j].gl_Position;

            EmitVertex();
        }

        EndPrimitive();
    }
}