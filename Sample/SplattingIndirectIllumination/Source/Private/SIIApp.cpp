#include "SIIApp.h"
#include <glfw3.h>

using namespace RTGI;

//----------------------------------------------------------------------------
SIIApp::SIIApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Splatting Indirect Illumination demo";
    mShowMode = SM_Scene;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
SIIApp::~SIIApp()
{
}
//----------------------------------------------------------------------------
void SIIApp::Initialize(GPUDevice* device)
{
	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 0.01f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light projector.
    mLightProjector = new Camera();
    mLightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "SplattingIndirectIllumination/vVPLShadow.glsl";
    programInfo.FShaderFileName = "SplattingIndirectIllumination/fVPLShadow.glsl";
    programInfo.TCShaderFileName = "SplattingIndirectIllumination/tcVPLShadow.glsl";
    programInfo.TEShaderFileName = "SplattingIndirectIllumination/teVPLShadow.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | 
                                  ShaderType::ST_Fragment | 
                                  ShaderType::ST_TessellationControl |
                                  ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(programInfo);

    programInfo.VShaderFileName = "SplattingIndirectIllumination/vScene.glsl";
    programInfo.FShaderFileName = "SplattingIndirectIllumination/fScene.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScene = new Pass(programInfo);

    programInfo.VShaderFileName = "SplattingIndirectIllumination/vRSMBuffer.glsl";
    programInfo.FShaderFileName = "SplattingIndirectIllumination/fRSMBuffer.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passRSMBuffer = new Pass(programInfo);

	Technique* techSII = new Technique();
    techSII->AddPass(passShadow);
    techSII->AddPass(passRSMBuffer);
    techSII->AddPass(passScene);
	MaterialTemplate* mtSII = new MaterialTemplate();
    mtSII->AddTechnique(techSII);

    programInfo.VShaderFileName = "SplattingIndirectIllumination/vSampleRSMBuffer.glsl";
    programInfo.FShaderFileName = "SplattingIndirectIllumination/fSampleRSMBuffer.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passVPLQuad = new Pass(programInfo);

    Technique* techVPLQuad = new Technique();
    techVPLQuad->AddPass(passVPLQuad);
    MaterialTemplate* mtVPLQuad = new MaterialTemplate();
    mtVPLQuad->AddTechnique(techVPLQuad);

    programInfo.VShaderFileName = "SplattingIndirectIllumination/vSIITemp.glsl";
    programInfo.FShaderFileName = "SplattingIndirectIllumination/fSIITemp.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(programInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    // Create shadow map render target.
    int shadowMapWidth, shadowMapHeight;
    shadowMapWidth = 1024;
    shadowMapHeight = 1024;
    mShadowMapTexture = new Texture2D();
    mShadowMapTexture->CreateRenderTarget(mDevice, shadowMapWidth, 
        shadowMapHeight, BF_RGBF);

    mShadowMapDepthTexture = new Texture2D();
    mShadowMapDepthTexture->CreateRenderTarget(mDevice, shadowMapWidth, 
        shadowMapHeight, BF_Depth);

    // Create shadow map frame buffer.
    Texture* renderTargets[] = { mShadowMapTexture };
    mShadowMapFB = new FrameBuffer(mDevice);
    mShadowMapFB->SetRenderTargets(1, renderTargets, mShadowMapDepthTexture);

    // Create RSM-buffer MRT textures.
    int RSMWidth = 1024;
    int RSMHeight = 1024;
    mRSMPositionTexturePX = new Texture2D();
    mRSMPositionTexturePX->CreateRenderTarget(mDevice, RSMWidth, RSMHeight, 
        BF_RGBF);
    mRSMNormalTexturePX = new Texture2D();
    mRSMNormalTexturePX->CreateRenderTarget(mDevice, RSMWidth, RSMHeight, 
        BF_RGBF);
    mRSMFluxTexturePX = new Texture2D();
    mRSMFluxTexturePX->CreateRenderTarget(mDevice, RSMWidth, RSMHeight, 
        BF_RGBF);
    mRSMDepthTexturePX = new Texture2D();
    mRSMDepthTexturePX->CreateRenderTarget(mDevice, RSMWidth, RSMHeight, 
        BF_Depth);

    // Create RSM-buffer.
    Texture* rsmTextures[3] = { mRSMPositionTexturePX, mRSMNormalTexturePX, mRSMFluxTexturePX };
    mRSMBufferPX = new FrameBuffer(mDevice);
    mRSMBufferPX->SetRenderTargets(3, rsmTextures, mRSMDepthTexturePX);

    // Create RSM sample texture.
    mRSMSampleTexture = new Texture1D();
    mRSMSampleTexture->CreateUniformRandomTexture(mDevice, 128, 2);

    // Create VPL quad.
    material = new Material(mtVPLQuad);
    mVPLQuad = new SIIVPLQuad(material, mMainCamera);
    mVPLQuad->LoadFromPLYFile("screenquad.ply");
    mVPLQuad->CreateDeviceResource(mDevice);
    mVPLQuad->IsQuad = true;
    mVPLQuad->RSMPositionTexture = mRSMPositionTexturePX;
    mVPLQuad->RSMNormalTexture = mRSMNormalTexturePX;
    mVPLQuad->RSMFluxTexture = mRSMFluxTexturePX;
    mVPLQuad->RSMSampleTexture = mRSMSampleTexture;

	// Create scene.
	mat4 rotM;
	material = new Material(mtSII);
	mModel = new SIITriMesh(material, mMainCamera);
	mModel->LoadFromPLYFile("cow.ply");
    mat4 scale = glm::scale(mat4(), vec3(2.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mModel->SetWorldTranslation(vec3(-2.0f, 3.0f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mGround = new SIITriMesh(material, mMainCamera);
	mGround->LoadFromPLYFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mCeiling = new SIITriMesh(material, mMainCamera);
	mCeiling->LoadFromPLYFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mBackWall = new SIITriMesh(material, mMainCamera);
	mBackWall->LoadFromPLYFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mLeftWall = new SIITriMesh(material, mMainCamera);
	mLeftWall->LoadFromPLYFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mRightWall = new SIITriMesh(material, mMainCamera);
	mRightWall->LoadFromPLYFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtScreenQuad);
    mShadowMapScreenQuad = new SIITempScreenQuad(material);
	mShadowMapScreenQuad->LoadFromPLYFile("screenquad.ply");
    mShadowMapScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mShadowMapScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mShadowMapScreenQuad->CreateDeviceResource(mDevice);
    mShadowMapScreenQuad->TempTexture = mShadowMapTexture;
}
//----------------------------------------------------------------------------
void SIIApp::DrawShadow()
{
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

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
void SIIApp::DrawRSMs()
{
    // Positive X.
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), 
        vec3(10.0f, 10.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));


    mGround->SetCamera(mLightProjector);
    mGround->Render(0, 1);

    mCeiling->SetCamera(mLightProjector);
    mCeiling->Render(0, 1);

    mBackWall->SetCamera(mLightProjector);
    mBackWall->Render(0, 1);

    mLeftWall->SetCamera(mLightProjector);
    mLeftWall->Render(0, 1);

    mRightWall->SetCamera(mLightProjector);
    mRightWall->Render(0, 1);

    mModel->SetCamera(mLightProjector);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void SIIApp::SampleRSMs()
{
    mVPLQuad->Render(0, 0);
}
//----------------------------------------------------------------------------
void SIIApp::DrawScene()
{
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

    mGround->SetCamera(mMainCamera);
    mGround->Render(0, 2);

    mCeiling->SetCamera(mMainCamera);
    mCeiling->Render(0, 2);

    mBackWall->SetCamera(mMainCamera);
    mBackWall->Render(0, 2);

    mLeftWall->SetCamera(mMainCamera);
    mLeftWall->Render(0, 2);

    mRightWall->SetCamera(mMainCamera);
    mRightWall->Render(0, 2);

    mModel->SetCamera(mMainCamera);
    mModel->Render(0, 2);
}
//----------------------------------------------------------------------------
void SIIApp::FrameFunc()
{
    static float angle = 0.0f;
    angle += 1.0f;
    mat4 rot;
    rot = rotate(mat4(), radians(angle), vec3(0, 1, 0));
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
    DrawShadow();
    mShadowMapFB->Disable();

    // RSM pass.
    mRSMBufferPX->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawRSMs();
    mRSMBufferPX->Disable();

    // Final pass.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( mShowMode == SM_Scene )
    {
        DrawScene();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        SampleRSMs();
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mShadowMapScreenQuad->Render(0, 0);
    }
}
//----------------------------------------------------------------------------
void SIIApp::Terminate()
{
	// Release all resources.

    delete mLightProjector;

    mShadowMapTexture = 0;
    mShadowMapDepthTexture = 0;
    mShadowMapFB = 0;
    mShadowMapScreenQuad = 0;

    mRSMBufferPX = 0;
    mRSMPositionTexturePX = 0;
    mRSMNormalTexturePX = 0;
    mRSMFluxTexturePX = 0;
    mRSMDepthTexturePX = 0;

    mRSMSampleTexture = 0;

    mVPLQuad = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void SIIApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mShowMode = SM_Scene;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mShowMode = SM_Shadow;
		mShadowMapScreenQuad->TempTexture = mShadowMapTexture;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mShowMode = SM_RSMPosition;
		mShadowMapScreenQuad->TempTexture = mRSMPositionTexturePX;
	}
	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
	{
		mShowMode = SM_RSMNormal;
		mShadowMapScreenQuad->TempTexture = mRSMNormalTexturePX;
	}
	if (glfwGetKey(Window, GLFW_KEY_5) == GLFW_PRESS)
	{
		mShowMode = SM_RSMFlux;
		mShadowMapScreenQuad->TempTexture = mRSMFluxTexturePX;
	}
	if (glfwGetKey(Window, GLFW_KEY_6) == GLFW_PRESS)
	{
		mIsWireframe = !mIsWireframe;
	}
}