//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeTask_H
#define RTGI_ComputeTask_H

#include "FrameworkCommon.h"
#include "PassManager.h"
#include "ComputePass.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2013
//----------------------------------------------------------------------------
class ComputeTask : public PassManager
{
public:
    ComputeTask();
    virtual ~ComputeTask();

    void CreateDeviceResource(GPUDevice* device);

    void Dispatch(unsigned int pass, unsigned int globalX, 
        unsigned int globalY, unsigned int globalZ);

    virtual void OnGetShaderConstants(){};
    virtual void OnPreDispatch(unsigned int pass){};
    virtual void OnPostDispatch(unsigned int pass){};
};

typedef RefPointer<ComputeTask> ComputeTaskPtr;

}

#endif