#ifndef RTGI_BuildSVOInitNodes_H
#define RTGI_BuildSVOInitNodes_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVOInitNodes : public ComputeTask
{
public:
    BuildSVOInitNodes();
    ~BuildSVOInitNodes();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVOInitNodes> BuildSVOInitNodesPtr;

}

#endif