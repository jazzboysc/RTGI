#include "SimpleVoxelizationTriMesh.h"

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
    SceneVoxels = 0;
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

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
    mSceneBBCenterLoc2 = glGetUniformLocation(program, "SceneBBCenter");
    mSceneBBExtensionLoc2 = glGetUniformLocation(program, "SceneBBExtension");
    mWorldLoc2 = glGetUniformLocation(program, "World");
    mViewLoc2 = glGetUniformLocation(program, "View");
    mProjLoc2 = glGetUniformLocation(program, "Proj");
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
        SceneVoxels->BindToImageUnit(0, GL_READ_WRITE);
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
        }
    }
}
//----------------------------------------------------------------------------