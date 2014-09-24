#ifndef RTGI_VPLSampleRSM_H
#define RTGI_VPLSampleRSM_H

#include "ComputeTask.h"

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
    virtual void OnPreRun(unsigned int pass);
    virtual void OnPostRun(unsigned int pass);

private:

};

typedef RefPointer<VPLSampleRSM> VPLSampleRSMPtr;

}

#endif