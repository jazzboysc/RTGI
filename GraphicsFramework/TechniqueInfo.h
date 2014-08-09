//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TechniqueInfo_H
#define RTGI_TechniqueInfo_H

#include "FrameworkCommon.h"
#include "Technique.h"
#include "PassInfo.h"
#include "GeometryAttributes.h"
#include <vector>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/09/2014
//----------------------------------------------------------------------------
class TechniqueInfo
{
public:
	TechniqueInfo();
	~TechniqueInfo();

	void CreatePassInfo(Technique* technique, GeometryAttributes* geometryAttr);

	PassInfo* GetPassInfo(int i) const;

private:
	std::vector<PassInfo*> mPassInfoArray;
};

}

#endif