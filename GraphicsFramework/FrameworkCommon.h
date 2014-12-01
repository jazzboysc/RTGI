//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_FrameworkCommon_H
#define RTGI_FrameworkCommon_H

#include "Angel.h"
#include <float.h>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

#ifdef __APPLE__
#ifndef glGenVertexArrays
#define glGenVertexArrays    glGenVertexArraysAPPLE
#endif
#ifndef glBindVertexArray
#define glBindVertexArray    glBindVertexArrayAPPLE
#endif
#ifndef glDeleteVertexArrays
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
#endif

#if defined(_WIN32)
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif

#define RTGI_OUTPUT_RESOURCE_LOADING

#define PI_SP 3.141592f
#define DEG_TO_RAD_SP PI_SP/180.0f

//----------------------------------------------------------------------------
#define RTGI_MAX(a,b)((a < b) ? (b) : (a))
//----------------------------------------------------------------------------
#define RTGI_MIN(a,b)((a < b) ? (a) : (b))
//----------------------------------------------------------------------------
inline float Dist3(vec3& v1, vec3& v2)
{
	float res = 0.0f;
	vec3 d = v1 - v2;
	res = length(d);
	return res;
}
//----------------------------------------------------------------------------
inline float DistSquare2(float x1, float y1, float x2, float y2)
{
	float dy = y2 - y1;
	float dx = x2 - x1;
	return dy*dy + dx*dx;
}
//----------------------------------------------------------------------------
inline bool GetInverseMatrix(mat4& srcM, mat4& dstM)
{
    float fA0 = srcM[0][0]*srcM[1][1] - srcM[0][1]*srcM[1][0];
    float fA1 = srcM[0][0]*srcM[1][2] - srcM[0][2]*srcM[1][0];
    float fA2 = srcM[0][0]*srcM[1][3] - srcM[0][3]*srcM[1][0];
    float fA3 = srcM[0][1]*srcM[1][2] - srcM[0][2]*srcM[1][1];
    float fA4 = srcM[0][1]*srcM[1][3] - srcM[0][3]*srcM[1][1];
    float fA5 = srcM[0][2]*srcM[1][3] - srcM[0][3]*srcM[1][2];
    float fB0 = srcM[2][0]*srcM[3][1] - srcM[2][1]*srcM[3][0];
    float fB1 = srcM[2][0]*srcM[3][2] - srcM[2][2]*srcM[3][0];
    float fB2 = srcM[2][0]*srcM[3][3] - srcM[2][3]*srcM[3][0];
    float fB3 = srcM[2][1]*srcM[3][2] - srcM[2][2]*srcM[3][1];
    float fB4 = srcM[2][1]*srcM[3][3] - srcM[2][3]*srcM[3][1];
    float fB5 = srcM[2][2]*srcM[3][3] - srcM[2][3]*srcM[3][2];

    float fDet = fA0*fB5 - fA1*fB4 + fA2*fB3 + fA3*fB2 - fA4*fB1 + fA5*fB0;
    if( abs(fDet) <= 1e-06f )
    {
        return false;
	}

	dstM[0][0] = + srcM[1][1]*fB5 - srcM[1][2]*fB4 + srcM[1][3]*fB3;
	dstM[1][0] = - srcM[1][0]*fB5 + srcM[1][2]*fB2 - srcM[1][3]*fB1;
	dstM[2][0] = + srcM[1][0]*fB4 - srcM[1][1]*fB2 + srcM[1][3]*fB0;
	dstM[3][0] = - srcM[1][0]*fB3 + srcM[1][1]*fB1 - srcM[1][2]*fB0;
	dstM[0][1] = - srcM[0][1]*fB5 + srcM[0][2]*fB4 - srcM[0][3]*fB3;
	dstM[1][1] = + srcM[0][0]*fB5 - srcM[0][2]*fB2 + srcM[0][3]*fB1;
	dstM[2][1] = - srcM[0][0]*fB4 + srcM[0][1]*fB2 - srcM[0][3]*fB0;
	dstM[3][1] = + srcM[0][0]*fB3 - srcM[0][1]*fB1 + srcM[0][2]*fB0;
	dstM[0][2] = + srcM[3][1]*fA5 - srcM[3][2]*fA4 + srcM[3][3]*fA3;
	dstM[1][2] = - srcM[3][0]*fA5 + srcM[3][2]*fA2 - srcM[3][3]*fA1;
	dstM[2][2] = + srcM[3][0]*fA4 - srcM[3][1]*fA2 + srcM[3][3]*fA0;
	dstM[3][2] = - srcM[3][0]*fA3 + srcM[3][1]*fA1 - srcM[3][2]*fA0;
    dstM[0][3] = - srcM[2][1]*fA5 + srcM[2][2]*fA4 - srcM[2][3]*fA3;
	dstM[1][3] = + srcM[2][0]*fA5 - srcM[2][2]*fA2 + srcM[2][3]*fA1;
	dstM[2][3] = - srcM[2][0]*fA4 + srcM[2][1]*fA2 - srcM[2][3]*fA0;
	dstM[3][3] = + srcM[2][0]*fA3 - srcM[2][1]*fA1 + srcM[2][2]*fA0;


    float fInvDet = 1.0f / fDet;

    dstM[0][0] *= fInvDet;
    dstM[0][1] *= fInvDet;
    dstM[0][2] *= fInvDet;
    dstM[0][3] *= fInvDet;

    dstM[1][0] *= fInvDet;
    dstM[1][1] *= fInvDet;
    dstM[1][2] *= fInvDet;
    dstM[1][3] *= fInvDet;

    dstM[2][0] *= fInvDet;
    dstM[2][1] *= fInvDet;
    dstM[2][2] *= fInvDet;
    dstM[2][3] *= fInvDet;

    dstM[3][0] *= fInvDet;
    dstM[3][1] *= fInvDet;
    dstM[3][2] *= fInvDet;
    dstM[3][3] *= fInvDet;

	return true;
}
//----------------------------------------------------------------------------
inline void GetOrthogonalBasis(const vec3& d, vec3& r, vec3& u)
{
    if( d.x > d.y )
    {
        r = vec3(d.z, 0.0, -d.x);
    }
    else
    {
        r = vec3(0.0, d.z, -d.y);
    }
    r = normalize(r);
    u = cross(d, r);
}
//----------------------------------------------------------------------------
// Based on PBRT Rotate function.
// See http://pbrt.org/
//----------------------------------------------------------------------------
inline mat4 Rotate(float angle, const vec3& axis)
{
    mat4 res;

	if( angle == 0.0f || length(axis) == 0.0f )
	{
		return res;
	}

    vec3 a = normalize(axis);
	float theta = DegreesToRadians * angle;
    float s = sinf(theta);
    float c = cosf(theta);

    res[0][0] = a.x * a.x + (1.f - a.x * a.x) * c;
    res[0][1] = a.x * a.y * (1.f - c) - a.z * s;
    res[0][2] = a.x * a.z * (1.f - c) + a.y * s;
    res[0][3] = 0.0f;

    res[1][0] = a.x * a.y * (1.f - c) + a.z * s;
    res[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
    res[1][2] = a.y * a.z * (1.f - c) - a.x * s;
    res[1][3] = 0.0f;

    res[2][0] = a.x * a.z * (1.f - c) - a.y * s;
    res[2][1] = a.y * a.z * (1.f - c) + a.x * s;
    res[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
    res[2][3] = 0.0f;

    res[3][0] = 0.0f;
    res[3][1] = 0.0f;
    res[3][2] = 0.0f;
    res[3][3] = 1.0f;

    return res;
}
//----------------------------------------------------------------------------
inline double UniformRandom(unsigned int uiSeed = 0)
{
    if( uiSeed > 0 )
	{
        srand(uiSeed);
	}

    double ratio = ((double)rand())/((double)(RAND_MAX));
    return ratio;
}
//----------------------------------------------------------------------------
inline double SymmetricRandom(unsigned int uiSeed = 0)
{
    if( uiSeed > 0 )
	{
        srand(uiSeed);
	}

    double ratio = ((double)rand())/((double)(RAND_MAX));
    return 2.0*ratio - 1.0;
}
//----------------------------------------------------------------------------
inline float Halton(const int base, int index)
{
	float x = 0.0f;
	float f = 1.0f / base;

	while( index )
	{
		x += f * (float) (index % base);
		index /= base;
		f *= 1.0f / base;
	}
	return x;
}
//----------------------------------------------------------------------------
inline vec3 UniformSampleSphere(float u1, float u2)
{
    float z = 1.0f - 2.0f * u1;
    float r = sqrtf(RTGI_MAX(0.0f, 1.0f - z*z));
    float phi = 2.0f * PI_SP * u2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    return vec3(x, y, z);
}
//----------------------------------------------------------------------------
inline int IntLog2(unsigned int x)
{
    assert(x > 0);

    int y = -1;
    while( x > 0 )
    {
        x = x >> 1;
        y++;
    }

    return y;
}
//----------------------------------------------------------------------------

#endif