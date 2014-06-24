#include "RSMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMTriMesh::RSMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f),
	mMaterialColorLoc(0)
{
}
//----------------------------------------------------------------------------
RSMTriMesh::~RSMTriMesh()
{
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

    // Get pass 1 uniform locations.
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
    
    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
	mWorldLoc2 = glGetUniformLocation(program, "World");
	mViewLoc2 = glGetUniformLocation(program, "View");
	mProjLoc2 = glGetUniformLocation(program, "Proj");
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        TriangleMesh::OnUpdateShaderConstants(technique, pass);
        glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
    }
    
    // Update pass 2 uniform data.
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