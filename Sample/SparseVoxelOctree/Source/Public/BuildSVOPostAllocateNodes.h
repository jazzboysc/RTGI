#ifndef RTGI_BuildSVOPostAllocateNodes_H
#define RTGI_BuildSVOPostAllocateNodes_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/31/2015
//----------------------------------------------------------------------------
class BuildSVOPostAllocateNodes : public ComputeTask
{
public:
    BuildSVOPostAllocateNodes();
    ~BuildSVOPostAllocateNodes();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVOPostAllocateNodes> BuildSVOPostAllocateNodesPtr;

}

#endif