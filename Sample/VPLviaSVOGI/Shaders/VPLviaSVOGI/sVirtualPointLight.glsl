#define PI 3.141593
#define VPL_INTENSITY 4.0

//----------------------------------------------------------------------------
// Virtual Point Light
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
mat4 GetVPLViewTransform(const vec3 E, const vec3 D)
{
    mat4 res;

    vec3 R, U;
    if( D.x > D.y )
    {
        R = vec3(D.z, 0.0, -D.x);
    }
    else
    {
        R = vec3(0.0, D.z, -D.y);
    }
    R = normalize(R);
    U = cross(D, R);

    float EdR = dot(E, R);
    float EdU = dot(E, U);
    float EdD = dot(E, D);

    res = mat4(vec4(R.x, U.x, D.x, 0.0),
        vec4(R.y, U.y, D.y, 0.0),
        vec4(R.z, U.z, D.z, 0.0),
        vec4(-EdR, -EdU, -EdD, 1.0));

    return res;
}
//----------------------------------------------------------------------------