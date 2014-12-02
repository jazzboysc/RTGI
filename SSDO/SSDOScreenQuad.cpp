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

    glUniform1f(mSampleRadiusLoc, SampleRadius);
    glUniform1f(mStrengthLoc, Strength);
    glUniform1f(mSingularityLoc, Singularity);
    glUniform1f(mDepthBiasLoc, DepthBias);
    glUniform1f(mBounceStrengthLoc, BounceStrength);
    glUniform1f(mBounceSingularityLoc, BounceSingularity);
    glUniform1i(mSampleCountLoc, SampleCount);
    glUniform1i(mPatternSizeLoc, PatternSize);
    glUniform1i(mPositionSamplerLoc, 0);
    glUniform1i(mNormalSamplerLoc, 1);
    glUniform1i(mColorSamplerLoc, 2);
    glUniform1i(mDirectLightingSamplerLoc, 3);
    glUniform1i(mRandomSamplerLoc, 4);
    glUniform1i(mEnvMapSamplerLoc, 5);
}
//----------------------------------------------------------------------------
void SSDOScreenQuad::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

	mSampleRadiusLoc = glGetUniformLocation(program, "sampleRadius");
	mStrengthLoc = glGetUniformLocation(program, "strength");
	mSingularityLoc = glGetUniformLocation(program, "singularity");
	mDepthBiasLoc = glGetUniformLocation(program, "depthBias");
	mBounceStrengthLoc = glGetUniformLocation(program, "bounceStrength");
	mBounceSingularityLoc = glGetUniformLocation(program, "bounceSingularity");
	mSampleCountLoc = glGetUniformLocation(program, "sampleCount");
	mPatternSizeLoc = glGetUniformLocation(program, "patternSize");

	mPositionSamplerLoc = glGetUniformLocation(program, "positionSampler");
	mNormalSamplerLoc = glGetUniformLocation(program, "normalSampler");
	mColorSamplerLoc = glGetUniformLocation(program, "colorSampler");
	mDirectLightingSamplerLoc = glGetUniformLocation(program, 
		"directLightingSampler");
	mRandomSamplerLoc = glGetUniformLocation(program, "randomSampler");
	mEnvMapSamplerLoc = glGetUniformLocation(program, "envMapSampler");
}
//----------------------------------------------------------------------------