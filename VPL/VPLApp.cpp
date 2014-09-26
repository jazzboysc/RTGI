#include "VPLApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLApp::VPLApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("VPL demo")
{
    mShowMode = SM_Shadow;
    mIsRotatingModel = false;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
VPLApp::~VPLApp()
{
}
//----------------------------------------------------------------------------
void VPLApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 0.01f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 33.2f), vec3(0.0f, 10.0f, 0.0f),
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
    shadowProgramInfo.VShaderFileName = "vShadow.glsl";
    shadowProgramInfo.FShaderFileName = "fShadow.glsl";
    shadowProgramInfo.TCShaderFileName = "tcShadow.glsl";
    shadowProgramInfo.TEShaderFileName = "teShadow.glsl";
    shadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                        ShaderType::ST_Fragment | 
                                        ShaderType::ST_TessellationControl |
                                        ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(shadowProgramInfo);

    ShaderProgramInfo gbufferProgramInfo;
    gbufferProgramInfo.VShaderFileName = "vGBuffer.glsl";
    gbufferProgramInfo.FShaderFileName = "fGBuffer.glsl";
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);

    ShaderProgramInfo rsmProgramInfo;
    rsmProgramInfo.VShaderFileName = "vRSM.glsl";
    rsmProgramInfo.GShaderFileName = "gRSM.glsl";
    rsmProgramInfo.FShaderFileName = "fRSM.glsl";
    rsmProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                     ShaderType::ST_Geometry |
                                     ShaderType::ST_Fragment;
    Pass* passRSM = new Pass(rsmProgramInfo);

	Technique* techVPL = new Technique();
    techVPL->AddPass(passShadow);
    techVPL->AddPass(passGBuffer);
    techVPL->AddPass(passRSM);
	MaterialTemplate* mtVPL = new MaterialTemplate();
	mtVPL->AddTechnique(techVPL);

    ShaderProgramInfo vplTempProgramInfo;
    vplTempProgramInfo.VShaderFileName = "vVPLTemp.glsl";
    vplTempProgramInfo.FShaderFileName = "fVPLTemp.glsl";
    vplTempProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(vplTempProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "fDirectLighting.glsl";
    directLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(directLightingProgramInfo);

    Technique* techDirectLighting = new Technique();
    techDirectLighting->AddPass(passDirectLighting);
    MaterialTemplate* mtDirectLighting = new MaterialTemplate();
    mtDirectLighting->AddTechnique(techDirectLighting);

    ShaderProgramInfo indirectLightingProgramInfo;
    indirectLightingProgramInfo.VShaderFileName = "vIndirectLighting.glsl";
    indirectLightingProgramInfo.FShaderFileName = "fIndirectLighting.glsl";
    indirectLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passIndirectLighting = new Pass(indirectLightingProgramInfo);

    Technique* techIndirectLighting = new Technique();
    techIndirectLighting->AddPass(passIndirectLighting);
    MaterialTemplate* mtIndirectLighting = new MaterialTemplate();
    mtIndirectLighting->AddTechnique(techIndirectLighting);

    // Create G-buffer textures.
    mGBufferPositionTexture = new Texture2D();
    mGBufferPositionTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_RGBAF);
    mGBufferNormalTexture = new Texture2D();
    mGBufferNormalTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_RGBAF);
    mGBufferAlbedoTexture = new Texture2D();
    mGBufferAlbedoTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_RGBAF);
    mGBufferDepthTexture = new Texture2D();
    mGBufferDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_Depth);

    // Create G-buffer.
    Texture* gbufferTextures[3] = { mGBufferPositionTexture, mGBufferNormalTexture, mGBufferAlbedoTexture };
    mGBufferFB = new FrameBuffer();
    mGBufferFB->SetRenderTargets(3, gbufferTextures, mGBufferDepthTexture);

    // Create shadow map render target.
    int shadowMapWidth, shadowMapHeight;
    shadowMapWidth = 512;
    shadowMapHeight = 512;
    mShadowMapTexture = new Texture2D();
    mShadowMapTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight, 
        Texture::TF_RGBAF);

    mShadowMapDepthTexture = new Texture2D();
    mShadowMapDepthTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight, 
        Texture::TF_Depth);

    // Create shadow map frame buffer.
    Texture* renderTargets[] = { mShadowMapTexture };
    mShadowMapFB = new FrameBuffer();
    mShadowMapFB->SetRenderTargets(1, renderTargets, mShadowMapDepthTexture);

    // Create direct lighting render target.
    mDirectLightingTexture = new Texture2D();
    mDirectLightingTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_RGBAF);

    mDirectLightingDepthTexture = new Texture2D();
    mDirectLightingDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_Depth);

    // Create direct lighting frame buffer.
    Texture* dlRenderTargets[] = { mDirectLightingTexture };
    mDirectLightingFB = new FrameBuffer();
    mDirectLightingFB->SetRenderTargets(1, dlRenderTargets, mDirectLightingDepthTexture);

    // Create indirect lighting render target.
    mIndirectLightingTexture = new Texture2D();
    mIndirectLightingTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_RGBAF);

    mIndirectLightingDepthTexture = new Texture2D();
    mIndirectLightingDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture::TF_Depth);

    // Create indirect lighting frame buffer.
    Texture* indlRenderTargets[] = { mIndirectLightingTexture };
    mIndirectLightingFB = new FrameBuffer();
    mIndirectLightingFB->SetRenderTargets(1, indlRenderTargets, mIndirectLightingDepthTexture);

    // Create RSM render targets.
    int rsmWidth, rsmHeight;
    rsmWidth = 512;
    rsmHeight = 512;
    mRSMPositionTextureArray = new Texture2DArray();
    mRSMPositionTextureArray->CreateRenderTarget(rsmWidth, rsmHeight, 5, Texture::TF_RGBAF);
    mRSMNormalTextureArray = new Texture2DArray();
    mRSMNormalTextureArray->CreateRenderTarget(rsmWidth, rsmHeight, 5, Texture::TF_RGBAF);
    mRSMFluxTextureArray = new Texture2DArray();
    mRSMFluxTextureArray->CreateRenderTarget(rsmWidth, rsmHeight, 5, Texture::TF_RGBAF);
    mRSMDepthTextureArray = new Texture2DArray();
    mRSMDepthTextureArray->CreateRenderTarget(rsmWidth, rsmHeight, 5, Texture::TF_Depth);

    // Create RSM frame buffer.
    Texture* rsmRenderTargets[] = { mRSMPositionTextureArray, mRSMNormalTextureArray, mRSMFluxTextureArray };
    mRSMFB = new FrameBuffer();
    mRSMFB->SetRenderTargets(3, rsmRenderTargets, mRSMDepthTextureArray);

    // Create VPL sample pattern.
    mVPLSamplePattern = new Texture1D();
    mVPLSamplePattern->CreateUniformRandomTexture(VPL_SAMPLE_COUNT, 4);
    mVPLSampleTest = new Texture1D();
    mVPLSampleTest->LoadFromSystemMemory(GL_RGBA32F, VPL_SAMPLE_COUNT, GL_RGBA, GL_FLOAT, 0);

    // Create VPL buffer.
    GLuint vplBufferSize = (sizeof(vec4)* 3) * VPL_SAMPLE_COUNT;
    mVPLBuffer = new StructuredBuffer();
    mVPLBuffer->ReserveDeviceResource(vplBufferSize, GL_DYNAMIC_COPY);

    // Create VPL sample compute tasks.
    ShaderProgramInfo sampleRSMProgramInfo;
    sampleRSMProgramInfo.CShaderFileName = "cSampleRSM.glsl";
    sampleRSMProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passSampleRSM = new ComputePass(sampleRSMProgramInfo);
    mSampleRSMTask = new VPLSampleRSM();
    mSampleRSMTask->AddPass(passSampleRSM);
    mSampleRSMTask->CreateDeviceResource();
    mSampleRSMTask->VPLSamplePattern = mVPLSamplePattern;
    mSampleRSMTask->VPLSampleTest = mVPLSampleTest;
    mSampleRSMTask->RSMPosition = mRSMPositionTextureArray;
    mSampleRSMTask->RSMNormal = mRSMNormalTextureArray;
    mSampleRSMTask->RSMFlux = mRSMFluxTextureArray;
    mSampleRSMTask->VPLBuffer = mVPLBuffer;

	// Create scene.
	mat4 rotM;
	material = new Material(mtVPL);
	mModel = new VPLTriMesh(material, mCamera);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = Scale(vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 3.0f));
	mModel->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;

    material = new Material(mtVPL);
	mGround = new VPLTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;

    material = new Material(mtVPL);
	mCeiling = new VPLTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;

    material = new Material(mtVPL);
	mBackWall = new VPLTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtVPL);
	mLeftWall = new VPLTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.2f, 0.2f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtVPL);
	mRightWall = new VPLTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.2f, 1.0f, 0.2f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtScreenQuad);
    mTempScreenQuad = new VPLTempScreenQuad(material);
    mTempScreenQuad->LoadFromFile("screenquad.ply");
    mTempScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mTempScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mTempScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mTempScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mTempScreenQuad->CreateDeviceResource();
    mTempScreenQuad->TempTexture = mShadowMapTexture;
    mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;

    material = new Material(mtDirectLighting);
    mDirectLightingScreenQuad = new VPLDirectLightingScreenQuad(material);
    mDirectLightingScreenQuad->LoadFromFile("screenquad.ply");
    mDirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mDirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mDirectLightingScreenQuad->CreateDeviceResource();
    mDirectLightingScreenQuad->GBufferPositionTexture = mGBufferPositionTexture;
    mDirectLightingScreenQuad->GBufferNormalTexture = mGBufferNormalTexture;
    mDirectLightingScreenQuad->GBufferAlbedoTexture = mGBufferAlbedoTexture;
    mDirectLightingScreenQuad->ShadowMapTexture = mShadowMapTexture;
    mDirectLightingScreenQuad->LightProjector = mLightProjector;

    material = new Material(mtIndirectLighting);
    mIndirectLightingScreenQuad = new VPLIndirectLightingScreenQuad(material);
    mIndirectLightingScreenQuad->LoadFromFile("screenquad.ply");
    mIndirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mIndirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mIndirectLightingScreenQuad->CreateDeviceResource();
    mIndirectLightingScreenQuad->VPLCount = VPL_SAMPLE_COUNT;
    mIndirectLightingScreenQuad->GBufferPositionTexture = mGBufferPositionTexture;
    mIndirectLightingScreenQuad->GBufferNormalTexture = mGBufferNormalTexture;
    mIndirectLightingScreenQuad->GBufferAlbedoTexture = mGBufferAlbedoTexture;
    mIndirectLightingScreenQuad->VPLBuffer = mVPLBuffer;
}
//----------------------------------------------------------------------------
void VPLApp::ShadowPass()
{
    mGround->SetCamera(mLightProjector);
	mGround->Render(0, 0);

    mCeiling->SetCamera(mLightProjector);
	mCeiling->Render(0, 0);

    mBackWall->SetCamera(mLightProjector);
	mBackWall->Render(0, 0);

    mLeftWall->SetCamera(mLightProjector);
	mLeftWall->Render(0, 0);

    mRightWall->SetCamera(mLightProjector);
	mRightWall->Render(0, 0);

    mModel->SetCamera(mLightProjector);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void VPLApp::GBufferPass()
{
    mGround->SetCamera(mCamera);
    mGround->Render(0, 1);

    mCeiling->SetCamera(mCamera);
    mCeiling->Render(0, 1);

    mBackWall->SetCamera(mCamera);
    mBackWall->Render(0, 1);

    mLeftWall->SetCamera(mCamera);
    mLeftWall->Render(0, 1);

    mRightWall->SetCamera(mCamera);
    mRightWall->Render(0, 1);

    mModel->SetCamera(mCamera);
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
void VPLApp::Run()
{
    static float angle = 0.0f;
    if( mIsRotatingModel )
    {
        angle += 1.0f;
        mat4 rot;
        rot = RotateY(angle);
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

    // Scene shadow pass.
    mShadowMapFB->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShadowPass();
    mShadowMapFB->Disable();

    // Scene G-buffer pass.
    mGBufferFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GBufferPass();
    mGBufferFB->Disable();

    // Scene light RSM pass.
    mRSMFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RSMPass();
    mRSMFB->Disable();

    // Sample RSM pass (VPL generation).
    mSampleRSMTask->Dispatch(0, VPL_SAMPLE_COUNT, 1, 1);

    // Deferred direct illumination pass.
    mDirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mDirectLightingScreenQuad->Render(0, 0);
    mDirectLightingFB->Disable();

    // Deferred indirect illumination pass.
    mIndirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mIndirectLightingScreenQuad->Render(0, 0);
    mIndirectLightingFB->Disable();

    // Show rendering result.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mTempScreenQuad->Render(0, 0);
    
	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void VPLApp::Terminate()
{
	// Release all resources.

	delete mCamera;
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
}
//----------------------------------------------------------------------------
void VPLApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
    case '1':
        mShowMode = SM_Shadow;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mShadowMapTexture;
        break;

    case '2':
        mShowMode = SM_GBufferPosition;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferPositionTexture;
        break;

    case '3':
        mShowMode = SM_GBufferNormal;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferNormalTexture;
        break;

    case '4':
        mShowMode = SM_GBufferAlbedo;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferAlbedoTexture;
        break;

    case '5':
        mShowMode = SM_RSMPosition;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMPositionTextureArray;
        break;

    case '6':
        mShowMode = SM_RSMNormal;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMNormalTextureArray;
        break;

    case '7':
        mShowMode = SM_RSMFlux;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;
        break;

    case '8':
        mShowMode = SM_DirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mDirectLightingTexture;
        break;

    case '9':
        mShowMode = SM_IndirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
        break;

    case '0':
        mShowMode = SM_Final;
        mTempScreenQuad->ShowMode = 2;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
        mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
        break;

    case 'X':
        mTempScreenQuad->RSMFaceIndex = 0;
        break;

    case 'x':
        mTempScreenQuad->RSMFaceIndex = 1;
        break;

    case 'Y':
        mTempScreenQuad->RSMFaceIndex = 2;
        break;

    case 'y':
        mTempScreenQuad->RSMFaceIndex = 3;
        break;

    case 'Z':
        break;

    case 'z':
        mTempScreenQuad->RSMFaceIndex = 4;
        break;

    case 'w':
        mIsWireframe = !mIsWireframe;
        break;

    case 'r':
        mIsRotatingModel = !mIsRotatingModel;
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------
void VPLApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------