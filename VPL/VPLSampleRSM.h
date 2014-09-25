#ifndef RTGI_VPLSampleRSM_H
#define RTGI_VPLSampleRSM_H

#include "ComputeTask.h"
#include "Texture1D.h"
#include "Texture2DArray.h"
#include "StructuredBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2014
//----------------------------------------------------------------------------
class VPLSampleRSM : public ComputeTask
{
public:
    VPLSampleRSM();
    ~VPLSampleRSM();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    Texture1DPtr VPLSamplePattern;
    Texture1DPtr VPLSampleTest;
    Texture2DArrayPtr RSMPosition;
    Texture2DArrayPtr RSMNormal;
    Texture2DArrayPtr RSMFlux;
    StructuredBufferPtr VPLBuffer;

private:
    GLint mRSMPositionLoc;
    GLint mRSMNormalLoc;
    GLint mRSMFluxLoc;
};

typedef RefPointer<VPLSampleRSM> VPLSampleRSMPtr;

}

#endif