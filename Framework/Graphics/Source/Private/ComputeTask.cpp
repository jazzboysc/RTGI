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

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    GPU_DEVICE_FUNC(program->GetProgramHandle()->Device, 
        ComputeShaderDispatch)(program, globalX, globalY, globalZ);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------