#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

uniform mat4 Proj;

mat4 GetAxisViewTransform(int axis)
{
    mat4 res;
    return res;
}

void main()
{
    vec3 faceNormalWorld = vNormalWorld[0].xyz + vNormalWorld[1].xyz + vNormalWorld[2].xyz;
    faceNormalWorld = normalize(faceNormalWorld);

    int axis = 0;
    if( faceNormalWorld.y > faceNormalWorld.x )
    {
        axis = 1;
        if( faceNormalWorld.z > faceNormalWorld.y )
        {
            axis = 2;
        }
    }
    else
    {
        if( faceNormalWorld.z > faceNormalWorld.x )
        {
            axis = 2;
        }
    }

    mat4 View = GetAxisViewTransform(axis);

    for( int i = 0; i < gl_in.length(); ++i )
    {
        gPositionWorld = vPositionWorld[i];
        gNormalWorld = vNormalWorld[i];
        gl_Position = Proj * View * gl_in[i].gl_Position;

        EmitVertex();
    }

    EndPrimitive();
}