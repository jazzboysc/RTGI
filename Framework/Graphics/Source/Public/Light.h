//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Light_H
#define RTGI_Light_H

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

	glm::vec3 Color;
    
    void SetProjector(Camera* projector);
    Camera* GetProjector() const;
    void SetLocation(const glm::vec3& location);
    glm::vec3 GetLocation() const;
   
private:
	glm::vec3 mLocation;
    Camera* mProjector;
    
};

typedef RefPointer<Light> LightPtr;

}

#endif
