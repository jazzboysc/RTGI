#ifndef RTGI_BuildSVOAllocateNodes_H
#define RTGI_BuildSVOAllocateNodes_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVOAllocateNodes : public ComputeTask
{
public:
    BuildSVOAllocateNodes();
    ~BuildSVOAllocateNodes();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVOAllocateNodes> BuildSVOAllocateNodesPtr;

}

#endif