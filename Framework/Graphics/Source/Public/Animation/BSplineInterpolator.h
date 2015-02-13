#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Interpolator.h"

namespace RTGI
{
class BSplineInterpolator : public Interpolator
{
public:
	BSplineInterpolator() {}
	BSplineInterpolator(std::vector<glm::vec3> _inPos);
	virtual ~BSplineInterpolator();

	glm::vec3 interpolate(float x, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	virtual glm::mat4 getVal(float _dt);

private:

};

}