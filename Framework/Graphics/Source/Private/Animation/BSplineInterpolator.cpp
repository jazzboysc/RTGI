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
#include "BSplineInterpolator.h"
#include <glm/vec4.hpp>
/* -- DATA STRUCTURES ---------------------------------------------------- */

/* -- GLOBAL VARIABLES --------------------------------------------------- */

/* -- LOCAL VARIABLES ---------------------------------------------------- */


namespace RTGI
{

BSplineInterpolator::~BSplineInterpolator()
{}

BSplineInterpolator::BSplineInterpolator(std::vector<glm::vec3> _inPos) : Interpolator(_inPos)
{}

glm::vec3 BSplineInterpolator::interpolate(float x, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	float a, b, c, d;
	float t, it;
	t = x;
	it = 1 - t;

	a = it*it*it / 6.0f;
	b = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
	c = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
	d = t*t*t / 6.0f;

	return a * v0 + b * v1 + c * v2 + d * v3;
}

glm::mat4 BSplineInterpolator::getVal(float _dt)
{
	int nCtrlPts = m_ctrlPts.size();
	float unitTime = 1.0f / nCtrlPts;
	float x = fmod(_dt, unitTime) / unitTime;


	int idxV1 = int(_dt / unitTime);
	glm::vec3 v0, v1, v2, v3;
	if ((idxV1 - 1) < nCtrlPts &&
		(idxV1 - 1) >= 0)
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