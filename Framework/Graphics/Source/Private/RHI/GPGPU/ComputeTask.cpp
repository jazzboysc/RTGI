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
    assert(p);
    assert(p->IsVertexPass() == false);

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
    StructuredBuffer* indirectCommandBuffer, void* indirect)
{
    // TODO:
    assert(false);
}
//----------------------------------------------------------------------------
void ComputeTask::DispatchVertex(unsigned int pass, unsigned int threadCount)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    assert(p);
    assert(p->IsVertexPass() == true);

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    program->GetProgramHandle()->Device->DispatchVertex(threadCount);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------
void ComputeTask::DispatchVertexIndirect(unsigned int pass,
    StructuredBuffer* indirectCommandBuffer, void* indirect)
{
    ComputePass* p = (ComputePass*)GetPass(pass);
    assert(p);
    assert(p->IsVertexPass() == true);
    assert(indirectCommandBuffer);

    ShaderProgram* program = p->GetShaderProgram();
    program->Enable();
    OnPreDispatch(pass);

    indirectCommandBuffer->BindToIndirect();
    program->GetProgramHandle()->Device->DispatchVertexIndirect(indirect);

    OnPostDispatch(pass);
    program->Disable();
}
//----------------------------------------------------------------------------