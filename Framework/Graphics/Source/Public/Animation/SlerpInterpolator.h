#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Interpolator.h"

namespace RTGI
{

class SlerpInterpolator : public Interpolator
{
public:
	SlerpInterpolator(std::vector<glm::vec3> _inRot);
	virtual ~SlerpInterpolator();

	virtual glm::mat4 getVal(float _dt);

private:

};

}