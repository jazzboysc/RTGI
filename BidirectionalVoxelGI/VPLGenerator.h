#ifndef RTGI_VPLGenerator_H
#define RTGI_VPLGenerator_H

#include "GraphicsFrameworkHeader.h"
#include "RSMRenderer.h"
#include "SampleRSM.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class VPLGenerator : public SubRenderer
{
public:
    VPLGenerator(RenderSet* renderSet = 0);
    virtual ~VPLGenerator();

    void SetRSMRenderer(RSMRenderer* rsm);
    RSMRenderer* GetRSMRenderer() const;

    void Run();
    virtual void OnRender(int technique, int pass, Camera* camera);

    void Initialize();
    StructuredBufferPtr mVPLBuffer;

private:
    RSMRendererPtr mRSM;

    // Sample RSM.
    SampleRSMPtr mSampleRSMTask;

    // VPL stuff.
    enum { VPL_SAMPLE_COUNT = 128 };
    Texture1DPtr mVPLSamplePattern;
    Texture1DPtr mVPLSampleTest;

};

typedef RefPointer<VPLGenerator> VPLGeneratorPtr;

}

#endif