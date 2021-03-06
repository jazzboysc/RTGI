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
    float TessLevel;

private:
    // Scene voxelization pass.
    ShaderUniform mWorldLocSV;
    ShaderUniform mSceneBBCenterLocSV;
    ShaderUniform mSceneBBExtensionLocSV;
    ShaderUniform mMaterialColorLocSV;
    ShaderUniform mDimLocSV;
    ShaderUniform mInv2SceneBBExtensionLocSV;

    // Scene point light shadow map pass.
    ShaderUniform mWorldLocPointLightShadowMap;
    ShaderUniform mTessLevelLoc;

    // Scene spot light shadow map pass.
    ShaderUniform mWorldLocSpotLightShadowMap;

    // G-buffer pass.
    ShaderUniform mWorldLocGBuffer, mViewLocGBuffer, mProjLocGBuffer;
    ShaderUniform mWorldCacheLocGBuffer, mViewCacheLocGBuffer, mProjCacheLocGBuffer;
    ShaderUniform mMaterialColorLocGBuffer;

    // Scene point light RSM pass.
    ShaderUniform mWorldLocPointLightRSM;
    ShaderUniform mMaterialColorLocPointLightRSM;

    // Scene spot light RSM pass.
    ShaderUniform mWorldLocSpotLightRSM;
    ShaderUniform mMaterialColorLocSpotLightRSM;
};

typedef RefPointer<SceneMesh> SceneMeshPtr;

}

#endif