#ifndef RTGI_SceneMesh_H
#define RTGI_SceneMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class SceneMesh : public TriangleMesh
{
public:
	SceneMesh(Material* material, Camera* camera);
	virtual ~SceneMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
    vec3 LightColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;
    StructuredBufferPtr VPLBuffer;

private:
    // pass 1, scene shadow pass.
    GLint mLightProjectorNearFarLoc;

    // pass 2, G-buffer pass.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mMaterialColorLoc2;

    // pass 3, RSM pass.
    GLint mWorldLoc3, mProjLoc3;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mMaterialColorLoc3;
};

typedef RefPointer<SceneMesh> SceneMeshPtr;

}

#endif