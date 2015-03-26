#pragma once

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
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class CausticsMapRenderer : public SubRenderer
{
public:
	CausticsMapRenderer(GPUDevice* device, RenderSet* receiverSet = 0);
	virtual ~CausticsMapRenderer();

	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<CausticsMapRenderer> CausticsMapRendererPtr;

}
