#include "SimpleVoxelizationTriMesh.h"
#include "SimpleVoxelizationApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::SimpleVoxelizationTriMesh(Material* material, 
    Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::~SimpleVoxelizationTriMesh()
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
    mSceneBBCenterLoc = glGetUniformLocation(program, "SceneBBCenter");
    mSceneBBExtensionLoc = glGetUniformLocation(program, "SceneBBExtension");
    mMaterialColorLoc = glGetUniformLocation(program, "MaterialColor");
    mDimLoc = glGetUniformLocation(program, "dim");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
    mSceneBBCenterLoc2 = glGetUniformLocation(program, "SceneBBCenter");
    mSceneBBExtensionLoc2 = glGetUniformLocation(program, "SceneBBExtension");
    mWorldLoc2 = glGetUniformLocation(program, "World");
    mViewLoc2 = glGetUniformLocation(program, "View");
    mProjLoc2 = glGetUniformLocation(program, "Proj");
    mDimLoc2 = glGetUniformLocation(program, "dim");
}
//----------------------------------------------------------------------------
void SimpleVoxelizationTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        vec3 center = SceneBB->GetBoxCenter();
        vec3 extension = SceneBB->GetExtension();
        glUniform3fv(mSceneBBCenterLoc, 1, (GLfloat*)&center);
        glUniform3fv(mSceneBBExtensionLoc, 1, (GLfloat*)&extension);
        glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
        glUniform1i(mDimLoc, SimpleVoxelizationApp::VOXEL_DIMENSION);
	}

    if( pass == 1 )
    {
        glUniformMatrix4fv(mWorldLoc2, 1, GL_TRUE, mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);

            vec3 center = SceneBB->GetBoxCenter();
            vec3 extension = SceneBB->GetExtension();
            glUniform3fv(mSceneBBCenterLoc2, 1, (GLfloat*)&center);
            glUniform3fv(mSceneBBExtensionLoc2, 1, (GLfloat*)&extension);
            glUniform1i(mDimLoc2, SimpleVoxelizationApp::VOXEL_DIMENSION);
        }
    }
}
//----------------------------------------------------------------------------