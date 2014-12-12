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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ColorTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, DirectLightingTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, RandomTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, EnvMapTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    TriangleMesh::OnUpdateShaderConstants(technique, pass);

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