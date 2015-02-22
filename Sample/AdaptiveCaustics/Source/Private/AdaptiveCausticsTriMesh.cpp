#include "AdaptiveCausticsTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticsTriMesh::CausticsTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
CausticsTriMesh::~CausticsTriMesh()
{
}
//----------------------------------------------------------------------------
void CausticsTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
}
//----------------------------------------------------------------------------
void CausticsTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

    mMaterialColorLoc.SetValue(MaterialColor);
}
//----------------------------------------------------------------------------