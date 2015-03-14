#pragma once

namespace RTGI
{
#define RTGI_CausticsBuffer_RefractorFrontNormal_Name   "RefractorFrontNormal"

struct RefractorResourceDesc
{
	int Width;
	int Height;
	BufferFormat RefractorFrontNormalFormat;
	bool RefractorFrontNormalMipmap;

	RefractorResourceDesc()
	{
		Width = 0;
		Height = 0;
		RefractorFrontNormalFormat = BF_Unknown;
		RefractorFrontNormalMipmap = false;
	}
};
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class RefractorResourceRenderer : public SubRenderer
{
public:
	RefractorResourceRenderer(GPUDevice* device, RenderSet* refractorSet = 0);
	virtual ~RefractorResourceRenderer();

	void CreateCausticsResource(RefractorResourceDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;
};

typedef RefPointer<RefractorResourceRenderer> RefractorResourceRendererPtr;

}
