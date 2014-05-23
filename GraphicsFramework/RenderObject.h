//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderObject_H
#define RTGI_RenderObject_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Material.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class RenderObject : public RefObject
{
public:
	RenderObject(Material* material);
	virtual ~RenderObject();

	virtual void Render() = 0;

	virtual void OnUpdateShaderConstants(){};
	virtual void OnRender(){};
	virtual void OnEnableBuffers(){};
	virtual void OnDisableBuffers(){};

protected:
	Material* mMaterial;

	GLuint mVAO;
	GLuint mVBO;
};

typedef RefPointer<RenderObject> RenderObjectPtr;

}

#endif