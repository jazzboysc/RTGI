#include "SSDOTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOTriMesh::SSDOTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
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

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
}
//----------------------------------------------------------------------------
void SSDOTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

    mMaterialColorLoc.SetValue(MaterialColor);
}
//----------------------------------------------------------------------------