#ifndef RTGI_VPLviaRSMApp_H
#define RTGI_VPLviaRSMApp_H

#include "GraphicsFrameworkHeader.h"
#include "VPLviaRSMTriMesh.h"
#include "VPLviaRSMDirectLightingQuad.h"
#include "VPLviaRSMTempScreenQuad.h"
#include "VPLviaRSMFinalScreenQuad.h"

namespace RTGI
{

struct VPL
{
    vec3 R;
    vec3 U;
    vec3 D;
    vec3 E;
    vec3 Flux;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2014
//----------------------------------------------------------------------------
class VPLviaRSMApp : public Application
{
public:
	VPLviaRSMApp(int width, int height);
	~VPLviaRSMApp();

	void Initialize();
	void Run();
	void Terminate();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

private:
    enum ShowMode
    {
        SM_Shadow,
        SM_RSMPosition,
        SM_RSMNormal,
        SM_RSMFlux,
        SM_DirectLighting,
        SM_VPLShadow,
        SM_IndirectLighting,
        SM_Final
    };

    bool mIsWireframe;
    ShowMode mShowMode;

	void ShadowPass();
    void RSMPass();
    void GBufferPass();
    void VPLShadowPass(const VPL& vpl);
    void DirectLightingPass();
    void IndirectLightingPass(const VPL& vpl);

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;
    Camera* mLightProjector;

    // Scene light shadow map.
    FrameBufferPtr mShadowMapFB;
    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;

    // RSM-buffer.
    int mRSMWidth, mRSMHeight;
    FrameBufferPtr mRSMBufferPX;
    Texture2DPtr mRSMPositionTexturePX;
    Texture2DPtr mRSMNormalTexturePX;
    Texture2DPtr mRSMFluxTexturePX;
    Texture2DPtr mRSMDepthTexturePX;
    float* mRSMPositionData;
    float* mRSMNormalData;
    float* mRSMFluxData;

    enum { VPLSampleCount = 4 };
    float mRSMSamplePos[VPLSampleCount * 2];
    VPL mVPLs[VPLSampleCount];

    // G-buffer.
    FrameBufferPtr mGBuffer;
    Texture2DPtr mGBufferPositionTexture;
    Texture2DPtr mGBufferNormalTexture;
    Texture2DPtr mGBufferDiffuseTexture;
    Texture2DPtr mGBufferDepthTexture;

    // VPL shadow map.
    FrameBufferPtr mVPLShadowMapFB;
    Texture2DPtr mVPLShadowMapTexture;
    Texture2DPtr mVPLShadowMapDepthTexture;

    // Direct lighting buffer.
    FrameBufferPtr mDirectLightingFB;
    Texture2DPtr mDirectLightingTexture;
    Texture2DPtr mDirectLightingDepthTexture;

    // Indirect lighting buffer.
    FrameBufferPtr mIndirectLightingFB[2];
    Texture2DPtr mIndirectLightingTexture[2];
    Texture2DPtr mIndirectLightingDepthTexture[2];

    Texture1DPtr mRSMSampleTexture;

    VPLviaRSMDirectLightingQuadPtr mDirectLightingQuad;
    VPLviaRSMTempScreenQuadPtr mTempResultScreenQuad;
    VPLviaRSMFinalScreenQuadPtr mFinalResultScreenQuad;

	VPLviaRSMTriMeshPtr mGround;
	VPLviaRSMTriMeshPtr mCeiling;
	VPLviaRSMTriMeshPtr mBackWall;
	VPLviaRSMTriMeshPtr mLeftWall;
	VPLviaRSMTriMeshPtr mRightWall;
	VPLviaRSMTriMeshPtr mModel;
};

}

#endif