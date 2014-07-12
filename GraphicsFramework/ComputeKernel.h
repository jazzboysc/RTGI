//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeKernel_H
#define RTGI_ComputeKernel_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "ComputeProgram.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class ComputeKernel : public RefObject
{
public:
	ComputeKernel(ComputeProgram* program, const std::string& kernelName);
	~ComputeKernel();

private:
    cl_kernel mKernel;

};

typedef RefPointer<ComputeKernel> ComputeKernelPtr;

}

#endif
