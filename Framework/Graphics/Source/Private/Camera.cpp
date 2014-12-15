//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

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
void Camera::SetLookAt(const glm::vec3& location, const glm::vec3& lookAt, 
	const glm::vec3& up)
{
	mLocation = location;
	glm::vec3 d = location - lookAt;
	mDirection = glm::normalize(d);
	glm::vec3 r = glm::cross(up, mDirection);
	mRight = glm::normalize(r);
	glm::vec3 u = glm::cross(mDirection, mRight);
	mUp = glm::normalize(u);
}
//----------------------------------------------------------------------------
void Camera::SetLocation(const glm::vec3& location)
{
	mLocation = location;
}
//----------------------------------------------------------------------------
glm::vec3 Camera::GetLocation() const
{
	return mLocation;
}
//----------------------------------------------------------------------------
void Camera::SetAxes(const glm::vec3& right, const glm::vec3& up, const glm::vec3& direction)
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

    float halfAngleRadians = glm::radians(0.5f * upFovDegrees);
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
glm::mat4 Camera::GetViewTransform()
{
	glm::mat4 res;

	float EdR = glm::dot(mLocation, mRight);
    float EdU = glm::dot(mLocation, mUp);
    float EdD = glm::dot(mLocation, mDirection);

	res[0][0] = mRight.x;
	res[1][0] = mRight.y;
	res[2][0] = mRight.z;
	res[3][0] = -EdR;

	res[0][1] = mUp.x;
	res[1][1] = mUp.y;
	res[2][1] = mUp.z;
	res[3][1] = -EdU;

	res[0][2] = mDirection.x;
	res[1][2] = mDirection.y;
	res[2][2] = mDirection.z;
	res[3][2] = -EdD;

	res[0][3] = 0.0f;
	res[1][3] = 0.0f;
	res[2][3] = 0.0f;
	res[3][3] = 1.0f;

	return res;
}
//----------------------------------------------------------------------------
glm::mat4 Camera::GetProjectionTransform()
{
	glm::mat4 res;

	if( mIsPerspective )
	{
        res = glm::perspective(glm::radians(mUpFovDegrees), mAspectRatio, mFrustum[VF_DMIN],
			mFrustum[VF_DMAX] );
	}
	else
	{
        res = glm::ortho(mFrustum[VF_RMIN], mFrustum[VF_RMAX], mFrustum[VF_UMIN],
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
glm::vec3 Camera::GetDirection() const
{
	return mDirection;
}
//----------------------------------------------------------------------------
glm::vec3 Camera::GetRight() const
{
	return mRight;
}
//----------------------------------------------------------------------------
void Camera::SetAngle(const float& horizontalAngle, const float& verticalAngle)
{
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	glm::vec3 rightDir = glm::vec3(
		sin(horizontalAngle - glm::pi<float>() * 0.5f),
		0,
		cos(horizontalAngle - glm::pi<float>() * 0.5f)
		);

	// Up vector
	glm::vec3 up = glm::cross(rightDir, direction);

	mRight = glm::normalize(-rightDir);
	mUp = glm::normalize(up);
	mDirection = glm::normalize(direction);
}
//----------------------------------------------------------------------------
void Camera::GetAngle(float& _horizontalAngle, float& _verticalAngle) const
{
	float v = asin(mDirection.y);
	float h = asin(mDirection.x / cos(v));

	_horizontalAngle = h;
	_verticalAngle = v;
}
