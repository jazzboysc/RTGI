#ifndef RTGI_VPLGenerator_H
#define RTGI_VPLGenerator_H

#include "GraphicsFrameworkHeader.h"
#include "RSMRenderer.h"

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

    void CreateVPLBuffer();

private:
    RSMRendererPtr mRSM;
};

typedef RefPointer<VPLGenerator> VPLGeneratorPtr;

}

#endif