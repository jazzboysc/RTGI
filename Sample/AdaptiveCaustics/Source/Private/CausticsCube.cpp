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

	auto program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");

	program->GetUniformLocation(&mCubemapSampler, "cubemapSampler");
}
//----------------------------------------------------------------------------
void CausticsCube::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear_Linear;
	sampler.MagFilter = FT_Linear;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;
	sampler.WrapR = WT_Clamp;

	if (technique == 0)
	{
		mMaterialColorLoc.SetValue(MaterialColor);
		CubeTexture->BindToSampler(0, &sampler);
		mCubemapSampler.SetValue(0);
	}

	if (technique == 1)
	{
		vec3 lightLoc = Light->GetLocation();
		mLightPositionLoc.SetValue(lightLoc);
		mLightViewLoc.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc.SetValue(Light->GetProjector()->GetProjectionTransform());
		mLightColorLoc.SetValue(Light->Color);

		VertexSplattingTexture->BindToSampler(0, &sampler);
		mVertexSplattingSamplerLoc.SetValue(0);
	}
}
//----------------------------------------------------------------------------