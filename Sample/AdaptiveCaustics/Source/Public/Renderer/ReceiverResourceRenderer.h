#pragma once

namespace RTGI
{
#define RTGI_CausticsBuffer_ReceiverPosition_Name "ReceiverPosition"

struct ReceiverResourceDesc
{
	int Width;
	int Height;
	BufferFormat ReceiverPositionFormat;
	bool ReceiverPositionMipmap;

	ReceiverResourceDesc()
	{
		Width = 0;
		Height = 0;
		ReceiverPositionFormat = BF_Unknown;
		ReceiverPositionMipmap = false;
	}
};
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class ReceiverResourceRenderer : public SubRenderer
{
public:
	ReceiverResourceRenderer(GPUDevice* device, RenderSet* receiverSet = 0);
	virtual ~ReceiverResourceRenderer();

	void CreateCausticsResource(ReceiverResourceDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<ReceiverResourceRenderer> ReceiverResourceRendererPtr;

}
