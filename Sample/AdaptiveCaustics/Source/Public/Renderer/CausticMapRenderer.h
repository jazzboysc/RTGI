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
struct ACMBuffer
{
	vec4 something;
};

struct ACMAtomicCounter
{
	uint writeCount;
	uint temp;
};

struct ACMUniformBuffer
{
	uint mipmapLevel; // 2^6 to start with
	uint readOffset;
	uint readCount;
	uint writeOffset;
	uint width;
	uint height;
	float deltaX;
	float deltaY;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/26/2015
//----------------------------------------------------------------------------
class AdaptiveCausticsTraversalInfo : public ComputeTask
{
public:
	AdaptiveCausticsTraversalInfo();
	~AdaptiveCausticsTraversalInfo();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnPreDispatch(unsigned int pass);
	virtual void OnPostDispatch(unsigned int pass);

	ACMUniformBuffer UniformCache;
	ACMAtomicCounter AtomicCounterCache;

	ShaderUniform ImageUnit0Loc;
	ShaderUniform ImageUnit1Loc;
	Texture2DPtr mCompTexture;
	Texture2DArrayPtr mNormalTextures;

	AtomicCounterBufferPtr mAtomicCounterBuffer;

	StructuredBufferPtr mACMBuffer;
	UniformBufferPtr mACMUniformBuffer;
};

typedef RefPointer<AdaptiveCausticsTraversalInfo> GatherVoxelFragmentListInfoPtr;

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

	void InitializeMinCausticHierarchy(GPUDevice* pDevice, AdaptiveCausticsTraversalInfo* pTask, int resolution);
	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);

	Texture2DPtr mCompTexture;
	FrameBufferPtr mFBOClear;

private:
	PipelineStateBlockPtr mPSB;

	CausticsPointSetPtr mPoints;

	AdaptiveCausticsTraversalInfo* mTraversalTask;

	TraversalBufferData tbd;
	Texture2DPtr mReceiverPositionTexture;
	Texture2DArrayPtr mRefractorFrontAndBackNormalTextures;
	const int mMaxTraversalLevel = 10;
	GLuint mPrimCountQuery;
};

typedef RefPointer<CausticMapRenderer> CausticsMapRendererPtr;

}
