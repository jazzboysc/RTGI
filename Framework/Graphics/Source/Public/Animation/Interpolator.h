#pragma once
#include "RefPointer.h"
#include <vector>
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace RTGI
{

class Interpolator : public RefObject
{
public:
	Interpolator();
	Interpolator(std::vector<glm::vec3> _inRot);
	virtual ~Interpolator();

	virtual glm::mat4 getVal(float _dt);

	void PushCtrlPt(glm::vec3 _inVec3);

protected:
	std::vector<glm::vec3> m_ctrlPts;

};

typedef RefPointer<Interpolator> InterpolatorPtr;
}