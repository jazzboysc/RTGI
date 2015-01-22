#include "RSMDeferredLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMDeferredLightingQuad::RSMDeferredLightingQuad(Material* material,
    Light* light)
	:
	ScreenQuad(material),
	mLight(light)
{
}
//----------------------------------------------------------------------------
RSMDeferredLightingQuad::~RSMDeferredLightingQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
    IndirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void RSMDeferredLightingQuad::OnUpdateShaderConstants(int, int)
{
    vec3 lightLoc = mLight->GetLocation();
    mLightPositionLoc.SetValue(lightLoc);
    mLightColorLoc.SetValue(mLight->Color);
    mPositionSamplerLoc.SetValue(0);
    mNormalSamplerLoc.SetValue(1);
    mReflectanceSamplerLoc.SetValue(2);
    mIndirectLightingSamplerLoc.SetValue(3);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    PositionTexture->BindToSampler(0, &sampler);
    NormalTexture->BindToSampler(1, &sampler);
    ReflectanceTexture->BindToSampler(2, &sampler);
    IndirectLightingTexture->BindToSampler(3, &sampler);
}
//----------------------------------------------------------------------------
void RSMDeferredLightingQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightPositionLoc, "lightPosition");
    program->GetUniformLocation(&mLightColorLoc, "lightColor");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mReflectanceSamplerLoc, "reflectanceSampler");
    program->GetUniformLocation(&mIndirectLightingSamplerLoc, 
        "indirectLightingSampler");
}
//----------------------------------------------------------------------------