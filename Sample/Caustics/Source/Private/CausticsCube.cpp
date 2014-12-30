#include "CausticsCube.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticsCube::CausticsCube(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
CausticsCube::~CausticsCube()
{
}
//----------------------------------------------------------------------------
void CausticsCube::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
	program->GetUniformLocation(&mCubemapSampler, "cubemapSampler");
}
//----------------------------------------------------------------------------
void CausticsCube::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

    mMaterialColorLoc.SetValue(MaterialColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);

	mCubemapSampler.SetValue(0);
}
//----------------------------------------------------------------------------