#ifndef RTGI_VPLSampleRSM_H
#define RTGI_VPLSampleRSM_H

#include "ComputeTask.h"
#include "Texture1D.h"

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
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    Texture1DPtr VPLSamplePattern;
    Texture1DPtr VPLSampleTest;
};

typedef RefPointer<VPLSampleRSM> VPLSampleRSMPtr;

}

#endif