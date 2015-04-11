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
void ComputeTask::DispatchCompute(unsigned int pass, unsigned int globalX,
    unsigned int globalY, unsigned int globalZ)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    RTGI_ASSERT(p);
    RTGI_ASSERT(p->IsVertexPass() == false);

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    program->GetProgramHandle()->Device->ComputeShaderDispatch(
		program, globalX, globalY, globalZ);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------
void ComputeTask::DispatchComputeIndirect(unsigned int pass,
    Buffer* indirectCommandBuffer, BufferView* indirectCommandBufferView, 
    void* indirect)
{
    // TODO:
    RTGI_ASSERT(false);
}
//----------------------------------------------------------------------------
void ComputeTask::DispatchVertex(unsigned int pass, unsigned int threadCount)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    RTGI_ASSERT(p);
    RTGI_ASSERT(p->IsVertexPass() == true);

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    program->GetProgramHandle()->Device->DispatchVertex(threadCount);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------
void ComputeTask::DispatchVertexIndirect(unsigned int pass,
    Buffer* indirectCommandBuffer, BufferView* indirectCommandBufferView, 
    void* indirect)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    RTGI_ASSERT(p);
    RTGI_ASSERT(p->IsVertexPass() == true);
    RTGI_ASSERT(indirectCommandBuffer && indirectCommandBufferView);

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    indirectCommandBuffer->BindTo(indirectCommandBufferView);
    program->GetProgramHandle()->Device->DispatchVertexIndirect(indirect);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------