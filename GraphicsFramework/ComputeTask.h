//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeTask_H
#define RTGI_ComputeTask_H

#include "FrameworkCommon.h"
#include "PassManager.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2013
//----------------------------------------------------------------------------
class ComputeTask : public PassManager
{
public:
    ComputeTask();
    virtual ~ComputeTask();

    void Run(unsigned int pass, unsigned int globalX, unsigned int globalY, 
        unsigned int globalZ);

    virtual void OnPreRun(unsigned int pass){};
    virtual void OnPostRun(unsigned int pass){};
};

typedef RefPointer<ComputeTask> ComputeTaskPtr;

}

#endif