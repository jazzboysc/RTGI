#include "VPLviaRSMApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLviaRSMApp::VPLviaRSMApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("VPL via RSM demo")
{
    mShowMode = SM_Final;
    mIsWireframe = false;
    mRSMPositionData = 0;
    mRSMNormalData = 0;
    mRSMFluxData = 0;
}
//----------------------------------------------------------------------------
VPLviaRSMApp::~VPLviaRSMApp()
{
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 0.01f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light projector.
    mLightProjector = new Camera();
    mLightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "vShadow.glsl";
    programInfo.FShaderFileName = "fShadow.glsl";
    programInfo.TCShaderFileName = "tcShadow.glsl";
    programInfo.TEShaderFileName = "teShadow.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | 
                                  ShaderType::ST_Fragment | 
                                  ShaderType::ST_TessellationControl |
                                  ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(programInfo);

    programInfo.VShaderFileName = "vRSMBuffer.glsl";
    programInfo.FShaderFileName = "fRSMBuffer.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passRSMBuffer = new Pass(programInfo);

    programInfo.VShaderFileName = "vDirectLighting.glsl";
    programInfo.FShaderFileName = "fDirectLighting.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(programInfo);

    programInfo.VShaderFileName = "vIndirectLighting.glsl";
    programInfo.FShaderFileName = "fIndirectLighting.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passIndirectLighting = new Pass(programInfo);

	Technique* techVPLviaRSM = new Technique();
    techVPLviaRSM->AddPass(passShadow);
    techVPLviaRSM->AddPass(passRSMBuffer);
    techVPLviaRSM->AddPass(passDirectLighting);
    techVPLviaRSM->AddPass(passIndirectLighting);
	MaterialTemplate* mtVPLviaRSM = new MaterialTemplate();
    mtVPLviaRSM->AddTechnique(techVPLviaRSM);

    programInfo.VShaderFileName = "vVPLviaRSMTemp.glsl";
    programInfo.FShaderFileName = "fVPLviaRSMTemp.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(programInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    programInfo.VShaderFileName = "vVPLviaRSMFinal.glsl";
    programInfo.FShaderFileName = "fVPLviaRSMFinal.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passFinalScreenQuad = new Pass(programInfo);

    Technique* techFinalScreenQuad = new Technique();
    techFinalScreenQuad->AddPass(passFinalScreenQuad);
    MaterialTemplate* mtFinalScreenQuad = new MaterialTemplate();
    mtFinalScreenQuad->AddTechnique(techFinalScreenQuad);

    // Create shadow map render target.
    int shadowMapWidth, shadowMapHeight;
    shadowMapWidth = 512;
    shadowMapHeight = 512;
    mShadowMapTexture = new Texture2D();
    mShadowMapTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight, 
        Texture2D::RTF_RGBF);
    mShadowMapDepthTexture = new Texture2D();
    mShadowMapDepthTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight,
        Texture2D::RTF_Depth);

    // Create shadow map frame buffer.
    Texture2D* shadowMapRenderTargets[] = { mShadowMapTexture };
    mShadowMapFB = new FrameBuffer();
    mShadowMapFB->SetRenderTargets(1, shadowMapRenderTargets, mShadowMapDepthTexture);

    // Create VPL shadow map render target.
    int vplShadowMapWidth, vplShadowMapHeight;
    vplShadowMapWidth = 128;
    vplShadowMapHeight = 128;
    mVPLShadowMapTexture = new Texture2D();
    mVPLShadowMapTexture->CreateRenderTarget(vplShadowMapWidth, vplShadowMapHeight,
        Texture2D::RTF_RGBF);
    mVPLShadowMapDepthTexture = new Texture2D();
    mVPLShadowMapDepthTexture->CreateRenderTarget(vplShadowMapWidth, vplShadowMapHeight,
        Texture2D::RTF_Depth);

    // Create VPL shadow map frame buffer.
    Texture2D* vplRenderTargets[] = { mVPLShadowMapTexture };
    mVPLShadowMapFB = new FrameBuffer();
    mVPLShadowMapFB->SetRenderTargets(1, vplRenderTargets, mVPLShadowMapDepthTexture);

    // Create RSM-buffer MRT textures.
    mRSMWidth = 512;
    mRSMHeight = 512;
    mRSMPositionTexturePX = new Texture2D();
    mRSMPositionTexturePX->CreateRenderTarget(mRSMWidth, mRSMHeight, Texture2D::RTF_RGBF);
    mRSMNormalTexturePX = new Texture2D();
    mRSMNormalTexturePX->CreateRenderTarget(mRSMWidth, mRSMHeight, Texture2D::RTF_RGBF);
    mRSMFluxTexturePX = new Texture2D();
    mRSMFluxTexturePX->CreateRenderTarget(mRSMWidth, mRSMHeight, Texture2D::RTF_RGBF);
    mRSMDepthTexturePX = new Texture2D();
    mRSMDepthTexturePX->CreateRenderTarget(mRSMWidth, mRSMHeight, Texture2D::RTF_Depth);

    int RSMPixelCount = mRSMWidth * mRSMHeight;
    size_t bufferSize = sizeof(float)* 3 * RSMPixelCount;
    mRSMPositionData = new float[bufferSize];
    memset(mRSMPositionData, 0x00, bufferSize);
    mRSMNormalData = new float[bufferSize];
    memset(mRSMNormalData, 0x00, bufferSize);
    mRSMFluxData = new float[bufferSize];
    memset(mRSMFluxData, 0x00, bufferSize);

    // Create RSM-buffer.
    Texture2D* rsmTextures[3] = { mRSMPositionTexturePX, mRSMNormalTexturePX, mRSMFluxTexturePX };
    mRSMBufferPX = new FrameBuffer();
    mRSMBufferPX->SetRenderTargets(3, rsmTextures, mRSMDepthTexturePX);

    // Create RSM sample texture.
    mRSMSampleTexture = new Texture1D();
    mRSMSampleTexture->CreateUniformRandomTextureRG(128);

    for( int i = 0; i < VPLSampleCount; ++i )
    {
        mRSMSamplePos[2 * i    ] = (float)UniformRandom();
        mRSMSamplePos[2 * i + 1] = (float)UniformRandom();
    }

    // Create direct lighting render target.
    mDirectLightingTexture = new Texture2D();
    mDirectLightingTexture->CreateRenderTarget(mWidth, mHeight,
        Texture2D::RTF_RGBF);
    mDirectLightingDepthTexture = new Texture2D();
    mDirectLightingDepthTexture->CreateRenderTarget(mWidth, mHeight,
        Texture2D::RTF_Depth);

    // Create direct lighting frame buffer.
    Texture2D* directLightingRenderTargets[] = { mDirectLightingTexture };
    mDirectLightingFB = new FrameBuffer();
    mDirectLightingFB->SetRenderTargets(1, directLightingRenderTargets, 
        mDirectLightingDepthTexture);

    // Create indirect lighting render target.
    mIndirectLightingTexture = new Texture2D();
    mIndirectLightingTexture->CreateRenderTarget(mWidth, mHeight,
        Texture2D::RTF_RGBF);
    mIndirectLightingDepthTexture = new Texture2D();
    mIndirectLightingDepthTexture->CreateRenderTarget(mWidth, mHeight,
        Texture2D::RTF_Depth);

    // Create indirect lighting frame buffer.
    Texture2D* indirectLightingRenderTargets[] = { mIndirectLightingTexture };
    mIndirectLightingFB = new FrameBuffer();
    mIndirectLightingFB->SetRenderTargets(1, indirectLightingRenderTargets,
        mIndirectLightingDepthTexture);

	// Create scene.
	mat4 rotM;
	material = new Material(mtVPLviaRSM);
	mModel = new VPLviaRSMTriMesh(material, mCamera);
	mModel->LoadFromFile("cow.ply");
    mat4 scale = Scale(vec3(2.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(2.0f, 2.6f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;
    mModel->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtVPLviaRSM);
	mGround = new VPLviaRSMTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;
    mGround->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtVPLviaRSM);
	mCeiling = new VPLviaRSMTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;
    mCeiling->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtVPLviaRSM);
	mBackWall = new VPLviaRSMTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;
    mBackWall->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtVPLviaRSM);
	mLeftWall = new VPLviaRSMTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;
    mLeftWall->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtVPLviaRSM);
	mRightWall = new VPLviaRSMTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;
    mRightWall->VPLShadowMap = mVPLShadowMapTexture;

    material = new Material(mtScreenQuad);
    mTempResultScreenQuad = new VPLviaRSMTempScreenQuad(material);
    mTempResultScreenQuad->LoadFromFile("screenquad.ply");
    mTempResultScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mTempResultScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mTempResultScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mTempResultScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mTempResultScreenQuad->CreateDeviceResource();
    mTempResultScreenQuad->TempTexture = mDirectLightingTexture;

    material = new Material(mtFinalScreenQuad);
    mFinalResultScreenQuad = new VPLviaRSMFinalScreenQuad(material);
    mFinalResultScreenQuad->LoadFromFile("screenquad.ply");
    mFinalResultScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mFinalResultScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mFinalResultScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mFinalResultScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mFinalResultScreenQuad->CreateDeviceResource();
    mFinalResultScreenQuad->DirectLightingTexture = mDirectLightingTexture;
    mFinalResultScreenQuad->IndirectLightingTexture = mIndirectLightingTexture;
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::ShadowPass()
{
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

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
void VPLviaRSMApp::RSMPass()
{
    // Positive X.
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), 
        vec3(10.0f, 10.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    mGround->SetCamera(mLightProjector);
    mCeiling->SetCamera(mLightProjector);
    mBackWall->SetCamera(mLightProjector);
    mLeftWall->SetCamera(mLightProjector);
    mRightWall->SetCamera(mLightProjector);
    mModel->SetCamera(mLightProjector);

    mGround->Render(0, 1);
    mCeiling->Render(0, 1);
    mBackWall->Render(0, 1);
    mLeftWall->Render(0, 1);
    mRightWall->Render(0, 1);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::VPLShadowPass(const VPL& vpl)
{
    mLightProjector->SetAxes(vpl.R, vpl.U, vpl.D);
    mLightProjector->SetLocation(vpl.E);

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
void VPLviaRSMApp::DirectLightingPass()
{
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

    mGround->SetCamera(mCamera);
    mCeiling->SetCamera(mCamera);
    mBackWall->SetCamera(mCamera);
    mLeftWall->SetCamera(mCamera);
    mRightWall->SetCamera(mCamera);
    mModel->SetCamera(mCamera);

    mGround->Render(0, 2);
    mCeiling->Render(0, 2);
    mBackWall->Render(0, 2);
    mLeftWall->Render(0, 2);
    mRightWall->Render(0, 2);
    mModel->Render(0, 2);
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::IndirectLightingPass(const VPL& vpl)
{
    mLightProjector->SetAxes(vpl.R, vpl.U, vpl.D);
    mLightProjector->SetLocation(vpl.E);

    mGround->SetCamera(mCamera);
    mGround->LightColor = vpl.Flux;
    mCeiling->SetCamera(mCamera);
    mCeiling->LightColor = vpl.Flux;
    mBackWall->SetCamera(mCamera);
    mBackWall->LightColor = vpl.Flux;
    mLeftWall->SetCamera(mCamera);
    mLeftWall->LightColor = vpl.Flux;
    mRightWall->SetCamera(mCamera);
    mRightWall->LightColor = vpl.Flux;
    mModel->SetCamera(mCamera);
    mModel->LightColor = vpl.Flux;

    mGround->Render(0, 3);
    mCeiling->Render(0, 3);
    mBackWall->Render(0, 3);
    mLeftWall->Render(0, 3);
    mRightWall->Render(0, 3);
    mModel->Render(0, 3);
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::Run()
{
    static float angle = 0.0f;
    angle += 1.0f;
    mat4 rot;
    rot = RotateY(angle);
    vec3 trans = mModel->GetWorldTranslation();
    mModel->SetWorldTransform(rot);
    mModel->SetWorldTranslation(trans);

    if( mIsWireframe )
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Shadow pass.
    mShadowMapFB->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShadowPass();
    mShadowMapFB->Disable();

    // RSM pass.
    mRSMBufferPX->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RSMPass();
    mRSMBufferPX->Disable();

    // Create VPLs.
    VPL tempVPL;
    mRSMPositionTexturePX->GetImageData(mRSMPositionData);
    mRSMNormalTexturePX->GetImageData(mRSMNormalData);
    mRSMFluxTexturePX->GetImageData(mRSMFluxData);
    int sampleX = int(mRSMSamplePos[0] * mRSMWidth);
    int sampleY = int(mRSMSamplePos[1] * mRSMHeight);
    int sampleIndex = sampleY*mRSMWidth + sampleX;
    tempVPL.E = *(vec3*)&mRSMPositionData[3 * sampleIndex];
    tempVPL.D = *(vec3*)&mRSMNormalData[3 * sampleIndex];
    tempVPL.Flux = *(vec3*)&mRSMFluxData[3 * sampleIndex];
    tempVPL.D = -(tempVPL.D*2.0f - 1.0f);
    GetOrthogonalBasis(tempVPL.D, tempVPL.R, tempVPL.U);

    // VPL Shadow pass.
    mVPLShadowMapFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    VPLShadowPass(tempVPL);
    mVPLShadowMapFB->Disable();

    // Direct lighting pass.
    mDirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DirectLightingPass();
    mDirectLightingFB->Disable();

    // Indirect lighting pass.
    mIndirectLightingFB->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    IndirectLightingPass(tempVPL);
    mIndirectLightingFB->Disable();

    if( mShowMode == SM_Final )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mFinalResultScreenQuad->Render(0, 0);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mTempResultScreenQuad->Render(0, 0);
    }

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::Terminate()
{
	// Release all resources.

	delete mCamera;
    delete mLightProjector;

    mShadowMapTexture = 0;
    mShadowMapDepthTexture = 0;
    mShadowMapFB = 0;

    mVPLShadowMapTexture = 0;
    mVPLShadowMapDepthTexture = 0;
    mVPLShadowMapFB = 0;

    mRSMBufferPX = 0;
    mRSMPositionTexturePX = 0;
    mRSMNormalTexturePX = 0;
    mRSMFluxTexturePX = 0;
    mRSMDepthTexturePX = 0;

    delete[] mRSMPositionData;
    delete[] mRSMNormalData;
    delete[] mRSMFluxData;

    mRSMSampleTexture = 0;

    mDirectLightingFB = 0;
    mDirectLightingTexture = 0;
    mDirectLightingDepthTexture = 0;

    mIndirectLightingFB = 0;
    mIndirectLightingTexture = 0;
    mIndirectLightingDepthTexture = 0;

    mTempResultScreenQuad = 0;
    mFinalResultScreenQuad = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
    case '1':
        mShowMode = SM_DirectLighting;
        mTempResultScreenQuad->TempTexture = mDirectLightingTexture;
        break;

    case '2':
        mShowMode = SM_Shadow;
        mTempResultScreenQuad->TempTexture = mShadowMapTexture;
        break;

    case '3':
        mShowMode = SM_RSMPosition;
        mTempResultScreenQuad->TempTexture = mRSMPositionTexturePX;
        break;

    case '4':
        mShowMode = SM_RSMNormal;
        mTempResultScreenQuad->TempTexture = mRSMNormalTexturePX;
        break;

    case '5':
        mShowMode = SM_RSMFlux;
        mTempResultScreenQuad->TempTexture = mRSMFluxTexturePX;
        break;

    case '6':
        mShowMode = SM_VPLShadow;
        mTempResultScreenQuad->TempTexture = mVPLShadowMapTexture;
        break;

    case '7':
        mShowMode = SM_IndirectLighting;
        mTempResultScreenQuad->TempTexture = mIndirectLightingTexture;
        break;

    case '0':
        mShowMode = SM_Final;
        break;

    case 'w':
        mIsWireframe = !mIsWireframe;
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void VPLviaRSMApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------