//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ComputeKernel.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ComputeKernel::ComputeKernel(ComputeProgram* program,
    const std::string& kernelName)
{
    cl_int res;
    cl_program p = program->GetProgram();
    mKernel = clCreateKernel(p, kernelName.c_str(), &res);
    assert( res >= 0 );
}
//----------------------------------------------------------------------------
ComputeKernel::~ComputeKernel()
{
    clReleaseKernel(mKernel);
}
//----------------------------------------------------------------------------