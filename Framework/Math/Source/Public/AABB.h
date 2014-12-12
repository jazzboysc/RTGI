//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_AABB_H
#define RTGI_AABB_H

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/02/2013
//----------------------------------------------------------------------------
class AABB
{
public:
	AABB();
	~AABB();

	float GetDiagonal();
	glm::vec3 GetBoxCenter();
    glm::vec3 GetExtension();

	void Merge(const AABB& box);

	glm::vec3 Min;
	glm::vec3 Max;
};

}

#endif