#include "SSDOTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOTriMesh::SSDOTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f),
	mMaterialColorLoc(0)
{
}
//----------------------------------------------------------------------------
SSDOTriMesh::~SSDOTriMesh()
{
}
//----------------------------------------------------------------------------
void SSDOTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
}
//----------------------------------------------------------------------------
void SSDOTriMesh::OnUpdateShaderConstants()
{
	TriangleMesh::OnUpdateShaderConstants();

	glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
}
//----------------------------------------------------------------------------