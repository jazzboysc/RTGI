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
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    GPU_DEVICE_FUNC_GetUniformLocation(program, mLightProjectorNearFarLoc, 
        "LightProjectorNearFar");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1);
    GPU_DEVICE_FUNC_GetUniformLocation(program, mWorldLoc2, "World");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mViewLoc2, "View");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mProjLoc2, "Proj");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mMaterialColorLoc2, "MaterialColor");

    // Get pass 3 uniform locations.
    program = mMaterial->GetProgram(0, 2);
    GPU_DEVICE_FUNC_GetUniformLocation(program, mWorldLoc3, "World");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mProjLoc3, "Proj");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mLightPositionWorldLoc, "LightPositionWorld");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mLightColorLoc, "LightColor");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mMaterialColorLoc3, "MaterialColor");
    GPU_DEVICE_FUNC_SetProgramParameterInt(program, 
        GL_GEOMETRY_VERTICES_OUT_EXT, 
        BidirectionalVoxelGIApp::RSM_FACE_COUNT * 3);
}
//----------------------------------------------------------------------------
void SceneMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        if( mCamera )
        {
            float nearFarPlane[2];
            mCamera->GetNearFarPlane(nearFarPlane);
            GPU_DEVICE_FUNC_SetUniformValueFloat2(mLightProjectorNearFarLoc, 
                nearFarPlane);
        }
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc2, mWorldTransform);
        GPU_DEVICE_FUNC_SetUniformValueVec3(mMaterialColorLoc2, MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc2, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc2, projTrans);
        }
    }

    // Update pass 3 uniform data.
    if( pass == 2 )
    {
        GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc3, mWorldTransform);
        GPU_DEVICE_FUNC_SetUniformValueVec3(mLightColorLoc, LightColor);
        GPU_DEVICE_FUNC_SetUniformValueVec3(mMaterialColorLoc3, MaterialColor);

        assert( LightProjector );
        if( LightProjector )
        {
            vec3 lightPosition = LightProjector->GetLocation();
            GPU_DEVICE_FUNC_SetUniformValueVec3(mLightPositionWorldLoc, lightPosition);

            mat4 projTrans = LightProjector->GetProjectionTransform();
            GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc3, projTrans);
        }
    }
}
//----------------------------------------------------------------------------