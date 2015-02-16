//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PassBase_H
#define RTGI_PassBase_H

#include "RefObject.h"
#include "ShaderProgram.h"
#include "PipelineStateBlock.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/09/2014
//----------------------------------------------------------------------------
class PassBase : public RefObject
{
public:
    PassBase(const ShaderProgramInfo& programInfo);
    PassBase(ShaderProgram* shaderProgram);
    virtual ~PassBase();

    ShaderProgram* GetShaderProgram();
    void CreateDeviceResource(GPUDevice* device);

    void SetPipelineStateBlock(PipelineStateBlock* psb);
    PipelineStateBlock* GetPipelineStateBlock() const;

protected:
    ShaderProgramPtr mShaderProgram;
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<PassBase> PassBasePtr;

}

#endif