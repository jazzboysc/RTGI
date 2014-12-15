//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "ComputeTask.h"
#include "ComputePass.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ComputeTask::ComputeTask()
{
}
//----------------------------------------------------------------------------
ComputeTask::~ComputeTask()
{
}
//----------------------------------------------------------------------------
void ComputeTask::CreateDeviceResource(GPUDevice* device)
{
    PassManager::CreateDeviceResource(device);
    OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void ComputeTask::Dispatch(unsigned int pass, unsigned int globalX,
    unsigned int globalY, unsigned int globalZ)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    assert(p);

    p->GetShaderProgram()->Enable();
    OnPreDispatch(pass);

    glDispatchCompute(globalX, globalY, globalZ);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    OnPostDispatch(pass);
    p->GetShaderProgram()->Disable();
}
//----------------------------------------------------------------------------