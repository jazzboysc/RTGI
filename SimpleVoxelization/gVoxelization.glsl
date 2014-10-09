#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform mat4 Proj;

const vec3 X = vec3(1.0, 0.0, 0.0);
const vec3 Y = vec3(0.0, 1.0, 0.0);
const vec3 Z = vec3(0.0, 0.0, 1.0);

mat4 GetAxisViewTransform(int axis)
{
    mat4 res;

    vec3 E, R, U, D;

    float offset = 0.5;

    if( axis == 0 )
    {
        E = SceneBBCenter - X*(SceneBBExtension.x + offset);
        R = Z;
        U = Y;
        D = -X;
    }
    else if( axis == 1 )
    {
        E = SceneBBCenter - Y*(SceneBBExtension.y + offset);
        R = X;
        U = Z;
        D = -Y;
    }
    else
    {
        E = SceneBBCenter - Z*(SceneBBExtension.z + offset);
        R = -X;
        U = Y;
        D = -Z;
    }

    float EdR = dot(E, R);
    float EdU = dot(E, U);
    float EdD = dot(E, D);

    res = mat4(vec4(R.x, U.x, D.x, 0.0),
               vec4(R.y, U.y, D.y, 0.0),
               vec4(R.z, U.z, D.z, 0.0),
               vec4(-EdR, -EdU, -EdD, 1.0));

    return res;
}

void main()
{
    vec3 faceNormalWorld = vNormalWorld[0].xyz + vNormalWorld[1].xyz + vNormalWorld[2].xyz;
    faceNormalWorld = normalize(faceNormalWorld);

    float absNdX = abs(dot(faceNormalWorld, X));
    float absNdY = abs(dot(faceNormalWorld, Y));
    float absNdZ = abs(dot(faceNormalWorld, Z));

    int axis = 0;
    if( absNdY > absNdX )
    {
        axis = 1;
        if( absNdZ > absNdY )
        {
            axis = 2;
        }
    }
    else
    {
        if( absNdZ > absNdX )
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