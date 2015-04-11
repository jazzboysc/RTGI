//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeTask_H
#define RTGI_ComputeTask_H

#include "PassManager.h"
#include "ComputePass.h"
#include "Buffer.h"

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

    void DispatchCompute(unsigned int pass, unsigned int globalX, 
        unsigned int globalY, unsigned int globalZ);

    void DispatchComputeIndirect(unsigned int pass, 
        Buffer* indirectCommandBuffer, BufferView* indirectCommandBufferView, 
        void* indirect);

    void DispatchVertex(unsigned int pass, unsigned int threadCount);

    void DispatchVertexIndirect(unsigned int pass, 
        Buffer* indirectCommandBuffer, BufferView* indirectCommandBufferView, 
        void* indirect);

    virtual void OnGetShaderConstants(){};
    virtual void OnPreDispatch(unsigned int pass){};
    virtual void OnPostDispatch(unsigned int pass){};
};

typedef RefPointer<ComputeTask> ComputeTaskPtr;

}

#endif