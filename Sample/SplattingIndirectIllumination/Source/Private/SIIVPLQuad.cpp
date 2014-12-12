#include "SIIVPLQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SIIVPLQuad::SIIVPLQuad(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
SIIVPLQuad::~SIIVPLQuad()
{
    RSMPositionTexture = 0;
    RSMNormalTexture = 0;
    RSMFluxTexture = 0;
    RSMSampleTexture = 0;
}
//----------------------------------------------------------------------------
void SIIVPLQuad::OnGetShaderConstants()
{
    // Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mRSMPositionTextureLoc, "RSMPositionSampler");
    program->GetUniformLocation(&mRSMNormalTextureLoc, "RSMNormalSampler");
    program->GetUniformLocation(&mRSMFluxTextureLoc, "RSMFluxSampler");
    program->GetUniformLocation(&mRSMSampleTextureLoc, "RSMSampleSampler");
}
//----------------------------------------------------------------------------
void SIIVPLQuad::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        mRSMPositionTextureLoc.SetValue(0);
        mRSMNormalTextureLoc.SetValue(1);
        mRSMFluxTextureLoc.SetValue(2);
        mRSMSampleTextureLoc.SetValue(3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RSMPositionTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RSMNormalTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, RSMFluxTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_1D, RSMSampleTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	}
}
//----------------------------------------------------------------------------