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
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*6.8f;
    LightProjector = 0;
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

	// Get shadow map pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_ShadowMap);
    program->GetUniformLocation(&mWorldLocShadowMap, "World");
    program->GetUniformLocation(&mViewLocShadowMap, "View");
    program->GetUniformLocation(&mProjLocShadowMap, "Proj");
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");
    program->GetUniformLocation(&mTessLevelLoc, "TessLevel");

    // Get G-buffer pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_GBuffer);
    program->GetUniformLocation(&mWorldLocGBuffer, "World");
    program->GetUniformLocation(&mViewLocGBuffer, "View");
    program->GetUniformLocation(&mProjLocGBuffer, "Proj");
    program->GetUniformLocation(&mWorldCacheLocGBuffer, "WorldCache");
    program->GetUniformLocation(&mViewCacheLocGBuffer, "ViewCache");
    program->GetUniformLocation(&mProjCacheLocGBuffer, "ProjCache");
    program->GetUniformLocation(&mMaterialColorLocGBuffer, "MaterialColor");

    // Get RSM pass uniform locations.
    program = mMaterial->GetProgram(0, VPLviaSVOGI::SMP_RSM);
    program->GetUniformLocation(&mWorldLocRSM, "World");
    program->GetUniformLocation(&mProjLocRSM, "Proj");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mMaterialColorLocRSM, "MaterialColor");
    program->GetProgramHandle()->Device->SetProgramParameterInt(
		program, SPP_Geometry_Vertices_Out,
        VPLviaSVOGI::RSM_FACE_COUNT * 3);
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

    // Update shadow map pass uniform data.
    if( pass == VPLviaSVOGI::SMP_ShadowMap )
    {
        mWorldLocShadowMap.SetValue(worldTrans);
        mTessLevelLoc.SetValue(TessLevel);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLocShadowMap.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLocShadowMap.SetValue(projTrans);

            float nearFarPlane[2];
            mCamera->GetNearFarPlane(nearFarPlane);
            mLightProjectorNearFarLoc.SetValue(nearFarPlane);
        }
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

    // Update RSM pass uniform data.
    if( pass == VPLviaSVOGI::SMP_RSM )
    {
        mWorldLocRSM.SetValue(worldTrans);
        mLightColorLoc.SetValue(LightColor);
        mMaterialColorLocRSM.SetValue(MaterialColor);

        assert( LightProjector );
        if( LightProjector )
        {
            vec3 lightPosition = LightProjector->GetLocation();
            mLightPositionWorldLoc.SetValue(lightPosition);

            mat4 projTrans = LightProjector->GetProjectionTransform();
            mProjLocRSM.SetValue(projTrans);
        }
    }
}
//----------------------------------------------------------------------------