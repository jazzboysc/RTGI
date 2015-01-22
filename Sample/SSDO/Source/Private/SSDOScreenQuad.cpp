#include "SSDOScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOScreenQuad::SSDOScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
}
//----------------------------------------------------------------------------
SSDOScreenQuad::~SSDOScreenQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ColorTexture = 0;
	DirectLightingTexture = 0;
	RandomTexture = 0;
	EnvMapTexture = 0;
}
//----------------------------------------------------------------------------
void SSDOScreenQuad::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    PositionTexture->BindToSampler(0, &sampler);
    NormalTexture->BindToSampler(1, &sampler);
    ColorTexture->BindToSampler(2, &sampler);
    DirectLightingTexture->BindToSampler(3, &sampler);
    RandomTexture->BindToSampler(4, &sampler);
    EnvMapTexture->BindToSampler(5, &sampler);

    mSampleRadiusLoc.SetValue(SampleRadius);
    mStrengthLoc.SetValue(Strength);
    mSingularityLoc.SetValue(Singularity);
    mDepthBiasLoc.SetValue(DepthBias);
    mBounceStrengthLoc.SetValue(BounceStrength);
    mBounceSingularityLoc.SetValue(BounceSingularity);
    mSampleCountLoc.SetValue(SampleCount);
    mPatternSizeLoc.SetValue(PatternSize);
    mPositionSamplerLoc.SetValue(0);
    mNormalSamplerLoc.SetValue(1);
    mColorSamplerLoc.SetValue(2);
    mDirectLightingSamplerLoc.SetValue(3);
    mRandomSamplerLoc.SetValue(4);
    mEnvMapSamplerLoc.SetValue(5);
}
//----------------------------------------------------------------------------
void SSDOScreenQuad::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mSampleRadiusLoc, "sampleRadius");
    program->GetUniformLocation(&mStrengthLoc, "strength");
    program->GetUniformLocation(&mSingularityLoc, "singularity");
    program->GetUniformLocation(&mDepthBiasLoc, "depthBias");
    program->GetUniformLocation(&mBounceStrengthLoc, "bounceStrength");
    program->GetUniformLocation(&mBounceSingularityLoc, "bounceSingularity");
    program->GetUniformLocation(&mSampleCountLoc, "sampleCount");
    program->GetUniformLocation(&mPatternSizeLoc, "patternSize");

    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mColorSamplerLoc, "colorSampler");
    program->GetUniformLocation(&mDirectLightingSamplerLoc,
		"directLightingSampler");
    program->GetUniformLocation(&mRandomSamplerLoc, "randomSampler");
    program->GetUniformLocation(&mEnvMapSamplerLoc, "envMapSampler");
}
//----------------------------------------------------------------------------