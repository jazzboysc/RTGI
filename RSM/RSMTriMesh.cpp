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

	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnUpdateShaderConstants()
{
	TriangleMesh::OnUpdateShaderConstants();

	glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
}
//----------------------------------------------------------------------------