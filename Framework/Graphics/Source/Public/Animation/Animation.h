#pragma once
#include "RefPointer.h"
#include "BSplineInterpolator.h"
#include "CatmullRomInterpolator.h"
#include "SlerpInterpolator.h"


namespace RTGI
{
	struct ControlPoint
	{
		glm::vec3 position;
		glm::vec3 orientation;
	};

	enum EPositionInterpolator
	{
		eBSpline,
		eCatmullRom
	};

	enum ERotationInterpolator
	{
		eSlerp
	};

	class Animation
	{
	public:
		Animation();
		~Animation();

		void SetInterpolator(EPositionInterpolator _posInt, ERotationInterpolator _rotInt);
		void SetDuration(float _time);
		void Start();
		void Pause();
		void Tick(Camera* _pCamera, float _dt);

		std::vector<ControlPoint> ControlPoints;
	private:
		InterpolatorPtr mPositionInterpolator;
		InterpolatorPtr mRotationInterpolator;


		bool mPause;
		float mDuration;
		float mCurrentTime;
	};
	typedef RefPointer<Animation> AnimationPtr;
}