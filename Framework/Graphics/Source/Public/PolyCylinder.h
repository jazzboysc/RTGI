//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PolyCylinder_H
#define RTGI_PolyCylinder_H


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
	virtual void Render(int technique, int pass);

	std::vector<LPoint> Points;

	TriangleMeshPtr Sphere;
	TriangleMeshPtr Cylinder;

	void SetWorldTransform(const glm::mat4& trans);
	glm::mat4 GetWorldTransform() const;
	void SetWorldTranslation(const glm::vec3& translation);
	glm::vec3 GetWorldTranslation() const;

private:
	glm::mat4 mWorldTransform;
	//Camera* mCamera;
};

typedef RefPointer<PolyCylinder> PolyCylinderPtr;

}

#endif