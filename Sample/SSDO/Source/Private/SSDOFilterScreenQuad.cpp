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
void SSDOFilterScreenQuad::OnUpdateShaderConstants(int, int)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSDOTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ColorTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, DirectLightingTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    mSSDOSamplerLoc.SetValue(0);
    mPositionSamplerLoc.SetValue(1);
    mNormalSamplerLoc.SetValue(2);
    mColorSamplerLoc.SetValue(3);
    mDirectLightingSamplerLoc.SetValue(4);
    mPositionThresholdLoc.SetValue(PositionThreshold);
    mNormalThresholdLoc.SetValue(NormalThreshold);
    mMaxRadianceLoc.SetValue(MaxRadiance);
    mKernelSizeLoc.SetValue(KernelSize);
}
//----------------------------------------------------------------------------
void SSDOFilterScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSSDOSamplerLoc, "ssdoSampler");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mColorSamplerLoc, "colorSampler");
    program->GetUniformLocation(&mDirectLightingSamplerLoc, "directLightingSampler");
    program->GetUniformLocation(&mPositionThresholdLoc, "positionThreshold");
    program->GetUniformLocation(&mNormalThresholdLoc, "normalThreshold");
    program->GetUniformLocation(&mMaxRadianceLoc, "maxRadiance");
    program->GetUniformLocation(&mKernelSizeLoc, "kernelSize");
}
//----------------------------------------------------------------------------