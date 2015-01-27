#ifndef RTGI_BuildSVO_H
#define RTGI_BuildSVO_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVO : public ComputeTask
{
public:
    BuildSVO();
    ~BuildSVO();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVO> BuildSVOPtr;

}

#endif