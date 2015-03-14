#pragma once

namespace RTGI
{
#define RTGI_CausticsBuffer_ReceiverPosition_Name "ReceiverPosition"
#define RTGI_CausticsBuffer_RefractorFrontNormal_Name   "RefractorFrontNormal"
#define RTGI_CausticsBuffer_RefractorBackNormal_Name   "RefractorBackNormal"
#define RTGI_CausticsBuffer_RefractorAlbedo_Name   "RefractorAlbedo"

struct CausticsResourceDesc
{
	int Width;
	int Height;
	BufferFormat ReceiverPositionFormat;
	BufferFormat RefractorFrontNormalFormat;
	BufferFormat RefractorBackNormalFormat;
	BufferFormat RefractorAlbedoFormat;
	bool ReceiverPositionMipmap;
	bool RefractorFrontNormalMipmap;
	bool RefractorBackNormalMipmap;
	bool RefractorAlbedoMipmap;

	CausticsResourceDesc()
	{
		Width = 0;
		Height = 0;
		ReceiverPositionFormat = BF_Unknown;
		RefractorFrontNormalFormat = BF_Unknown;
		RefractorBackNormalFormat = BF_Unknown;
		RefractorAlbedoFormat = BF_Unknown;
		ReceiverPositionMipmap = false;
		RefractorFrontNormalMipmap = false;
		RefractorBackNormalMipmap = false;
		RefractorAlbedoMipmap = false;
	}
};
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class CausticsResourceRenderer : public SubRenderer
{
public:
	CausticsResourceRenderer(GPUDevice* device, RenderSet* receiverSet, RenderSet* refractorSet = 0);
	virtual ~CausticsResourceRenderer();

	void CreateCausticsResource(CausticsResourceDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	RenderSet* mReceiverSet;
	RenderSet* mRefractorSet;
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<CausticsResourceRenderer> CausticsResourceRendererPtr;

}
