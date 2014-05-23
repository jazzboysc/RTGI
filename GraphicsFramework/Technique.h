//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Technique_H
#define RTGI_Technique_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Pass.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class Technique : public RefObject
{
public:
	Technique();
	~Technique();

	void AddPass(Pass* pass);
	unsigned int GetPassCount() const;
	Pass* GetPass(unsigned int i) const;

	void CreateDeviceResource();

protected:
	std::vector<Pass*> mPasses;
};

typedef RefPointer<Technique> TechniquePtr;

}

#endif