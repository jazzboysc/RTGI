#ifndef RTGI_TempScreenQuad_H
#define RTGI_TempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "Texture2DArray.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class TempScreenQuad : public ScreenQuad
{
public:
    TempScreenQuad(Material* material);
    virtual ~TempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;
    Texture2DPtr TempTexture2;
    Texture2DArrayPtr TempTextureArray;
    int ShowMode;
    int TextureArrayIndex;

private:
    GLint mShowModeLoc;
	GLint mTempSamplerLoc;
    GLint mTempSampler2Loc;
    GLint mTempSamplerArrayLoc;
    GLint mTextureArrayIndexLoc;
};

typedef RefPointer<TempScreenQuad> TempScreenQuadPtr;

}

#endif