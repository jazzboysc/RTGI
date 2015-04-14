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

struct ACMSharedCommandBuffer
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
class AdaptiveCausticsTaskInfo : public ComputeTask
{
public:
	AdaptiveCausticsTaskInfo();
	~AdaptiveCausticsTaskInfo();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnPreDispatch(unsigned int pass);
	virtual void OnPostDispatch(unsigned int pass);

	ACMSharedCommandBuffer UniformCache;
	ACMAtomicCounter AtomicCounterCache;

	ShaderUniform ImageUnit0Loc;
	ShaderUniform ImageUnit1Loc;
	Texture2DPtr mCompTexture;
	Texture2DArrayPtr mNormalTextures;

	AtomicCounterBufferPtr mAtomicCounterBuffer;

	StructuredBufferPtr mACMBuffer;
	StructuredBufferPtr mACMSharedCommandBuffer;
};

typedef RefPointer<AdaptiveCausticsTaskInfo> GatherVoxelFragmentListInfoPtr;

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

	void InitializeMinCausticHierarchy(GPUDevice* pDevice, AdaptiveCausticsTaskInfo* pTask, int resolution);
	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);

	Texture2DPtr mCompTexture;
	FrameBufferPtr mFBOClear;

	int DebugMipmapLevel;
	bool DrawDebugMipmap;
	int TraversalLevel;

private:
	PipelineStateBlockPtr mPSB;

	CausticsPointSetPtr mPoints;

	AdaptiveCausticsTaskInfo* mTraversalTask;
	AdaptiveCausticsTaskInfo* mDebugDrawTask;
	
	TraversalBufferData tbd;
	Texture2DPtr mReceiverPositionTexture;
	Texture2DArrayPtr mRefractorFrontAndBackNormalTextures;
	const int mMaxTraversalLevel = 10;
	GLuint mPrimCountQuery;
};

typedef RefPointer<CausticMapRenderer> CausticsMapRendererPtr;

}
