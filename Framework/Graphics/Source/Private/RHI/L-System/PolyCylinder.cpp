//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

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
void PolyCylinder::Render(int technique, int pass)
{
	for( unsigned int i = 0; i < Points.size(); i = i + 2 )
	{
		LPoint& curPoint = Points[i];
		glm::vec4 curPosH(curPoint.Position, 1.0f);
		glm::vec3 curPos;
		curPosH = mWorldTransform * curPosH;
		curPos.x = curPosH.x;
		curPos.y = curPosH.y;
		curPos.z = curPosH.z;

		Sphere->SetWorldTranslation(curPos);
		Sphere->Render(technique, pass);

		LPoint& nextPoint = Points[i + 1];
		glm::vec4 nextPosH(nextPoint.Position, 1.0f);
		glm::vec3 nextPos;
		nextPosH = mWorldTransform * nextPosH;
		nextPos.x = nextPosH.x;
		nextPos.y = nextPosH.y;
		nextPos.z = nextPosH.z;

		glm::vec3 dir = nextPos - curPos;
		dir = glm::normalize(dir);
		glm::vec3 y(0.0f, 1.0f, 0.0f);
		glm::vec3 axis = glm::cross(y, dir);
		float cosTheta = glm::dot(y, dir);
		float theta = acos(cosTheta);
		float angle = theta / PI_SP * 180.0f;
		glm::mat4 rotM = glm::rotate(glm::mat4(), glm::radians(angle), axis);

		glm::mat4 tranM = glm::translate(glm::mat4(), curPos);
		glm::mat4 m = tranM * rotM;
		Cylinder->SetWorldTransform(m);
		Cylinder->Render(technique, pass);

		Sphere->SetWorldTranslation(nextPos);
		Sphere->Render(technique, pass);
	}
}
//----------------------------------------------------------------------------
void PolyCylinder::SetWorldTransform(const glm::mat4& trans)
{
	mWorldTransform = trans;
}
//----------------------------------------------------------------------------
glm::mat4 PolyCylinder::GetWorldTransform() const
{
	return mWorldTransform;
}
//----------------------------------------------------------------------------
void PolyCylinder::SetWorldTranslation(const glm::vec3& translation)
{
	mWorldTransform[0][3] = translation.x;
	mWorldTransform[1][3] = translation.y;
	mWorldTransform[2][3] = translation.z;
}
//----------------------------------------------------------------------------
glm::vec3 PolyCylinder::GetWorldTranslation() const
{
	glm::vec3 res;
	res.x = mWorldTransform[0][3];
	res.y = mWorldTransform[1][3];
	res.z = mWorldTransform[2][3];
	return res;
}
//----------------------------------------------------------------------------