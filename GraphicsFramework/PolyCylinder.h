//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PolyCylinder_H
#define RTGI_PolyCylinder_H

#include "FrameworkCommon.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Expression.h"
#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/19/2013
//----------------------------------------------------------------------------
class PolyCylinder : public RenderObject
{
public:
	PolyCylinder(Material* material, Camera* camera);
	~PolyCylinder();

	// Implement base class interface.
	virtual void Render();

	std::vector<LPoint> Points;

	TriangleMeshPtr Sphere;
	TriangleMeshPtr Cylinder;

	void SetWorldTransform(const mat4& trans);
	mat4 GetWorldTransform() const;
	void SetWorldTranslation(const vec3& translation);
	vec3 GetWorldTranslation() const;

private:
	mat4 mWorldTransform;
	//Camera* mCamera;
};

typedef RefPointer<PolyCylinder> PolyCylinderPtr;

}

#endif