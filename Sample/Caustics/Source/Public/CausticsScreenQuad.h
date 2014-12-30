#pragma once
#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class CausticsScreenQuad : public ScreenQuad
{
public:
	CausticsScreenQuad(Material* material, Camera* camera);
	virtual ~CausticsScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	LightPtr Light;

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ReflectanceTexture;

private:
	ShaderUniform mLightPositionLoc;
	ShaderUniform mLightColorLoc;

    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mReflectanceSamplerLoc;
};

typedef RefPointer<CausticsScreenQuad> CausticsScreenQuadPtr;

}
