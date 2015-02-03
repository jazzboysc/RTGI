#include "VertexGrid.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VertexGrid::VertexGrid(int width, int height, Material* material)
	:
	PointSet(material),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
	mPointCount = width * height;
	mVertexData.resize(mPointCount);
	mVertexData[5] = 1.0f;
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
		mLightViewLoc2.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc2.SetValue(Light->GetProjector()->GetProjectionTransform());


		IntersectionPositionTexture->BindToSampler(0, &sampler);
		CausticsMapTexture->BindToSampler(1, &sampler);

		mIntersectionPositionSamplerLoc.SetValue(0);
		mCausticsMapSamplerLoc.SetValue(1);

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

	program->GetUniformLocation(&mWorldLoc2, "World");
	program->GetUniformLocation(&mViewLoc2, "View");
	program->GetUniformLocation(&mProjLoc2, "Proj");

	program->GetUniformLocation(&mIntersectionPositionSamplerLoc, "intersectionPositionSampler");
	program->GetUniformLocation(&mCausticsMapSamplerLoc, "causticsMapSampler");
	program->GetUniformLocation(&mCausticsMapResolutionLoc, "causticsMapResolution");
	program->GetUniformLocation(&mLightViewLoc2, "lightView");
	program->GetUniformLocation(&mLightProjLoc2, "lightProj");

}
//----------------------------------------------------------------------------
