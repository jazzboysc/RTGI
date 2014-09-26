#ifndef RTGI_VPLTempScreenQuad_H
#define RTGI_VPLTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "Texture2DArray.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/21/2014
//----------------------------------------------------------------------------
class VPLTempScreenQuad : public ScreenQuad
{
public:
    VPLTempScreenQuad(Material* material);
    virtual ~VPLTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;
    Texture2DPtr TempTexture2;
    Texture2DArrayPtr TempTextureArray;
    int ShowMode;
    int RSMFaceIndex;

private:
    GLint mShowModeLoc;
	GLint mTempSamplerLoc;
    GLint mTempSampler2Loc;
    GLint mTempSamplerArrayLoc;
    GLint mRSMFaceIndexLoc;
};

typedef RefPointer<VPLTempScreenQuad> VPLTempScreenQuadPtr;

}

#endif