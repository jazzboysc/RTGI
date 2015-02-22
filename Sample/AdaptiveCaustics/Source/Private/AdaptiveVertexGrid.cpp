#include "AdaptiveVertexGrid.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VertexGrid::VertexGrid(int width, int height, Material* material)
	:
	PointSet(material),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
	mPointCount = width * height;
}
//----------------------------------------------------------------------------
VertexGrid::~VertexGrid()
{
}
//----------------------------------------------------------------------------
void VertexGrid::OnUpdateShaderConstants(int technique, int pass)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Nearest;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;
	sampler.WrapR = WT_Clamp;

	PointSet::OnUpdateShaderConstants(technique, pass);

	if (pass == 0)
	{
		mLightViewLoc.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc.SetValue(Light->GetProjector()->GetProjectionTransform());
		mLightColorLoc.SetValue(Light->Color);


		IntersectionPositionTexture->BindToSampler(0, &sampler);
		//CausticsMapTexture->BindToSampler(1, &sampler);

		mIntersectionPositionSamplerLoc.SetValue(0);
		//mCausticsMapSamplerLoc.SetValue(1);

		mCausticsMapResolutionLoc.SetValue(CausticsMapsResolution);
	}

	mMaterialColorLoc.SetValue(MaterialColor);
}
//----------------------------------------------------------------------------
void VertexGrid::OnGetShaderConstants()
{
	PointSet::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");

	program->GetUniformLocation(&mWorldLoc, "World");
	program->GetUniformLocation(&mViewLoc, "View");
	program->GetUniformLocation(&mProjLoc, "Proj");

	program->GetUniformLocation(&mIntersectionPositionSamplerLoc, "intersectionPositionSampler");
	program->GetUniformLocation(&mCausticsMapSamplerLoc, "causticsMapSampler");
	program->GetUniformLocation(&mCausticsMapResolutionLoc, "causticsMapResolution");
	program->GetUniformLocation(&mLightViewLoc, "lightView");
	program->GetUniformLocation(&mLightProjLoc, "lightProj");
	program->GetUniformLocation(&mLightColorLoc, "lightColor");

}
//----------------------------------------------------------------------------
