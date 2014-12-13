#include "VPLTriMesh.h"
#include "VPLApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLTriMesh::VPLTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
    MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
    LightProjector = 0;
}
//----------------------------------------------------------------------------
VPLTriMesh::~VPLTriMesh()
{
    ShadowMap = 0;
    VPLBuffer = 0;
}
//----------------------------------------------------------------------------
void VPLTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mWorldLoc2, "World");
    program->GetUniformLocation(&mViewLoc2, "View");
    program->GetUniformLocation(&mProjLoc2, "Proj");
    program->GetUniformLocation(&mMaterialColorLoc2, "MaterialColor");

    // Get pass 3 uniform locations.
    program = mMaterial->GetProgram(0, 2);
    program->GetUniformLocation(&mWorldLoc3, "World");
    program->GetUniformLocation(&mProjLoc3, "Proj");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mMaterialColorLoc3, "MaterialColor");
    GPU_DEVICE_FUNC_SetProgramParameterInt(program,
        SPP_Geometry_Vertices_Out, VPLApp::RSM_FACE_COUNT * 3);

    // Get pass 4 uniform locations.
    program = mMaterial->GetProgram(0, 3);
    program->GetUniformLocation(&mWorldLoc4, "World");
    program->GetUniformLocation(&mLightProjectorNearFarLoc4, "LightProjectorNearFar");
    //glProgramParameteri(program, GL_GEOMETRY_VERTICES_OUT_EXT, VPLApp::VPL_SAMPLE_COUNT * 3);
}
//----------------------------------------------------------------------------
void VPLTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        if( mCamera )
        {
            float nearFarPlane[2];
            mCamera->GetNearFarPlane(nearFarPlane);
            mLightProjectorNearFarLoc.SetValue(nearFarPlane);
        }
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        mWorldLoc2.SetValue(mWorldTransform);
        mMaterialColorLoc2.SetValue(MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);
        }
    }

    // Update pass 3 uniform data.
    if( pass == 2 )
    {
        mWorldLoc3.SetValue(mWorldTransform);
        mLightColorLoc.SetValue(LightColor);
        mMaterialColorLoc3.SetValue(MaterialColor);

        assert( LightProjector );
        if( LightProjector )
        {
            vec3 lightPosition = LightProjector->GetLocation();
            mLightPositionWorldLoc.SetValue(lightPosition);

            mat4 projTrans = LightProjector->GetProjectionTransform();
            mProjLoc3.SetValue(projTrans);
        }
    }

    // Update pass 4 uniform data.
    if( pass == 3 )
    {
        mWorldLoc4.SetValue(mWorldTransform);
        float nearFarPlane[2];
        nearFarPlane[0] = 0.01f;
        nearFarPlane[1] = 50.0f;
        mLightProjectorNearFarLoc4.SetValue(nearFarPlane);
        VPLBuffer->Bind(0);
    }
}
//----------------------------------------------------------------------------