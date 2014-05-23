//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Camera_H
#define RTGI_Camera_H

#include "FrameworkCommon.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/29/2013
//----------------------------------------------------------------------------
class Camera
{
public:
	Camera();
	~Camera();

	// Camera frame
	//
	//       U
	//       |     
	//       |   
	//       |  
	//       |_________R
	//      / E
	//     /
	//    /
	//    D

    // Default location  E = (0,0,0)
    // Default direction R = (1,0,0)
    // Default up        U = (0,1,0)
    // Default right     D = (0,0,1)

	void SetLookAt(const vec3& location, const vec3& lookAt, const vec3& up);
    void SetLocation(const vec3& location);
    void SetAxes(const vec3& right, const vec3& up, const vec3& direction);
	void SetFrustum(float upFovDegrees, float aspectRatio, float nearPlane, 
		float farPlane);

	vec3 GetLocation() const;

	mat4 GetViewTransform();
	mat4 GetProjectionTransform();

    // View frustum.
    enum
    {
        VF_DMIN     = 0,  // near
        VF_DMAX     = 1,  // far
        VF_UMIN     = 2,  // bottom
        VF_UMAX     = 3,  // top
        VF_RMIN     = 4,  // left
        VF_RMAX     = 5,  // right
        VF_COUNT = 6
    };

private:
	vec3 mLocation, mRight, mUp, mDirection;
    float mFrustum[VF_COUNT];  // near, far, bottom, top, left, right
	float mUpFovDegrees, mAspectRatio;

    bool mIsPerspective;
};

}

#endif