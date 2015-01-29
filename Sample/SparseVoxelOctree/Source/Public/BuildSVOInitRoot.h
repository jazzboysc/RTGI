#ifndef RTGI_BuildSVOInitRoot_H
#define RTGI_BuildSVOInitRoot_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVOInitRoot : public ComputeTask
{
public:
    BuildSVOInitRoot();
    ~BuildSVOInitRoot();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVOInitRoot> BuildSVOInitRootPtr;

}

#endif