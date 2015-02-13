#include "Animation.h"

namespace RTGI
{

Animation::~Animation()
{

}

Animation::Animation()
{

}

void Animation::Tick(Camera* _pCamera, float _dt)
{

}

void Animation::Pause()
{
	mPause = true;
}

void Animation::Start()
{
	mPause = false;
	mCurrentTime = 0.0f;
}

void Animation::SetDuration(float _time)
{
	mDuration = _time;
}

void Animation::SetInterpolator(EPositionInterpolator _posInt, ERotationInterpolator _rotInt)
{
	switch (_posInt)
	{
	case RTGI::eBSpline:
		mPositionInterpolator = new BSplineInterpolator();
		break;
	case RTGI::eCatmullRom:
		mPositionInterpolator = new CatmullRomInterpolator();
		break;
	default:
		break;
	}
}

}