//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Camera_H
#define RTGI_Camera_H

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/29/2013
//----------------------------------------------------------------------------
class Camera
{
public:
	Camera(bool IsPerspective = true);
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

	void SetLookAt(const glm::vec3& location, const glm::vec3& lookAt, const glm::vec3& up);
    void SetLocation(const glm::vec3& location);
    void SetAxes(const glm::vec3& right, const glm::vec3& up, const glm::vec3& direction);
	void SetAngle(const float& horizontalAngle, const float& verticalAngle);
	void SetPerspectiveFrustum(float upFovDegrees, float aspectRatio, float nearPlane, 
		float farPlane);
	void SetOrthogonalFrustum(float upMax, float aspectRatio, float nearPlane, 
		float farPlane);

	void Rotate(glm::vec3 _rotation);

	glm::vec3 GetLocation() const;
	glm::vec3 GetDirection() const;
	glm::vec3 GetRight() const;
	void GetAngle(float& _horizontalAngle, float& _verticalAngle);

    void GetNearFarPlane(float* nearFarPlane) const;
	glm::mat4 GetViewTransform();
	glm::mat4 GetProjectionTransform();

	bool IsPerspective() const;

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
	glm::fquat mRot;
	glm::vec3 mLocation, mRight, mUp, mDirection;
    float mFrustum[VF_COUNT];  // near, far, bottom, top, left, right
	float mUpFovDegrees, mAspectRatio;

    bool mIsPerspective;
};

}

#endif