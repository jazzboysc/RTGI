//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

// Math
#define GLM_FORCE_RADIANS 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/simd_mat4.hpp"
#include "glm/gtx/simd_quat.hpp"
#include "glm/gtx/simd_vec4.hpp"
#include "glm/gtx/norm.hpp"

// C Math
#include <float.h>

// Misc
#include "RNG.h"
#include "AABB.h"


#define PI_SP glm::pi<float>()
//----------------------------------------------------------------------------
#define RTGI_MAX(a,b)((a < b) ? (b) : (a))
//----------------------------------------------------------------------------
#define RTGI_MIN(a,b)((a < b) ? (a) : (b))
//----------------------------------------------------------------------------
inline float Halton(const int base, int index)
{
	float x = 0.0f;
	float f = 1.0f / base;

	while (index)
	{
		x += f * (float)(index % base);
		index /= base;
		f *= 1.0f / base;
	}
	return x;
}
//----------------------------------------------------------------------------
inline double UniformRandom(unsigned int uiSeed = 0)
{
	if (uiSeed > 0)
	{
		srand(uiSeed);
	}

	double ratio = ((double)rand()) / ((double)(RAND_MAX));
	return ratio;
}

//----------------------------------------------------------------------------
inline glm::vec3 UniformSampleSphere(float u1, float u2)
{
	float z = 1.0f - 2.0f * u1;
	float r = sqrtf(RTGI_MAX(0.0f, 1.0f - z*z));
	float phi = 2.0f * PI_SP * u2;
	float x = r * cosf(phi);
	float y = r * sinf(phi);
	return glm::vec3(x, y, z);
}

//----------------------------------------------------------------------------
inline double SymmetricRandom(unsigned int uiSeed = 0)
{
	if (uiSeed > 0)
	{
		srand(uiSeed);
	}

	double ratio = ((double)rand()) / ((double)(RAND_MAX));
	return 2.0*ratio - 1.0;
}