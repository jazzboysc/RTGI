#ifndef RTGI_BuildSVOFlagNodes_H
#define RTGI_BuildSVOFlagNodes_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVOFlagNodes : public ComputeTask
{
public:
    BuildSVOFlagNodes();
    ~BuildSVOFlagNodes();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVOFlagNodes> BuildSVOFlagNodesPtr;

}

#endif