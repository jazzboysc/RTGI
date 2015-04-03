#pragma once
#include "VertexBuffer.h"
#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"
#include "AdaptiveCausticsPointSet.h"

namespace RTGI
{
#define RTGI_CausticsBuffer_CausticMap_Name "CausticMap"

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
	VertexBufferPtr genericTraversalBuffers[5];
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class CausticMapRenderer : public SubRenderer
{
public:
	CausticMapRenderer(GPUDevice* device, RenderSet* receiverSet = 0);
	virtual ~CausticMapRenderer();



	void Initialize(GPUDevice* device,
		ReceiverResourceRenderer* receiverResourceRenderer,
		RefractorResourceRenderer* refractorResourceRenderer,
		Camera* mainCamera);

	void InitializeMinCausticHierarchy(GPUDevice* pDevice, VertexBuffer* pVB, int resolution);
	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);

private:
	PipelineStateBlockPtr mPSB;

	CausticsPointSetPtr mPoints;


	TraversalBufferData tbd;
	Texture2DPtr mReceiverPositionTexture;
	Texture2DArrayPtr mRefractorFrontAndBackNormalTextures;
	const int mMaxTraversalLevel = 10;
	GLuint mPrimCountQuery;
};

typedef RefPointer<CausticMapRenderer> CausticsMapRendererPtr;

}
