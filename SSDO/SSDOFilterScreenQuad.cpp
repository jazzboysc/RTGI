#include "SSDOFilterScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOFilterScreenQuad::SSDOFilterScreenQuad(Material* material)
	:
	ScreenQuad(material)
{
}
//----------------------------------------------------------------------------
SSDOFilterScreenQuad::~SSDOFilterScreenQuad()
{
	SSDOTexture = 0;
	PositionTexture = 0;
	NormalTexture = 0;
	ColorTexture = 0;
	DirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void SSDOFilterScreenQuad::OnUpdateShaderConstants()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSDOTexture->GetTexture());
	glUniform1i(mSSDOSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glUniform1i(mPositionSamplerLoc, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glUniform1i(mNormalSamplerLoc, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ColorTexture->GetTexture());
	glUniform1i(mColorSamplerLoc, 3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, DirectLightingTexture->GetTexture());
	glUniform1i(mDirectLightingSamplerLoc, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glUniform1f(mPositionThresholdLoc, PositionThreshold);
	glUniform1f(mNormalThresholdLoc, NormalThreshold);
	glUniform1f(mMaxRadianceLoc, MaxRadiance);
	glUniform1i(mKernelSizeLoc, KernelSize);
}
//----------------------------------------------------------------------------
void SSDOFilterScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mSSDOSamplerLoc = glGetUniformLocation(program, "ssdoSampler");
	mPositionSamplerLoc = glGetUniformLocation(program, "positionSampler");
	mNormalSamplerLoc = glGetUniformLocation(program, "normalSampler");
	mColorSamplerLoc = glGetUniformLocation(program, "colorSampler");
	mDirectLightingSamplerLoc = glGetUniformLocation(program, "directLightingSampler");

	mPositionThresholdLoc = glGetUniformLocation(program, "positionThreshold");
	mNormalThresholdLoc = glGetUniformLocation(program, "normalThreshold");
	mMaxRadianceLoc = glGetUniformLocation(program, "maxRadiance");
	mKernelSizeLoc = glGetUniformLocation(program, "kernelSize");
}
//----------------------------------------------------------------------------