//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputingApplication_H
#define RTGI_ComputingApplication_H

#include "ComputingFrameworkCommon.h"
#include "Application.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/26/2013
//----------------------------------------------------------------------------
class ComputingApplication : public Application
{
public:
	ComputingApplication();
	virtual ~ComputingApplication();

    virtual void InitializeOpenCL();
    virtual void TerminateOpenCL();
    
protected:    
    cl_platform_id mOpenCLPlatform;
    cl_device_id mOpenCLDevice;
    cl_context mOpenCLContext;
};

}

#endif