#include "VPLTriMesh.h"
#include "VPLApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLTriMesh::VPLTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    mLightProjectorNearFarLoc = -1;
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
    LightProjector = 0;
    ShadowMap = 0;
}
//----------------------------------------------------------------------------
VPLTriMesh::~VPLTriMesh()
{
}
//----------------------------------------------------------------------------
void VPLTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
    mLightProjectorNearFarLoc = 
        glGetUniformLocation(program, "LightProjectorNearFar");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
    mWorldLoc2 = glGetUniformLocation(program, "World");
    mViewLoc2 = glGetUniformLocation(program, "View");
    mProjLoc2 = glGetUniformLocation(program, "Proj");
    mMaterialColorLoc2 = glGetUniformLocation(program, "MaterialColor");

    // Get pass 3 uniform locations.
    program = mMaterial->GetProgram(0, 2)->GetProgram();
    mWorldLoc3 = glGetUniformLocation(program, "World");
    mProjLoc3 = glGetUniformLocation(program, "Proj");
    mLightPositionWorldLoc = glGetUniformLocation(program, "LightPositionWorld");
    mLightColorLoc = glGetUniformLocation(program, "LightColor");
    mMaterialColorLoc3 = glGetUniformLocation(program, "MaterialColor");
    glProgramParameteri(program, GL_GEOMETRY_VERTICES_OUT_EXT, VPLApp::RSM_FACE_COUNT * 3);
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
            glUniform2fv(mLightProjectorNearFarLoc, 1, nearFarPlane);
        }
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        glUniformMatrix4fv(mWorldLoc2, 1, GL_TRUE, mWorldTransform);
        glUniform3fv(mMaterialColorLoc2, 1, (GLfloat*)&MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);
        }
    }

    // Update pass 3 uniform data.
    if( pass == 2 )
    {
        glUniformMatrix4fv(mWorldLoc3, 1, GL_TRUE, mWorldTransform);
        glUniform3fv(mLightColorLoc, 1, (GLfloat*)&LightColor);
        glUniform3fv(mMaterialColorLoc3, 1, (GLfloat*)&MaterialColor);

        assert( LightProjector );
        if( LightProjector )
        {
            vec3 lightPosition = LightProjector->GetLocation();
            glUniform3fv(mLightPositionWorldLoc, 1, (GLfloat*)&lightPosition);

            mat4 projTrans = LightProjector->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc3, 1, GL_TRUE, projTrans);
        }
    }
}
//----------------------------------------------------------------------------