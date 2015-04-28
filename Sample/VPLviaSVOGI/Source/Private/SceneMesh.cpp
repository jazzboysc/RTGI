#include "SceneMesh.h"
#include "VPLviaSVOGIApp.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SceneMesh::SceneMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
    MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    TessLevel = 14.0f;
}
//----------------------------------------------------------------------------
SceneMesh::~SceneMesh()
{
}
//----------------------------------------------------------------------------
void SceneMesh::OnGetShaderConstants()
{
    ShaderProgram* program = 0;

    // Get scene voxelization pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_Voxelization);
    program->GetUniformLocation(&mWorldLocSV, "World");
    program->GetUniformLocation(&mMaterialColorLocSV, "MaterialColor");
    // No need to do these for SVO voxelizer, but it's OK.
    program->GetUniformLocation(&mSceneBBCenterLocSV, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLocSV, "SceneBBExtension");
    program->GetUniformLocation(&mDimLocSV, "dim");
    program->GetUniformLocation(&mInv2SceneBBExtensionLocSV, "Inv2SceneBBExtension");

	// Get point light shadow map pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_PointLightShadowMap);
    program->GetUniformLocation(&mWorldLocPointLightShadowMap, "World");
    program->GetUniformLocation(&mTessLevelLoc, "TessLevel");

    // Get spot light shadow map pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_SpotLightShadowMap);
    program->GetUniformLocation(&mWorldLocSpotLightShadowMap, "World");

    // Get G-buffer pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_GBuffer);
    program->GetUniformLocation(&mWorldLocGBuffer, "World");
    program->GetUniformLocation(&mViewLocGBuffer, "View");
    program->GetUniformLocation(&mProjLocGBuffer, "Proj");
    program->GetUniformLocation(&mWorldCacheLocGBuffer, "WorldCache");
    program->GetUniformLocation(&mViewCacheLocGBuffer, "ViewCache");
    program->GetUniformLocation(&mProjCacheLocGBuffer, "ProjCache");
    program->GetUniformLocation(&mMaterialColorLocGBuffer, "MaterialColor");

    // Get point light RSM pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_PointLightRSM);
    program->GetUniformLocation(&mWorldLocRSM, "World");
    program->GetUniformLocation(&mMaterialColorLocRSM, "MaterialColor");
}
//----------------------------------------------------------------------------
void SceneMesh::OnUpdateShaderConstants(int technique, int pass)
{
    glm::mat4 worldTrans = mSpatialInfo->GetWorldTransform();

    // Update scene voxelization pass uniform data.
    if( pass == VPLviaSVOGI::SMP_Voxelization )
    {
        mWorldLocSV.SetValue(worldTrans);
        mMaterialColorLocSV.SetValue(MaterialColor);

        // No need to do these for SVO voxelizer, but it's OK.

        vec3 center = SceneBB->GetBoxCenter();
        vec3 extension = SceneBB->GetExtension();
        vec3 inv2extension = vec3(1.0f / (2.0f*extension.x), 1.0f / (2.0f*extension.y), 1.0f / (2.0f*extension.z));

        mSceneBBCenterLocSV.SetValue(center);
        mSceneBBExtensionLocSV.SetValue(extension);
        mDimLocSV.SetValue(VPLviaSVOGI::VOXEL_DIMENSION);
        mInv2SceneBBExtensionLocSV.SetValue(inv2extension);
    }

    // Update point light shadow map pass uniform data.
    if( pass == VPLviaSVOGI::SMP_PointLightShadowMap )
    {
        mWorldLocPointLightShadowMap.SetValue(worldTrans);
        mTessLevelLoc.SetValue(TessLevel);
	}

    // Update spot light shadow map pass uniform data.
    if( pass == VPLviaSVOGI::SMP_SpotLightShadowMap )
    {
        mWorldLocSpotLightShadowMap.SetValue(worldTrans);
    }

    // Update G-buffer pass uniform data.
    if( pass == VPLviaSVOGI::SMP_GBuffer )
    {
        mWorldLocGBuffer.SetValue(worldTrans);
        mMaterialColorLocGBuffer.SetValue(MaterialColor);

        glm::mat4 worldCacheTrans = *mRenderCache->GetWorldCache();
        mWorldCacheLocGBuffer.SetValue(worldCacheTrans);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLocGBuffer.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLocGBuffer.SetValue(projTrans);

            mat4 viewCacheTrans = mCamera->GetViewCacheTransform();
            mViewCacheLocGBuffer.SetValue(viewCacheTrans);

            mat4 projCacheTrans = mCamera->GetProjectionCacheTransform();
            mProjCacheLocGBuffer.SetValue(projCacheTrans);
        }
    }

    // Update point light RSM pass uniform data.
    if( pass == VPLviaSVOGI::SMP_PointLightRSM )
    {
        mWorldLocRSM.SetValue(worldTrans);
        mMaterialColorLocRSM.SetValue(MaterialColor);
    }
}
//----------------------------------------------------------------------------