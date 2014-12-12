//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "AABB.h"

using namespace RTGI;

//----------------------------------------------------------------------------
AABB::AABB()
{
	Min.x = FLT_MAX;
	Min.y = FLT_MAX;
	Min.z = FLT_MAX;
	Max.x = -FLT_MAX;
	Max.y = -FLT_MAX;
	Max.z = -FLT_MAX;
}
//----------------------------------------------------------------------------
AABB::~AABB()
{
}
//----------------------------------------------------------------------------
float AABB::GetDiagonal()
{
	float diag = glm::distance(Min, Max);
	return diag;
}
//----------------------------------------------------------------------------
glm::vec3 AABB::GetBoxCenter()
{
	return 0.5f*(Min + Max);
}
//----------------------------------------------------------------------------
glm::vec3 AABB::GetExtension()
{
    return 0.5f*(Max - Min);
}
//----------------------------------------------------------------------------
void AABB::Merge(const AABB& box)
{
	Min.x = RTGI_MIN(Min.x, box.Min.x);
	Min.y = RTGI_MIN(Min.y, box.Min.y);
	Min.z = RTGI_MIN(Min.z, box.Min.z);
	Max.x = RTGI_MAX(Max.x, box.Max.x);
	Max.y = RTGI_MAX(Max.y, box.Max.y);
	Max.z = RTGI_MAX(Max.z, box.Max.z);
}
//----------------------------------------------------------------------------