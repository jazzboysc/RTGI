#ifndef RTGI_VPLGenerator_H
#define RTGI_VPLGenerator_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class VPLGenerator : public SubRenderer
{
public:
    VPLGenerator(RenderSet* renderSet = 0);
    virtual ~VPLGenerator();
};

}

#endif