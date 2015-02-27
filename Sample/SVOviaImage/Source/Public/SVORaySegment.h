#ifndef RTGI_SVORaySegment_H
#define RTGI_SVORaySegment_H

#include "PolylineGeometry.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/17/2015
//----------------------------------------------------------------------------
class SVORaySegment : public PolylineGeometry
{
public:
    SVORaySegment(Material* material, Camera* camera);
    virtual ~SVORaySegment();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);

};

typedef RefPointer<SVORaySegment> SVORaySegmentPtr;

}

#endif