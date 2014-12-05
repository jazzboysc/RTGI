#ifndef RTGI_Merger_H
#define RTGI_Merger_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class Merger : public SubRenderer
{
public:
    Merger(RenderSet* renderSet = 0);
    virtual ~Merger();
};

}

#endif