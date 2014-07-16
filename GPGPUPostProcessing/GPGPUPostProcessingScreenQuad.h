#ifndef RTGI_GPGPUPostProcessingScreenQuad_H
#define RTGI_GPGPUPostProcessingScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/16/2014
//----------------------------------------------------------------------------
class GPGPUPostProcessingScreenQuad : public ScreenQuad
{
public:
	GPGPUPostProcessingScreenQuad(Material* material);
	virtual ~GPGPUPostProcessingScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr RenderTexture;

private:
	GLint mRenderSamplerLoc;
};

typedef RefPointer<GPGPUPostProcessingScreenQuad> GPGPUPostProcessingScreenQuadPtr;

}

#endif