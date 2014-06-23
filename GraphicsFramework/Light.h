//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Light_H
#define RTGI_Light_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Camera.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/14/2013
//----------------------------------------------------------------------------
class Light : public RefObject
{
public:
	Light();
    virtual ~Light();

	vec3 Color;
    
    void SetProjector(Camera* projector);
    Camera* GetProjector() const;
    void SetLocation(const vec3& location);
    vec3 GetLocation() const;
   
private:
	vec3 mLocation;
    Camera* mProjector;
    
};

typedef RefPointer<Light> LightPtr;

}

#endif
