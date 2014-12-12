//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Pass_H
#define RTGI_Pass_H


#include "PassBase.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class Pass : public PassBase
{
public:
    Pass(const ShaderProgramInfo& programInfo);
	Pass(ShaderProgram* shaderProgram);
	~Pass();

	void Enable();
	void Disable();

    bool IsTessellationEnabled() const;
};

typedef RefPointer<Pass> PassPtr;

}

#endif