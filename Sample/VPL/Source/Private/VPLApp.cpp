#include "VPLApp.h"
#include "InformationPanel.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

//----------------------------------------------------------------------------
VPLApp::VPLApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "VPL demo";
    mShowMode = SM_Shadow;
    mIsRotatingModel = false;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
VPLApp::~VPLApp()
{
}
//----------------------------------------------------------------------------
void VPLApp::Initialize(GPUDevice* device)
{
	float color = 0.5f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 0.01f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 33.2f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light projector.
    mLightProjector = new Camera();
    mLightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    //mLightProjector->SetLookAt(vec3(-10.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    //    vec3(0.0f, 1.0f, 0.0f));
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;

    ShaderProgramInfo shadowProgramInfo;
    shadowProgramInfo.VShaderFileName = "VPL/vShadow.glsl";
    shadowProgramInfo.FShaderFileName = "VPL/fShadow.glsl";
    shadowProgramInfo.TCShaderFileName = "VPL/tcShadow.glsl";
    shadowProgramInfo.TEShaderFileName = "VPL/teShadow.glsl";
    shadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                        ShaderType::ST_Fragment | 
                                        ShaderType::ST_TessellationControl |
                                        ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(shadowProgramInfo);

    ShaderProgramInfo gbufferProgramInfo;
    gbufferProgramInfo.VShaderFileName = "VPL/vGBuffer.glsl";
    gbufferProgramInfo.FShaderFileName = "VPL/fGBuffer.glsl";
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);

    ShaderProgramInfo rsmProgramInfo;
    rsmProgramInfo.VShaderFileName = "VPL/vRSM.glsl";
    rsmProgramInfo.GShaderFileName = "VPL/gRSM.glsl";
    rsmProgramInfo.FShaderFileName = "VPL/fRSM.glsl";
    rsmProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                     ShaderType::ST_Geometry |
                                     ShaderType::ST_Fragment;
    Pass* passRSM = new Pass(rsmProgramInfo);

    ShaderProgramInfo vplShadowProgramInfo;
    vplShadowProgramInfo.VShaderFileName = "VPL/vVPLShadow.glsl";
    vplShadowProgramInfo.FShaderFileName = "VPL/fVPLShadow.glsl";
    vplShadowProgramInfo.TCShaderFileName = "VPL/tcVPLShadow.glsl";
    vplShadowProgramInfo.TEShaderFileName = "VPL/teVPLShadow.glsl";
    vplShadowProgramInfo.GShaderFileName = "VPL/gVPLShadow.glsl";
    vplShadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                           ShaderType::ST_Fragment |
                                           ShaderType::ST_TessellationControl |
                                           ShaderType::ST_TessellationEvaluation |
                                           ShaderType::ST_Geometry;
    Pass* passVPLShadow = new Pass(vplShadowProgramInfo);

	Technique* techVPL = new Technique();
    techVPL->AddPass(passShadow);
    techVPL->AddPass(passGBuffer);
    techVPL->AddPass(passRSM);
    techVPL->AddPass(passVPLShadow);
	MaterialTemplate* mtVPL = new MaterialTemplate();
	mtVPL->AddTechnique(techVPL);

    ShaderProgramInfo vplTempProgramInfo;
    vplTempProgramInfo.VShaderFileName = "VPL/vVPLTemp.glsl";
    vplTempProgramInfo.FShaderFileName = "VPL/fVPLTemp.glsl";
    vplTempProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(vplTempProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "VPL/vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "VPL/fDirectLighting.glsl";
    directLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(directLightingProgramInfo);

    Technique* techDirectLighting = new Technique();
    techDirectLighting->AddPass(passDirectLighting);
    MaterialTemplate* mtDirectLighting = new MaterialTemplate();
    mtDirectLighting->AddTechnique(techDirectLighting);

    ShaderProgramInfo indirectLightingProgramInfo;
    indirectLightingProgramInfo.VShaderFileName = "VPL/vIndirectLighting.glsl";
    indirectLightingProgramInfo.FShaderFileName = "VPL/fIndirectLighting.glsl";
    indirectLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passIndirectLighting = new Pass(indirectLightingProgramInfo);

    Technique* techIndirectLighting = new Technique();
    techIndirectLighting->AddPass(passIndirectLighting);
    MaterialTemplate* mtIndirectLighting = new MaterialTemplate();
    mtIndirectLighting->AddTechnique(techIndirectLighting);

    // Create G-buffer textures.
    mGBufferPositionTexture = new Texture2D();
    mGBufferPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBAF);
    mGBufferNormalTexture = new Texture2D();
    mGBufferNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBAF);
    mGBufferAlbedoTexture = new Texture2D();
    mGBufferAlbedoTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBAF);
    mGBufferDepthTexture = new Texture2D();
    mGBufferDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);

    // Create G-buffer.
    Texture* gbufferTextures[3] = { mGBufferPositionTexture, mGBufferNormalTexture, mGBufferAlbedoTexture };
    mGBufferFB = new FrameBuffer(mDevice);
    mGBufferFB->SetRenderTargets(3, gbufferTextures, mGBufferDepthTexture);

    // Create shadow map render target.
    int shadowMapWidth, shadowMapHeight;
    shadowMapWidth = 512;
    shadowMapHeight = 512;
    mShadowMapTexture = new Texture2D();
    mShadowMapTexture->CreateRenderTarget(mDevice, shadowMapWidth, shadowMapHeight, 
        BF_RGBAF);

    mShadowMapDepthTexture = new Texture2D();
    mShadowMapDepthTexture->CreateRenderTarget(mDevice, shadowMapWidth, shadowMapHeight, 
        BF_Depth);

    // Create shadow map frame buffer.
    Texture* renderTargets[] = { mShadowMapTexture };
    mShadowMapFB = new FrameBuffer(mDevice);
    mShadowMapFB->SetRenderTargets(1, renderTargets, mShadowMapDepthTexture);

    // Create direct lighting render target.
    mDirectLightingTexture = new Texture2D();
    mDirectLightingTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBAF);

    mDirectLightingDepthTexture = new Texture2D();
    mDirectLightingDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);

    // Create direct lighting frame buffer.
    Texture* dlRenderTargets[] = { mDirectLightingTexture };
    mDirectLightingFB = new FrameBuffer(mDevice);
    mDirectLightingFB->SetRenderTargets(1, dlRenderTargets, mDirectLightingDepthTexture);

    // Create indirect lighting render target.
    mIndirectLightingTexture = new Texture2D();
    mIndirectLightingTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBAF);

    mIndirectLightingDepthTexture = new Texture2D();
    mIndirectLightingDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);

    // Create indirect lighting frame buffer.
    Texture* indlRenderTargets[] = { mIndirectLightingTexture };
    mIndirectLightingFB = new FrameBuffer(mDevice);
    mIndirectLightingFB->SetRenderTargets(1, indlRenderTargets, mIndirectLightingDepthTexture);

    // Create RSM render targets.
    int rsmWidth, rsmHeight;
    rsmWidth = 256;
    rsmHeight = 256;
    mRSMPositionTextureArray = new Texture2DArray();
    mRSMPositionTextureArray->CreateRenderTarget(mDevice, rsmWidth, rsmHeight, 5, BF_RGBAF);
    mRSMNormalTextureArray = new Texture2DArray();
    mRSMNormalTextureArray->CreateRenderTarget(mDevice, rsmWidth, rsmHeight, 5, BF_RGBAF);
    mRSMFluxTextureArray = new Texture2DArray();
    mRSMFluxTextureArray->CreateRenderTarget(mDevice, rsmWidth, rsmHeight, 5, BF_RGBAF);
    mRSMDepthTextureArray = new Texture2DArray();
    mRSMDepthTextureArray->CreateRenderTarget(mDevice, rsmWidth, rsmHeight, 5, BF_Depth);

    // Create RSM frame buffer.
    Texture* rsmRenderTargets[] = { mRSMPositionTextureArray, mRSMNormalTextureArray, mRSMFluxTextureArray };
    mRSMFB = new FrameBuffer(mDevice);
    mRSMFB->SetRenderTargets(3, rsmRenderTargets, mRSMDepthTextureArray);

    // Create VPL shadow maps render target.
    int vplSMWidth, vplSMHeight;
    vplSMWidth = 256;
    vplSMHeight = 256;
    mVPLShadowMapTextureArray = new Texture2DArray();
    mVPLShadowMapTextureArray->CreateRenderTarget(mDevice, vplSMWidth, vplSMHeight, VPL_SAMPLE_COUNT, BF_RGBAF);
    mVPLShadowMapDepthTextureArray = new Texture2DArray();
    mVPLShadowMapDepthTextureArray->CreateRenderTarget(mDevice, vplSMWidth, vplSMHeight, VPL_SAMPLE_COUNT, BF_Depth);

    // Create VPL shaodw maps frame buffer.
    Texture* vplSMRenderTargets[] = { mVPLShadowMapTextureArray };
    mVPLShadowMapFB = new FrameBuffer(mDevice);
    mVPLShadowMapFB->SetRenderTargets(1, vplSMRenderTargets, mVPLShadowMapDepthTextureArray);

    // Create VPL sample pattern.
    mVPLSamplePattern = new Texture1D();
    mVPLSamplePattern->CreateUniformRandomTexture(mDevice, VPL_SAMPLE_COUNT, 4);
    mVPLSampleTest = new Texture1D();
    mVPLSampleTest->LoadFromSystemMemory(mDevice, BIF_RGBA32F, VPL_SAMPLE_COUNT, BF_RGBA, BCT_Float, 0);

    // Create VPL buffer.
    GLuint vplBufferSize = (sizeof(vec4)* 3 + sizeof(mat4)) * VPL_SAMPLE_COUNT;
    mVPLBuffer = new StructuredBuffer();
    mVPLBuffer->ReserveMutableDeviceResource(mDevice, vplBufferSize, BU_Dynamic_Copy);

    // Create VPL sample compute tasks.
    ShaderProgramInfo sampleRSMProgramInfo;
    sampleRSMProgramInfo.CShaderFileName = "VPL/cSampleRSM.glsl";
    sampleRSMProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passSampleRSM = new ComputePass(sampleRSMProgramInfo);
    mSampleRSMTask = new VPLSampleRSM();
    mSampleRSMTask->AddPass(passSampleRSM);
    mSampleRSMTask->CreateDeviceResource(mDevice);
    mSampleRSMTask->VPLSamplePattern = mVPLSamplePattern;
    mSampleRSMTask->VPLSampleTest = mVPLSampleTest;
    mSampleRSMTask->RSMPosition = mRSMPositionTextureArray;
    mSampleRSMTask->RSMNormal = mRSMNormalTextureArray;
    mSampleRSMTask->RSMFlux = mRSMFluxTextureArray;
    mSampleRSMTask->VPLBuffer = mVPLBuffer;

	// Create scene.
	mat4 rotM;
	material = new Material(mtVPL);
	mModel = new VPLTriMesh(material, mMainCamera);
	mModel->LoadFromPLYFile("dragon_s.ply");
    mat4 scale = glm::scale(mat4(), vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 3.0f));
	mModel->MaterialColor = vec3(1.8f, 1.8f, 1.8f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;
    mModel->VPLBuffer = mVPLBuffer;

    material = new Material(mtVPL);
	mGround = new VPLTriMesh(material, mMainCamera);
	mGround->LoadFromPLYFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;
    mGround->VPLBuffer = mVPLBuffer;

    material = new Material(mtVPL);
	mCeiling = new VPLTriMesh(material, mMainCamera);
	mCeiling->LoadFromPLYFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;
    mCeiling->VPLBuffer = mVPLBuffer;

    material = new Material(mtVPL);
	mBackWall = new VPLTriMesh(material, mMainCamera);
	mBackWall->LoadFromPLYFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;
    mBackWall->VPLBuffer = mVPLBuffer;

    material = new Material(mtVPL);
	mLeftWall = new VPLTriMesh(material, mMainCamera);
	mLeftWall->LoadFromPLYFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.2f, 0.2f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;
    mLeftWall->VPLBuffer = mVPLBuffer;

    material = new Material(mtVPL);
	mRightWall = new VPLTriMesh(material, mMainCamera);
	mRightWall->LoadFromPLYFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.2f, 1.0f, 0.2f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;
    mRightWall->VPLBuffer = mVPLBuffer;

    // Create screen quads.
    material = new Material(mtScreenQuad);
    mTempScreenQuad = new VPLTempScreenQuad(material);
    mTempScreenQuad->LoadFromPLYFile("screenquad.ply");
    mTempScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mTempScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mTempScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mTempScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mTempScreenQuad->CreateDeviceResource(mDevice);
    mTempScreenQuad->TempTexture = mShadowMapTexture;
    mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;

    material = new Material(mtDirectLighting);
    mDirectLightingScreenQuad = new VPLDirectLightingScreenQuad(material);
    mDirectLightingScreenQuad->LoadFromPLYFile("screenquad.ply");
    mDirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mDirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mDirectLightingScreenQuad->CreateDeviceResource(mDevice);
    mDirectLightingScreenQuad->GBufferPositionTexture = mGBufferPositionTexture;
    mDirectLightingScreenQuad->GBufferNormalTexture = mGBufferNormalTexture;
    mDirectLightingScreenQuad->GBufferAlbedoTexture = mGBufferAlbedoTexture;
    mDirectLightingScreenQuad->ShadowMapTexture = mShadowMapTexture;
    mDirectLightingScreenQuad->LightProjector = mLightProjector;

    material = new Material(mtIndirectLighting);
    mIndirectLightingScreenQuad = new VPLIndirectLightingScreenQuad(material);
    mIndirectLightingScreenQuad->LoadFromPLYFile("screenquad.ply");
    mIndirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mIndirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mIndirectLightingScreenQuad->CreateDeviceResource(mDevice);
    mIndirectLightingScreenQuad->VPLCount = VPL_SAMPLE_COUNT;
    mIndirectLightingScreenQuad->GBufferPositionTexture = mGBufferPositionTexture;
    mIndirectLightingScreenQuad->GBufferNormalTexture = mGBufferNormalTexture;
    mIndirectLightingScreenQuad->GBufferAlbedoTexture = mGBufferAlbedoTexture;
    mIndirectLightingScreenQuad->VPLBuffer = mVPLBuffer;

    // Create GPU timer.
    mTimer = new GPUTimer();
    mTimer->CreateDeviceResource();

	// Create information panel.
	int screenX, screenY;
	glfwGetWindowPos(Window, &screenX, &screenY);
	InformationPanel^ infoPanel = gcnew InformationPanel();
	infoPanel->Show();
	infoPanel->SetDesktopLocation(screenX + Width + 12, screenY - 30);

    // Create labels.
    InformationPanel::GetInstance()->AddTimingLabel("Scene Shadow Pass", 16, 20);
    InformationPanel::GetInstance()->AddTimingLabel("Scene G-buffer Pass", 16, 40);
    InformationPanel::GetInstance()->AddTimingLabel("RSM Pass", 16, 60);
    InformationPanel::GetInstance()->AddTimingLabel("VPL Creation Pass", 16, 80);
    InformationPanel::GetInstance()->AddTimingLabel("Direct Lighting Pass", 16, 100);
    InformationPanel::GetInstance()->AddTimingLabel("Indirect Lighting Pass", 16, 120);
}
//----------------------------------------------------------------------------
void VPLApp::ShadowPass()
{
    mGround->SetCamera(mLightProjector);
    mCeiling->SetCamera(mLightProjector);
    mBackWall->SetCamera(mLightProjector);
    mLeftWall->SetCamera(mLightProjector);
    mRightWall->SetCamera(mLightProjector);
    mModel->SetCamera(mLightProjector);

	mGround->Render(0, 0);
	mCeiling->Render(0, 0);
	mBackWall->Render(0, 0);
	mLeftWall->Render(0, 0);
	mRightWall->Render(0, 0);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void VPLApp::GBufferPass()
{
    mGround->SetCamera(mMainCamera);
    mCeiling->SetCamera(mMainCamera);
    mBackWall->SetCamera(mMainCamera);
    mLeftWall->SetCamera(mMainCamera);
    mRightWall->SetCamera(mMainCamera);
    mModel->SetCamera(mMainCamera);

    mGround->Render(0, 1);
    mCeiling->Render(0, 1);
    mBackWall->Render(0, 1);
    mLeftWall->Render(0, 1);
    mRightWall->Render(0, 1);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void VPLApp::RSMPass()
{
    mGround->Render(0, 2);
    mCeiling->Render(0, 2);
    mBackWall->Render(0, 2);
    mLeftWall->Render(0, 2);
    mRightWall->Render(0, 2);
    mModel->Render(0, 2);
}
//----------------------------------------------------------------------------
void VPLApp::VPLShadowPass()
{
    mGround->Render(0, 3);
    mCeiling->Render(0, 3);
    mBackWall->Render(0, 3);
    mLeftWall->Render(0, 3);
    mRightWall->Render(0, 3);
    mModel->Render(0, 3);
}
//----------------------------------------------------------------------------
void VPLApp::FrameFunc()
{
    static float angle = 0.0f;
    if( mIsRotatingModel )
    {
        angle += 1.0f;
        mat4 rot;
        rot = rotate(mat4(), radians(angle), vec3(0, 1, 0));
        vec3 trans = mModel->GetWorldTranslation();
        mModel->SetWorldTransform(rot);
        mModel->SetWorldTranslation(trans);
    }

    if( mIsWireframe )
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    InformationPanel^ infoPanel = InformationPanel::GetInstance();
    static double workLoad = 0.0;

    // Scene shadow pass.
    mTimer->Start();
    mShadowMapFB->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShadowPass();
    mShadowMapFB->Disable();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene Shadow Pass", workLoad);

    // Scene G-buffer pass.
    mTimer->Start();
    mGBufferFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GBufferPass();
    mGBufferFB->Disable();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene G-buffer Pass", workLoad);

    // Scene light RSM pass.
    mTimer->Start();
    mRSMFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RSMPass();
    mRSMFB->Disable();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("RSM Pass", workLoad);

    // Sample RSM pass (VPL generation).
    mTimer->Start();
    mSampleRSMTask->DispatchCompute(0, VPL_SAMPLE_COUNT, 1, 1);
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("VPL Creation Pass", workLoad);

    // VPL shadow pass.
    //mVPLShadowMapFB->Enable();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //VPLShadowPass();
    //mVPLShadowMapFB->Disable();

    // Deferred direct illumination pass.
    mTimer->Start();
    mDirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mDirectLightingScreenQuad->Render(0, 0);
    mDirectLightingFB->Disable();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Direct Lighting Pass", workLoad);

    // Deferred indirect illumination pass.
    mTimer->Start();
    mIndirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mIndirectLightingScreenQuad->Render(0, 0);
    mIndirectLightingFB->Disable();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Indirect Lighting Pass", workLoad);

    // Show rendering result.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mTempScreenQuad->Render(0, 0);
}
//----------------------------------------------------------------------------
void VPLApp::Terminate()
{
	// Release all resources.

    delete mLightProjector;

    mGBufferFB = 0;
    mGBufferPositionTexture = 0;
    mGBufferNormalTexture = 0;
    mGBufferAlbedoTexture = 0;
    mGBufferDepthTexture = 0;

    mShadowMapFB = 0;
    mShadowMapTexture = 0;
    mShadowMapDepthTexture = 0;

    mDirectLightingFB = 0;
    mDirectLightingTexture = 0;
    mDirectLightingDepthTexture = 0;

    mIndirectLightingFB = 0;
    mIndirectLightingTexture = 0;
    mIndirectLightingDepthTexture = 0;

    mRSMFB = 0;
    mRSMPositionTextureArray = 0;
    mRSMNormalTextureArray = 0;
    mRSMFluxTextureArray = 0;
    mRSMDepthTextureArray = 0;

    mVPLShadowMapFB = 0;
    mVPLShadowMapTextureArray = 0;
    mVPLShadowMapDepthTextureArray = 0;

    mSampleRSMTask = 0;
    mVPLSamplePattern = 0;
    mVPLSampleTest = 0;
    mVPLBuffer = 0;

    mTempScreenQuad = 0;
    mDirectLightingScreenQuad = 0;
    mIndirectLightingScreenQuad = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void VPLApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mShowMode = SM_Shadow;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mShadowMapTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mShowMode = SM_GBufferPosition;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mGBufferPositionTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mShowMode = SM_GBufferNormal;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mGBufferNormalTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
	{
		mShowMode = SM_GBufferAlbedo;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mGBufferAlbedoTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_5) == GLFW_PRESS)
	{
		mShowMode = SM_RSMPosition;
		mTempScreenQuad->ShowMode = 1;
		mTempScreenQuad->TempTextureArray = mRSMPositionTextureArray;
	}
	if (glfwGetKey(Window, GLFW_KEY_6) == GLFW_PRESS)
	{
		mShowMode = SM_RSMNormal;
		mTempScreenQuad->ShowMode = 1;
		mTempScreenQuad->TempTextureArray = mRSMNormalTextureArray;
	}
	if (glfwGetKey(Window, GLFW_KEY_7) == GLFW_PRESS)
	{
		mShowMode = SM_RSMFlux;
		mTempScreenQuad->ShowMode = 1;
		mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;
	}
	if (glfwGetKey(Window, GLFW_KEY_8) == GLFW_PRESS)
	{
		mShowMode = SM_DirectLighting;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mDirectLightingTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_9) == GLFW_PRESS)
	{
		mShowMode = SM_IndirectLighting;
		mTempScreenQuad->ShowMode = 0;
		mTempScreenQuad->TempTexture = mIndirectLightingTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_0) == GLFW_PRESS)
	{
		mShowMode = SM_Final;
		mTempScreenQuad->ShowMode = 2;
		mTempScreenQuad->TempTexture = mIndirectLightingTexture;
		mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mIsRotatingModel = !mIsRotatingModel;
	}
	if (glfwGetKey(Window, GLFW_KEY_T) == GLFW_PRESS)
	{
		mIsWireframe = !mIsWireframe;
	}
	if (glfwGetKey(Window, GLFW_KEY_V) == GLFW_PRESS)
	{
		mShowMode = SM_VPLShadow;
		mTempScreenQuad->ShowMode = 3;
		mTempScreenQuad->TempTextureArray = mVPLShadowMapTextureArray;
	}
	if (glfwGetKey(Window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		if (mShowMode == SM_VPLShadow)
		{
			mTempScreenQuad->TextureArrayIndex++;
			mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
		}
	}
	if (glfwGetKey(Window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		if (mShowMode == SM_VPLShadow)
		{
			mTempScreenQuad->TextureArrayIndex--;
			mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
		}
	}

	bool isShift = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
				   glfwGetKey(Window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

	if (isShift ^ glfwGetKey(Window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
	{
		// Uppercase
		if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 0;
		}
		if (glfwGetKey(Window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 2;
		}
		if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS)
		{
		}
	}
	else
	{
		// Lowercase
		if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 1;
		}
		if (glfwGetKey(Window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 3;
		}
		if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 4;
		}
	}
}