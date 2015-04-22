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

struct ACMIndirectCommandBuffer
{
	uint num_groups_x;
	uint num_groups_y;
	uint num_groups_z;
};

const int GMaxTraversalLevel = 10;

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
	Texture2DPtr mComputeTempTexture;
	Texture2DArrayPtr mRefractorNormalTextures;
	Texture2DArrayPtr mRefractorDepthTextures;
	Texture2DPtr mReceiverDepthTexture;

	// pass 2
	int TraversalLevel;
	float RefractionIndex;
	float splatResolutionModifier;
	float renderBufRes;
	Camera* mCamera;
	ShaderUniform splatResolutionModifierLoc;
	ShaderUniform renderBufResLoc;
	ShaderUniform lightProjLoc;

	ShaderUniform TanLightFovy2Loc;
	ShaderUniform NearFarInfoLoc;
	ShaderUniform RefractionIndexInfoLoc;
	ShaderUniform RefractorNormTexLoc;
	ShaderUniform RefractorDepthTexLoc;
	ShaderUniform ReceiverDepthTexLoc;


	AtomicCounterBufferPtr mAtomicCounterBuffer;

	StructuredBufferPtr mACMBuffer;
	StructuredBufferPtr mACMSharedCommandBuffer;
	StructuredBufferPtr mACMIndirectCommandBuffer;
	BufferViewPtr mACMIndirectCommandBufferView;
};

typedef RefPointer<AdaptiveCausticsTaskInfo> AdaptiveCausticsTaskInfoPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class CopyTextureScreenQuad;
class CausticMapRenderer : public SubRenderer
{
public:
	CausticMapRenderer(GPUDevice* device, RenderSet* receiverSet = 0);
	virtual ~CausticMapRenderer();



	void Initialize(GPUDevice* device,
		CausticsMapDesc* desc,
		ReceiverResourceRenderer* receiverResourceRenderer,
		RefractorResourceRenderer* refractorResourceRenderer,
		Camera* light);

	void InitializeMinCausticHierarchy(GPUDevice* pDevice, AdaptiveCausticsTaskInfo* pTask, int resolution);
	void CreateCausticsResource(CausticsMapDesc* desc);
	void Render(int technique, int pass, Camera* camera);
	void DoTraversal();
	void DoSplat();
	Texture2DPtr mCompTexture;
	Texture2DPtr mCausticsTexture;
	FrameBufferPtr mFBOComputeTemp;
	FrameBufferPtr mFBOCaustics;

	int DebugMipmapLevel;
	bool DrawDebugMipmap;
	int TraversalLevel;

private:
	PipelineStateBlockPtr mPSB;

	CausticsPointSetPtr mPoints;

	AdaptiveCausticsTaskInfo* mCausticsTask;
	AdaptiveCausticsTaskInfo* mDebugDrawTask;
	AdaptiveCausticsTaskInfo* mSplatTask;
	CopyTextureScreenQuad* mCopyTextureScreenQuad;

	TraversalBufferData tbd;
	Texture2DPtr mReceiverPositionTexture;
	Texture2DPtr mReceiverDepthTexture;
	Texture2DArrayPtr mRefractorNormalTextures;
	Texture2DArrayPtr mRefractorDepthTextures;
	GLuint mPrimCountQuery;
};

typedef RefPointer<CausticMapRenderer> CausticsMapRendererPtr;

}
