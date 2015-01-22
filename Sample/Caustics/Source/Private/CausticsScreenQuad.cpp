#include "CausticsScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticsScreenQuad::CausticsScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
}
//----------------------------------------------------------------------------
CausticsScreenQuad::~CausticsScreenQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
}
//----------------------------------------------------------------------------
void CausticsScreenQuad::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

	vec3 lightLoc = Light->GetLocation();
	mLightPositionLoc.SetValue(lightLoc);
	mLightColorLoc.SetValue(Light->Color);

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
void CausticsScreenQuad::OnGetShaderConstants()
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