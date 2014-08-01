#include "RayBundleTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RayBundleTriMesh::RayBundleTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f),
	EmissionColor(0.0f, 0.0f, 0.0f),
	mMaterialColorLoc(0),
	mEmissionColorLoc(0)
{
}
//----------------------------------------------------------------------------
RayBundleTriMesh::~RayBundleTriMesh()
{
}
//----------------------------------------------------------------------------
void RayBundleTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
	mEmissionColorLoc = glGetUniformLocation(program, "emissionColor");
}
//----------------------------------------------------------------------------
void RayBundleTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);
    glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
	glUniform3fv(mEmissionColorLoc, 1, (GLfloat*)&EmissionColor);
}
//----------------------------------------------------------------------------