#include "SceneMesh.h"
#include "BidirectionalVoxelGIApp.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SceneMesh::SceneMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
    MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
    LightProjector = 0;
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
    program = mMaterial->GetProgram(0, BidirectionalVoxelGIApp::SMP_Voxelization);
    program->GetUniformLocation(&mWorldLocSV, "World");
    program->GetUniformLocation(&mViewLocSV, "View");
    program->GetUniformLocation(&mProjLocSV, "Proj");
    program->GetUniformLocation(&mSceneBBCenterLocSV, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLocSV, "SceneBBExtension");
    program->GetUniformLocation(&mMaterialColorLocSV, "MaterialColor");
    program->GetUniformLocation(&mDimLocSV, "dim");
    program->GetUniformLocation(&mInv2SceneBBExtensionLocSV, "Inv2SceneBBExtension");

    // Get show voxelization pass uniform locations.
    program = mMaterial->GetProgram(0, BidirectionalVoxelGIApp::SMP_ShowVoxelization);
    program->GetUniformLocation(&mWorldLocSSV, "World");
    program->GetUniformLocation(&mViewLocSSV, "View");
    program->GetUniformLocation(&mProjLocSSV, "Proj");
    program->GetUniformLocation(&mSceneBBCenterLocSSV, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLocSSV, "SceneBBExtension");
    program->GetUniformLocation(&mDimLocSSV, "dim");

	// Get shadow map pass uniform locations.
    program = mMaterial->GetProgram(0, BidirectionalVoxelGIApp::SMP_ShadowMap);
    program->GetUniformLocation(&mWorldLocShadowMap, "World");
    program->GetUniformLocation(&mViewLocShadowMap, "View");
    program->GetUniformLocation(&mProjLocShadowMap, "Proj");
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");

    // Get G-buffer pass uniform locations.
    program = mMaterial->GetProgram(0, BidirectionalVoxelGIApp::SMP_GBuffer);
    program->GetUniformLocation(&mWorldLocGBuffer, "World");
    program->GetUniformLocation(&mViewLocGBuffer, "View");
    program->GetUniformLocation(&mProjLocGBuffer, "Proj");
    program->GetUniformLocation(&mMaterialColorLocGBuffer, "MaterialColor");

    // Get RSM pass uniform locations.
    program = mMaterial->GetProgram(0, BidirectionalVoxelGIApp::SMP_RSM);
    program->GetUniformLocation(&mWorldLocRSM, "World");
    program->GetUniformLocation(&mProjLocRSM, "Proj");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mMaterialColorLocRSM, "MaterialColor");
    GPU_DEVICE_FUNC_SetProgramParameterInt(program, 
        SPP_Geometry_Vertices_Out,
        BidirectionalVoxelGIApp::RSM_FACE_COUNT * 3);
}
//----------------------------------------------------------------------------
void SceneMesh::OnUpdateShaderConstants(int technique, int pass)
{
    if( pass == BidirectionalVoxelGIApp::SMP_Voxelization )
    {
        mWorldLocSV.SetValue(mWorldTransform);

        vec3 center = SceneBB->GetBoxCenter();
        vec3 extension = SceneBB->GetExtension();
        vec3 inv2extension = vec3(1.0f / (2.0f*extension.x), 1.0f / (2.0f*extension.y), 1.0f / (2.0f*extension.z));

        mSceneBBCenterLocSV.SetValue(center);
        mSceneBBExtensionLocSV.SetValue(extension);
        mMaterialColorLocSV.SetValue(MaterialColor);
        mDimLocSV.SetValue(BidirectionalVoxelGIApp::VOXEL_DIMENSION);
        mInv2SceneBBExtensionLocSV.SetValue(inv2extension);
    }

    if( pass == BidirectionalVoxelGIApp::SMP_ShowVoxelization )
    {
        mWorldLocSSV.SetValue(mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLocSSV.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLocSSV.SetValue(projTrans);
        }

        vec3 center = SceneBB->GetBoxCenter();
        vec3 extension = SceneBB->GetExtension();
        mSceneBBCenterLocSSV.SetValue(center);
        mSceneBBExtensionLocSSV.SetValue(extension);
        mDimLocSSV.SetValue(BidirectionalVoxelGIApp::VOXEL_DIMENSION);
    }

    // Update shadow map pass uniform data.
    if( pass == BidirectionalVoxelGIApp::SMP_ShadowMap )
    {
        mWorldLocShadowMap.SetValue(mWorldTransform);

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
    if( pass == BidirectionalVoxelGIApp::SMP_GBuffer )
    {
        mWorldLocGBuffer.SetValue(mWorldTransform);
        mMaterialColorLocGBuffer.SetValue(MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLocGBuffer.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLocGBuffer.SetValue(projTrans);
        }
    }

    // Update RSM pass uniform data.
    if( pass == BidirectionalVoxelGIApp::SMP_RSM )
    {
        mWorldLocRSM.SetValue(mWorldTransform);
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