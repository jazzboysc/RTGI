#include "DirectLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingQuad::DirectLightingQuad(Material* material, Light* light)
	:
	ScreenQuad(material),
	mLight(light)
{
}
//----------------------------------------------------------------------------
DirectLightingQuad::~DirectLightingQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
}
//----------------------------------------------------------------------------
void DirectLightingQuad::OnUpdateShaderConstants(int, int)
{
	TriangleMesh::OnUpdateShaderConstants(0, 0);

	vec3 lightLoc = mLight->GetLocation();
    mLightPositionLoc.SetValue(lightLoc);
    mLightColorLoc.SetValue(mLight->Color);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    PositionTexture->BindToSampler(0, &sampler);
    NormalTexture->BindToSampler(1, &sampler);
    ReflectanceTexture->BindToSampler(2, &sampler);

	mPositionSamplerLoc.SetValue(0);
	mNormalSamplerLoc.SetValue(1);
	mReflectanceSamplerLoc.SetValue(2);
}
//----------------------------------------------------------------------------
void DirectLightingQuad::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightPositionLoc, "lightPosition");
    program->GetUniformLocation(&mLightColorLoc, "lightColor");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mReflectanceSamplerLoc, "reflectanceSampler");
}
//----------------------------------------------------------------------------