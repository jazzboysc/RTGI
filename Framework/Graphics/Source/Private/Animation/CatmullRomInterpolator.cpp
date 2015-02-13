/* Module      : main
* Author      : Yong Piao
* Email       : ypiao@wpi.edu
* Course      : CS4732
*
* Description : Project 0 executable main.
*
* Date        : 10/30/2014
*
* History:
* Revision      Date          Changed By
* --------      ----------    ----------
* 01.00         10/30/2014		Yong Piao
* First release.
*
* (c) Copyright 2013, Worcester Polytechnic Institute.
*/

/* -- INCLUDE FILES ------------------------------------------------------ */
#include "CatmullRomInterpolator.h"
#include <glm/vec4.hpp>
/* -- DATA STRUCTURES ---------------------------------------------------- */

/* -- GLOBAL VARIABLES --------------------------------------------------- */

/* -- LOCAL VARIABLES ---------------------------------------------------- */

namespace RTGI
{


CatmullRomInterpolator::~CatmullRomInterpolator()
{}

CatmullRomInterpolator::CatmullRomInterpolator(std::vector<glm::vec3> _inPos) : Interpolator(_inPos)
{
	SetTension(0.5f);
}

void CatmullRomInterpolator::SetTension(float _tension)
{
	m_tension = _tension;

#define t m_tension

	m_matrix = glm::mat4(
		0,		1,		0,		0,
		-t,		0,		t,		0,
		2*t,	t-3,	3-2*t, -t,
		-t,		2-t,	t-2,	t);
}

glm::vec3 CatmullRomInterpolator::interpolate(float x, glm::vec3 v0, glm::vec3 v1,	glm::vec3 v2, glm::vec3 v3)
{
#define c1 c[0]
#define c2 c[1]
#define c3 c[2]
#define c4 c[3]

	// Construct matrix with the input points
	glm::mat4 c(
		glm::vec4(v0, 1),
		glm::vec4(v1, 1),
		glm::vec4(v2, 1),
		glm::vec4(v3, 1));
	
	// Calculate constants
	c *= m_matrix;

	// Apply cubic interpolation
	return glm::vec3(c1 + x*(c2 + x*(c3 + x*c4)));
}

glm::mat4 CatmullRomInterpolator::getVal(float _dt)
{
	int nCtrlPts = m_ctrlPts.size();
	float unitTime = 1.0f / nCtrlPts;
	float x = fmod(_dt, unitTime) / unitTime;


	int idxV1 = int(_dt / unitTime);
	glm::vec3 v0, v1, v2, v3;
	if ((idxV1 - 1) < nCtrlPts &&
		(idxV1 - 1) >= 0 )
	{
		v0 = m_ctrlPts[idxV1 - 1];
	}
	if ((idxV1) < nCtrlPts &&
		(idxV1) >= 0)
	{
		v1 = m_ctrlPts[idxV1];
	}
	if ((idxV1 + 1) < nCtrlPts &&
		(idxV1 + 1) >= 0)
	{
		v2 = m_ctrlPts[idxV1 + 1];
	}
	if ((idxV1 + 2) < nCtrlPts &&
		(idxV1 + 2) >= 0)
	{
		v3 = m_ctrlPts[idxV1 + 2];
	}

	return glm::translate(glm::mat4(), interpolate(x, v0, v1, v2, v3));
}

}