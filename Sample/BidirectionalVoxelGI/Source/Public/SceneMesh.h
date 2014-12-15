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

    AABB* SceneBB;
	vec3 MaterialColor;
    vec3 LightColor;
    Camera* LightProjector;

private:
    // Scene voxelization pass.
    ShaderUniform mWorldLocSV, mViewLocSV, mProjLocSV;
    ShaderUniform mSceneBBCenterLocSV;
    ShaderUniform mSceneBBExtensionLocSV;
    ShaderUniform mMaterialColorLocSV;
    ShaderUniform mDimLocSV;
    ShaderUniform mInv2SceneBBExtensionLocSV;

    // Show scene voxelization pass.
    ShaderUniform mWorldLocSSV, mViewLocSSV, mProjLocSSV;
    ShaderUniform mSceneBBCenterLocSSV;
    ShaderUniform mSceneBBExtensionLocSSV;
    ShaderUniform mDimLocSSV;

    // Scene shadow map pass.
    ShaderUniform mWorldLocShadowMap, mViewLocShadowMap, mProjLocShadowMap;
    ShaderUniform mLightProjectorNearFarLoc;

    // G-buffer pass.
    ShaderUniform mWorldLocGBuffer, mViewLocGBuffer, mProjLocGBuffer;
    ShaderUniform mMaterialColorLocGBuffer;

    // RSM pass.
    ShaderUniform mWorldLocRSM, mProjLocRSM;
    ShaderUniform mLightPositionWorldLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mMaterialColorLocRSM;
};

typedef RefPointer<SceneMesh> SceneMeshPtr;

}

#endif