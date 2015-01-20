//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Technique_H
#define RTGI_Technique_H

#include "PassManager.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class Technique : public PassManager
{
public:
	Technique();
	Technique(Pass* _pPass);
	~Technique();
};

typedef RefPointer<Technique> TechniquePtr;

}

#endif