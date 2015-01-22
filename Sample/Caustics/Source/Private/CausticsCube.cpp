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

    SamplerDesc sampler;
    sampler.MinFilter = FT_Linear_Linear;
    sampler.MagFilter = FT_Linear;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;
    sampler.WrapR = WT_Clamp;

    CubeTexture->BindToSampler(0, &sampler);

	mCubemapSampler.SetValue(0);
}
//----------------------------------------------------------------------------