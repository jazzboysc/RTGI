#include "GPGPUPostProcessingTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUPostProcessingTriMesh::GPGPUPostProcessingTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f),
	mMaterialColorLoc(0)
{
}
//----------------------------------------------------------------------------
GPGPUPostProcessingTriMesh::~GPGPUPostProcessingTriMesh()
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

	glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
}
//----------------------------------------------------------------------------