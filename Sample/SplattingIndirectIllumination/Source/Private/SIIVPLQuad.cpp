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

        SamplerDesc sampler;
        sampler.MinFilter = FT_Nearest;
        sampler.MagFilter = FT_Nearest;
        sampler.WrapS = WT_Clamp;
        sampler.WrapT = WT_Clamp;

        RSMPositionTexture->BindToSampler(0, &sampler);
        RSMNormalTexture->BindToSampler(1, &sampler);
        RSMFluxTexture->BindToSampler(2, &sampler);
        RSMSampleTexture->BindToSampler(3, &sampler);
	}
}
//----------------------------------------------------------------------------