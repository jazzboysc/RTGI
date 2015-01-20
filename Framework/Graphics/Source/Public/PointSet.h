//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PointSet_H
#define RTGI_PointSet_H

#include "RenderObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/19/2013
//----------------------------------------------------------------------------
class PointSet : public RenderObject
{
public:
	PointSet(Material* material);
	~PointSet();

	// Implement base class interface.
	virtual void Render(int technique, int pass);

	// Loada data from user specified memory location.
	void LoadFromMemory(int pointCount, GLfloat* vertexData);

	void SetWorldWindow(GLfloat left, GLfloat top, GLfloat right, 
		GLfloat bottom);

	void CreateDeviceResource();

private:
	float mWorldWindow[4]; // Left, Top, Right, Bottom;
	glm::mat4 mWorldWindowTransform;
	//GLint mWorldWindowTransformLoc;

	int mPointCount;
	std::vector<GLfloat> mVertexData;
};

typedef RefPointer<PointSet> PointSetPtr;

}

#endif