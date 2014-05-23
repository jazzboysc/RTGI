//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "PolyCylinder.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PolyCylinder::PolyCylinder(Material* material, Camera* camera)
	:
	RenderObject(material)
	//mCamera(camera)
{
}
//----------------------------------------------------------------------------
PolyCylinder::~PolyCylinder()
{
	Sphere = 0;
	Cylinder = 0;
}
//----------------------------------------------------------------------------
void PolyCylinder::Render()
{
	for( unsigned int i = 0; i < Points.size(); i = i + 2 )
	{
		LPoint& curPoint = Points[i];
		vec4 curPosH(curPoint.Position, 1.0f);
		vec3 curPos;
		curPosH = mWorldTransform * curPosH;
		curPos.x = curPosH.x;
		curPos.y = curPosH.y;
		curPos.z = curPosH.z;

		Sphere->SetWorldTranslation(curPos);
		Sphere->Render();

		LPoint& nextPoint = Points[i + 1];
		vec4 nextPosH(nextPoint.Position, 1.0f);
		vec3 nextPos;
		nextPosH = mWorldTransform * nextPosH;
		nextPos.x = nextPosH.x;
		nextPos.y = nextPosH.y;
		nextPos.z = nextPosH.z;

		vec3 dir = nextPos - curPos;
		dir = normalize(dir);
		vec3 y(0.0f, 1.0f, 0.0f);
		vec3 axis = cross(y, dir);
		float cosTheta = dot(y, dir);
		float theta = acos(cosTheta);
		float angle = theta / PI_SP * 180.0f;
		mat4 rotM = Rotate(angle, axis);

		mat4 tranM = Translate(curPos);
		mat4 m = tranM * rotM;
		Cylinder->SetWorldTransform(m);
		Cylinder->Render();

		Sphere->SetWorldTranslation(nextPos);
		Sphere->Render();
	}
}
//----------------------------------------------------------------------------
void PolyCylinder::SetWorldTransform(const mat4& trans)
{
	mWorldTransform = trans;
}
//----------------------------------------------------------------------------
mat4 PolyCylinder::GetWorldTransform() const
{
	return mWorldTransform;
}
//----------------------------------------------------------------------------
void PolyCylinder::SetWorldTranslation(const vec3& translation)
{
	mWorldTransform[0][3] = translation.x;
	mWorldTransform[1][3] = translation.y;
	mWorldTransform[2][3] = translation.z;
}
//----------------------------------------------------------------------------
vec3 PolyCylinder::GetWorldTranslation() const
{
	vec3 res;
	res.x = mWorldTransform[0][3];
	res.y = mWorldTransform[1][3];
	res.z = mWorldTransform[2][3];
	return res;
}
//----------------------------------------------------------------------------