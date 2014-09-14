#ifndef RTGI_VPLviaRSMTempScreenQuad_H
#define RTGI_VPLviaRSMTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2014
//----------------------------------------------------------------------------
class VPLviaRSMTempScreenQuad : public ScreenQuad
{
public:
    VPLviaRSMTempScreenQuad(Material* material);
    virtual ~VPLviaRSMTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	GLint mTempSamplerLoc;
};

typedef RefPointer<VPLviaRSMTempScreenQuad> VPLviaRSMTempScreenQuadPtr;

}

#endif