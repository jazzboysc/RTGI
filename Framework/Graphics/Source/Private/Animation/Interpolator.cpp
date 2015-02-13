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
#include "Interpolator.h"
/* -- DATA STRUCTURES ---------------------------------------------------- */

/* -- GLOBAL VARIABLES --------------------------------------------------- */

/* -- LOCAL VARIABLES ---------------------------------------------------- */




namespace RTGI
{


Interpolator::~Interpolator()
{

}

Interpolator::Interpolator()
{

}

Interpolator::Interpolator(std::vector<glm::vec3> _inPts)
{
	for (auto i : _inPts)
	{
		PushCtrlPt(i);
	}
}

glm::mat4 Interpolator::getVal(float _dt)
{
	return glm::mat4();
}

void Interpolator::PushCtrlPt(glm::vec3 _inVec3)
{
	m_ctrlPts.push_back(_inVec3);
}

}