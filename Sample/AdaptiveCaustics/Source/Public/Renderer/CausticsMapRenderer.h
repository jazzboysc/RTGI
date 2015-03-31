#pragma once
#include "VertexBuffer.h"

namespace RTGI
{
#define RTGI_CausticsBuffer_CausticsMap_Name "ReceiverPosition"

struct CausticsMapDesc
{
	int Width;
	int Height;
	BufferFormat CausticsMapFormat;
	bool CausticsMapMipmap;

	CausticsMapDesc()
	{
		Width = 0;
		Height = 0;
		CausticsMapFormat = BF_Unknown;
		CausticsMapMipmap = false;
	}
};

struct TraversalBufferData
{
	// Vertex buffers used when photons, photon buffers, or caustic splats.  
	//   Multiple buffers are used for various types of ping-ponging computation.
	VertexBufferPtr genericTraversalStartBuffer;
	VertexBufferPtr genericTraversalBuffer[5];
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class CausticsMapRenderer : public SubRenderer
{
public:
	CausticsMapRenderer(GPUDevice* device, RenderSet* receiverSet = 0);
	virtual ~CausticsMapRenderer();

	TraversalBufferData tbd;

	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<CausticsMapRenderer> CausticsMapRendererPtr;

}
