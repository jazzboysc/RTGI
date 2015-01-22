#include "RSMTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMTempScreenQuad::RSMTempScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
    ShowMode = 2;
}
//----------------------------------------------------------------------------
RSMTempScreenQuad::~RSMTempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void RSMTempScreenQuad::OnUpdateShaderConstants(int, int)
{
    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    TempTexture->BindToSampler(0, &sampler);
    
    float nearFarPlane[2];
    mCamera->GetNearFarPlane(nearFarPlane);
    mNearFarPlaneLoc.SetValue(nearFarPlane);
    mShowModeLoc.SetValue(ShowMode);
    mTempSamplerLoc.SetValue(0);
}
//----------------------------------------------------------------------------
void RSMTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
    program->GetUniformLocation(&mNearFarPlaneLoc, "nearFarPlane");
    program->GetUniformLocation(&mShowModeLoc, "showMode");
}
//----------------------------------------------------------------------------