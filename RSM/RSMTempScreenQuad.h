#ifndef RTGI_RSMTempScreenQuad_H
#define RTGI_RSMTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMTempScreenQuad : public ScreenQuad
{
public:
	RSMTempScreenQuad(Material* material, Camera* camera);
	virtual ~RSMTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;
    int ShowMode;

private:
	ShaderUniform mTempSamplerLoc;
    ShaderUniform mNearFarPlaneLoc;
    ShaderUniform mShowModeLoc;
};

typedef RefPointer<RSMTempScreenQuad> RSMTempScreenQuadPtr;

}

#endif