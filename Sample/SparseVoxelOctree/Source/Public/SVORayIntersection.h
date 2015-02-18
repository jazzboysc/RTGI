#ifndef RTGI_SVORayIntersection_H
#define RTGI_SVORayIntersection_H

#include "ComputeTask.h"
#include "AABB.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/17/2015
//----------------------------------------------------------------------------
class SVORayIntersection : public ComputeTask
{
public:
    SVORayIntersection();
    ~SVORayIntersection();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

private:
    ShaderUniform mRayStartPointLoc;
    ShaderUniform mRayEndPointLoc;
};

typedef RefPointer<SVORayIntersection> SVORayIntersectionPtr;

}

#endif