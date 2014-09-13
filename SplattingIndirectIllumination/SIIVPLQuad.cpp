#include "SIIVPLQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SIIVPLQuad::SIIVPLQuad(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
    mRSMPositionTextureLoc = -1;
    mRSMNormalTextureLoc = -1;
    mRSMFluxTextureLoc = -1;
    mRSMSampleTextureLoc = -1;
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
    GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
    mRSMPositionTextureLoc =
        glGetUniformLocation(program, "RSMPositionSampler");
    mRSMNormalTextureLoc =
        glGetUniformLocation(program, "RSMNormalSampler");
    mRSMFluxTextureLoc =
        glGetUniformLocation(program, "RSMFluxSampler");
    mRSMSampleTextureLoc =
        glGetUniformLocation(program, "RSMSampleSampler");
}
//----------------------------------------------------------------------------
void SIIVPLQuad::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RSMPositionTexture->GetTexture());
        glUniform1i(mRSMPositionTextureLoc, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RSMNormalTexture->GetTexture());
        glUniform1i(mRSMNormalTextureLoc, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, RSMFluxTexture->GetTexture());
        glUniform1i(mRSMFluxTextureLoc, 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_1D, RSMSampleTexture->GetTexture());
        glUniform1i(mRSMSampleTextureLoc, 3);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	}
}
//----------------------------------------------------------------------------