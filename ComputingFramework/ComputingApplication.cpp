//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "ComputingFrameworkPCH.h"
#include "ComputingApplication.h"
#include <sys/timeb.h>

using namespace RTGI;

//----------------------------------------------------------------------------
ComputingApplication::ComputingApplication()
{
}
//----------------------------------------------------------------------------
ComputingApplication::~ComputingApplication()
{
}
//----------------------------------------------------------------------------
void ComputingApplication::InitializeOpenCL()
{
    cl_int res = clGetPlatformIDs(1, &mOpenCLPlatform, NULL);
    assert( res >= 0 );
    
    res = clGetDeviceIDs(mOpenCLPlatform, CL_DEVICE_TYPE_GPU, 1,
                         &mOpenCLDevice, NULL);
    assert( res >= 0 );
    
#ifdef _WIN32
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)mOpenGLContext,
		CL_WGL_HDC_KHR, (cl_context_properties)mWindowsDeviceContext,
		CL_CONTEXT_PLATFORM, (cl_context_properties)mOpenCLPlatform,
		0};
#else
#ifdef __APPLE__
	cl_context_properties properties[] = {
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
		(cl_context_properties)mShareGroup,
		0};
#endif
#endif
    
    mOpenCLContext = clCreateContext(properties, 1, &mOpenCLDevice, NULL,
                                     NULL, &res);
    assert( res >= 0 );
}
//----------------------------------------------------------------------------
void ComputingApplication::TerminateOpenCL()
{
    clReleaseContext(mOpenCLContext);
    clReleaseDevice(mOpenCLDevice);
}
//----------------------------------------------------------------------------