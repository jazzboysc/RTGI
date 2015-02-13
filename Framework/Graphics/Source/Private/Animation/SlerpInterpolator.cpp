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
#include "SlerpInterpolator.h"
#include "FQuaternion.h"
#include <glm/vec4.hpp>
/* -- DATA STRUCTURES ---------------------------------------------------- */

/* -- GLOBAL VARIABLES --------------------------------------------------- */

/* -- LOCAL VARIABLES ---------------------------------------------------- */

namespace RTGI
{


SlerpInterpolator::~SlerpInterpolator()
{}

SlerpInterpolator::SlerpInterpolator(std::vector<glm::vec3> _inPos) : Interpolator(_inPos)
{}

glm::mat4 SlerpInterpolator::getVal(float _dt)
{
	int nCtrlPts = m_ctrlPts.size();
	float unitTime = 1.0f / nCtrlPts;
	float x = fmod(_dt, unitTime) / unitTime;


	int idxV1 = int(_dt / unitTime);
	glm::vec3 r0, r1;
	if ((idxV1) < nCtrlPts &&
		(idxV1) >= 0)
	{
		r0 = m_ctrlPts[idxV1];
	}
	if ((idxV1 + 1) < nCtrlPts &&
		(idxV1 + 1) >= 0)
	{
		r1 = m_ctrlPts[idxV1 + 1];
	}
	FQuaternion q0, q1;
	q0.euler(r0);
	q1.euler(r1);

	return FQuaternion::slerp(q0, q1, x).rotationMatrix();
}

}