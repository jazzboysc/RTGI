#ifndef RTGI_OITScreenQuad_H
#define RTGI_OITScreenQuad_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/23/2014
//----------------------------------------------------------------------------
class OITScreenQuad : public ScreenQuad
{
public:
	OITScreenQuad(Material* material);
	~OITScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr HeadPointerTexture;
	TextureBufferPtr GPUMemPool;
};

typedef RefPointer<OITScreenQuad> OITScreenQuadPtr;

}

#endif