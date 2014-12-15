#ifndef RTGI_SIIApp_H
#define RTGI_SIIApp_H

#include "SIITriMesh.h"
#include "SIITempScreenQuad.h"
#include "SIIVPLQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/12/2014
//----------------------------------------------------------------------------
class SIIApp : public Application
{
public:
	SIIApp(int width = 1024, int height = 768);
	~SIIApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
    enum ShowMode
    {
        SM_Shadow,
        SM_RSMPosition,
        SM_RSMNormal,
        SM_RSMFlux,
        SM_Scene
    };

    bool mIsWireframe;
    ShowMode mShowMode;

	void DrawShadow();
    void DrawRSMs();
    void SampleRSMs();
    void DrawScene();

    Camera* mLightProjector;

    FrameBufferPtr mShadowMapFB;
    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;

    // RSM-buffer.
    FrameBufferPtr mRSMBufferPX;
    Texture2DPtr mRSMPositionTexturePX;
    Texture2DPtr mRSMNormalTexturePX;
    Texture2DPtr mRSMFluxTexturePX;
    Texture2DPtr mRSMDepthTexturePX;

    Texture1DPtr mRSMSampleTexture;

    // VPL quad.
    SIIVPLQuadPtr mVPLQuad;

    SIITempScreenQuadPtr mShadowMapScreenQuad;

	SIITriMeshPtr mGround;
	SIITriMeshPtr mCeiling;
	SIITriMeshPtr mBackWall;
	SIITriMeshPtr mLeftWall;
	SIITriMeshPtr mRightWall;
	SIITriMeshPtr mModel;
};

}

#endif