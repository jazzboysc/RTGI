#ifndef RTGI_VPLGenerator_H
#define RTGI_VPLGenerator_H

#include "GraphicsFrameworkHeader.h"
#include "RSMRenderer.h"
#include "ComputeTask.h"
#include "Texture1D.h"
#include "Texture2DArray.h"
#include "StructuredBuffer.h"

namespace RTGI
{

#define RTGI_VPLGenerator_VPLBuffer_Name "VPLBuffer"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class SampleRSM : public ComputeTask
{
public:
    SampleRSM();
    ~SampleRSM();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    Texture1DPtr VPLSamplePattern;
    Texture1DPtr VPLSampleTest;

private:
    ShaderUniform mRSMPositionLoc;
    ShaderUniform mRSMNormalLoc;
    ShaderUniform mRSMFluxLoc;
};

typedef RefPointer<SampleRSM> SampleRSMPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class VPLGenerator : public SubRenderer
{
public:
    VPLGenerator(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~VPLGenerator();

    void SetRSMRenderer(RSMRenderer* rsm);
    RSMRenderer* GetRSMRenderer() const;

    void Run();
    virtual void OnRender(int technique, int pass, Camera* camera);

    void Initialize(GPUDevice* device, int vplCount);

private:
    RSMRendererPtr mRSM;

    // Sample RSM.
    SampleRSMPtr mSampleRSMTask;

    // VPL stuff.
    int mVPLCount;
    Texture1DPtr mVPLSamplePattern;
    Texture1DPtr mVPLSampleTest;

};

typedef RefPointer<VPLGenerator> VPLGeneratorPtr;

}

#endif