//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Camera.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Camera::Camera(bool IsPerspective)
	:
	mLocation(0.0f, 0.0f, 0.0f), 
	mRight(1.0f, 0.0f, 0.0f),
	mUp(0.0f, 1.0f, 0.0f), 
	mDirection(0.0f, 0.0f, 1.0f),
	mIsPerspective(IsPerspective)
{
	if( mIsPerspective )
	{
		SetPerspectiveFrustum(45.0f, 1.0f, 0.01f, 100.0f);
	}
	else
	{
		SetOrthogonalFrustum(1.0f, 1.0f, 0.01f, 100.0f);
	}
}
//----------------------------------------------------------------------------
Camera::~Camera()
{
}
//----------------------------------------------------------------------------
void Camera::SetLookAt(const vec3& location, const vec3& lookAt, 
	const vec3& up)
{
	mLocation = location;
	vec3 d = location - lookAt;
	mDirection = normalize(d);
	vec3 r = cross(up, mDirection);
	mRight = normalize(r);
	vec3 u = cross(mDirection, mRight);
	mUp = normalize(u);
}
//----------------------------------------------------------------------------
void Camera::SetLocation(const vec3& location)
{
	mLocation = location;
}
//----------------------------------------------------------------------------
vec3 Camera::GetLocation() const
{
	return mLocation;
}
//----------------------------------------------------------------------------
void Camera::SetAxes(const vec3& right, const vec3& up, const vec3& direction)
{
	mRight = right;
	mUp = up;
	mDirection = direction;
}
//----------------------------------------------------------------------------
void Camera::SetPerspectiveFrustum(float upFovDegrees, float aspectRatio, 
	float nearPlane, float farPlane)
{
	assert( mIsPerspective );

    float halfAngleRadians = 0.5f * upFovDegrees * DEG_TO_RAD_SP;
    mFrustum[VF_UMAX] = nearPlane * tan(halfAngleRadians);
    mFrustum[VF_RMAX] = aspectRatio * mFrustum[VF_UMAX];
    mFrustum[VF_UMIN] = -mFrustum[VF_UMAX];
    mFrustum[VF_RMIN] = -mFrustum[VF_RMAX];
    mFrustum[VF_DMIN] = nearPlane;
    mFrustum[VF_DMAX] = farPlane;

	mUpFovDegrees = upFovDegrees;
	mAspectRatio = aspectRatio;
}
//----------------------------------------------------------------------------
void Camera::SetOrthogonalFrustum(float upMax, float aspectRatio, 
	float nearPlane, float farPlane)
{
	assert( !mIsPerspective );

    mFrustum[VF_UMAX] = upMax;
    mFrustum[VF_RMAX] = aspectRatio * mFrustum[VF_UMAX];
    mFrustum[VF_UMIN] = -mFrustum[VF_UMAX];
    mFrustum[VF_RMIN] = -mFrustum[VF_RMAX];
    mFrustum[VF_DMIN] = nearPlane;
    mFrustum[VF_DMAX] = farPlane;

	mAspectRatio = aspectRatio;
}
//----------------------------------------------------------------------------
void Camera::GetNearFarPlane(float* nearFarPlane) const
{
    assert( nearFarPlane );
    nearFarPlane[0] = mFrustum[VF_DMIN];
    nearFarPlane[1] = mFrustum[VF_DMAX];
}
//----------------------------------------------------------------------------
mat4 Camera::GetViewTransform()
{
	mat4 res;

	float EdR = dot(mLocation, mRight);
	float EdU = dot(mLocation, mUp);
	float EdD = dot(mLocation, mDirection);

	res[0][0] = mRight.x;
	res[0][1] = mRight.y;
	res[0][2] = mRight.z;
	res[0][3] = -EdR;

	res[1][0] = mUp.x;
	res[1][1] = mUp.y;
	res[1][2] = mUp.z;
	res[1][3] = -EdU;

	res[2][0] = mDirection.x;
	res[2][1] = mDirection.y;
	res[2][2] = mDirection.z;
	res[2][3] = -EdD;

	res[3][0] = 0.0f;
	res[3][1] = 0.0f;
	res[3][2] = 0.0f;
	res[3][3] = 1.0f;

	return res;
}
//----------------------------------------------------------------------------
mat4 Camera::GetProjectionTransform()
{
	mat4 res;

	if( mIsPerspective )
	{
		res = Perspective(mUpFovDegrees, mAspectRatio, mFrustum[VF_DMIN], 
			mFrustum[VF_DMAX] );
	}
	else
	{
		res = Ortho(mFrustum[VF_RMIN], mFrustum[VF_RMAX], mFrustum[VF_UMIN],
			mFrustum[VF_UMAX], mFrustum[VF_DMIN], mFrustum[VF_DMAX]);
	}

	return res;
}
//----------------------------------------------------------------------------
bool Camera::IsPerspective() const
{
	return mIsPerspective;
}
//----------------------------------------------------------------------------
vec3 Camera::GetDirection() const
{
	return mDirection;
}
//----------------------------------------------------------------------------