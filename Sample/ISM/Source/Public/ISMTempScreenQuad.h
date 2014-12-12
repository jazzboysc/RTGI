#ifndef RTGI_ISMTempScreenQuad_H
#define RTGI_ISMTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/06/2014
//----------------------------------------------------------------------------
class ISMTempScreenQuad : public ScreenQuad
{
public:
    ISMTempScreenQuad(Material* material);
    virtual ~ISMTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
    ShaderUniform mTempSamplerLoc;
};

typedef RefPointer<ISMTempScreenQuad> ISMTempScreenQuadPtr;

}

#endif