//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderObject_H
#define RTGI_RenderObject_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Material.h"
#include "Camera.h"
#include "Pass.h"

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

    void SetCamera(Camera* camera);
    Camera* GetCamera() const;

	virtual void Render(int technique, int pass) = 0;
	virtual void OnUpdateShaderConstants(int technique, int pass) = 0;
    virtual void OnRender(Pass* pass, PassInfo* passInfo){};
	virtual void OnEnableBuffers(){};
	virtual void OnDisableBuffers(){};

protected:
	Material* mMaterial;
    Camera* mCamera;

	GLuint mVBO;
};

typedef RefPointer<RenderObject> RenderObjectPtr;

}

#endif