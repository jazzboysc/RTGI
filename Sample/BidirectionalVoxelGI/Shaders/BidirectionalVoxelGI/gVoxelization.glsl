#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

const vec3 X = vec3(1.0, 0.0, 0.0);
const vec3 Y = vec3(0.0, 1.0, 0.0);
const vec3 Z = vec3(0.0, 0.0, 1.0);

mat4 GetAxisViewProjTransforms(int axis, vec3 bbCenter, vec3 bbExtension)
{
    mat4 view, proj;
    vec3 E, R, U, D;
    float offset = 0.5;
    float left, right, bottom, top, near, far;

    if( axis == 0 )
    {
        E = bbCenter - X*(bbExtension.x + offset);
        R = Z;
        U = Y;
        D = -X;

        left = -bbExtension.z;
        right = bbExtension.z;
        bottom = -bbExtension.y;
        top = bbExtension.y;
        near = 0.01;
        far = 2.0*bbExtension.x + offset;
    }
    else if( axis == 1 )
    {
        E = bbCenter - Y*(bbExtension.y + offset);
        R = X;
        U = Z;
        D = -Y;

        left = -bbExtension.x;
        right = bbExtension.x;
        bottom = -bbExtension.z;
        top = bbExtension.z;
        near = 0.01;
        far = 2.0*bbExtension.y + offset;
    }
    else
    {
        E = bbCenter - Z*(bbExtension.z + offset);
        R = -X;
        U = Y;
        D = -Z;

        left = -bbExtension.x;
        right = bbExtension.x;
        bottom = -bbExtension.y;
        top = bbExtension.y;
        near = 0.01;
        far = 2.0*bbExtension.z + offset;
    }

    mat3 matRUD = mat3(R, U, D);
    vec3 EdRUD = E*matRUD;
    view = mat4(vec4(R.x, U.x, D.x, 0.0),
                vec4(R.y, U.y, D.y, 0.0),
                vec4(R.z, U.z, D.z, 0.0),
                vec4(-EdRUD.x, -EdRUD.y, -EdRUD.z, 1.0));

    float invRmL = 1.0 / (right - left);
    float invTmB = 1.0 / (top - bottom);
    float invNmF = 1.0 / (near - far);
    float m00 = 2.0 * invRmL;
    float m11 = 2.0 * invTmB;
    float m22 = 2.0 * invNmF;
    float m03 = -(right + left) * invRmL;
    float m13 = -(top + bottom) * invTmB;
    float m23 = -(far + near) * -invNmF;

    proj = mat4(vec4(m00, 0.0, 0.0, 0.0),
                vec4(0.0, m11, 0.0, 0.0),
                vec4(0.0, 0.0, m22, 0.0),
                vec4(m03, m13, m23, 1.0));

    return proj * view;
}

struct AABB
{
    vec3 Min;
    vec3 Max;
};

AABB GetTriangleAABB(vec3 p0, vec3 p1, vec3 p2)
{
    AABB res;

    res.Min.x = min(min(p0.x, p1.x), p2.x);
    res.Min.y = min(min(p0.y, p1.y), p2.y);
    res.Min.z = min(min(p0.z, p1.z), p2.z);
    res.Max.x = max(max(p0.x, p1.x), p2.x);
    res.Max.y = max(max(p0.y, p1.y), p2.y);
    res.Max.z = max(max(p0.z, p1.z), p2.z);

    return res;
}

vec3 GetAABBCenter(AABB bb)
{
    vec3 res;
    res = 0.5 * (bb.Max + bb.Min);
    return res;
}

vec3 GetAABBExtension(AABB bb)
{
    vec3 res;
    res = 0.5 * (bb.Max - bb.Min);
    return res;
}

void main()
{
    AABB bb = GetTriangleAABB(vPositionWorld[0].xyz, vPositionWorld[1].xyz, vPositionWorld[2].xyz);
    vec3 bbCenter = GetAABBCenter(bb);
    vec3 bbExtension = GetAABBExtension(bb);

    vec3 faceNormalWorld = vNormalWorld[0].xyz + vNormalWorld[1].xyz + vNormalWorld[2].xyz;
    faceNormalWorld = normalize(faceNormalWorld);

    mat3 matXYZ = mat3(X, Y, Z);
    vec3 NdXYZ = faceNormalWorld * matXYZ;

    float absNdX = abs(NdXYZ.x);
    float absNdY = abs(NdXYZ.y);
    float absNdZ = abs(NdXYZ.z);

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

    mat4 ProjView = GetAxisViewProjTransforms(axis, bbCenter, bbExtension);
    for( int i = 0; i < gl_in.length(); ++i )
    {
        gPositionWorld = vPositionWorld[i];
        gNormalWorld = vNormalWorld[i];
        gl_Position = ProjView * gl_in[i].gl_Position;

        EmitVertex();
    }

    EndPrimitive();
}