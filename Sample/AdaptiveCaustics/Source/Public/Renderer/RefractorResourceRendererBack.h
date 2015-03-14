#pragma once

namespace RTGI
{
#define RTGI_CausticsBuffer_RefractorBackNormal_Name   "RefractorBackNormal"

struct RefractorResourceDescBack
{
	int Width;
	int Height;
	BufferFormat RefractorBackNormalFormat;
	bool RefractorBackNormalMipmap;

	RefractorResourceDescBack()
	{
		Width = 0;
		Height = 0;
		RefractorBackNormalFormat = BF_Unknown;
		RefractorBackNormalMipmap = false;
	}
};
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class RefractorResourceRendererBack : public SubRenderer
{
public:
	RefractorResourceRendererBack(GPUDevice* device, RenderSet* refractorSet = 0);
	virtual ~RefractorResourceRendererBack();

	void CreateCausticsResource(RefractorResourceDescBack* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<RefractorResourceRendererBack> RefractorResourceRendererBackPtr;

}
