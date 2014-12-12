//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputePass_H
#define RTGI_ComputePass_H


#include "PassBase.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2013
//----------------------------------------------------------------------------
class ComputePass : public PassBase
{
public:
    ComputePass(const ShaderProgramInfo& programInfo);
    ComputePass(ShaderProgram* computeProgram);
    ~ComputePass();
};

typedef RefPointer<ComputePass> ComputePassPtr;

}

#endif