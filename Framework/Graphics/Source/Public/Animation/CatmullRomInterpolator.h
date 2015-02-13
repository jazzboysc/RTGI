#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Interpolator.h"

namespace RTGI
{

class CatmullRomInterpolator : public Interpolator
{
public:
	CatmullRomInterpolator() {}
	CatmullRomInterpolator(std::vector<glm::vec3> _inPos);
	virtual ~CatmullRomInterpolator();

	void SetTension(float _tension);

	glm::vec3 interpolate(float x, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	virtual glm::mat4 getVal(float _dt);

private:
	float m_tension;
	glm::mat4 m_matrix;
};

}