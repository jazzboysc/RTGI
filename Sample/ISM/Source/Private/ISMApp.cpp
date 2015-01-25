#include "ISMApp.h"
#include <glfw3.h>
using namespace RTGI;

//----------------------------------------------------------------------------
ISMApp::ISMApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "ISM demo";
    mShowMode = SM_Scene;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
ISMApp::~ISMApp()
{
}
//----------------------------------------------------------------------------
void ISMApp::Initialize(GPUDevice* device)
{
	float color = 1.0f;
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
    mLightProjector->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.01f, 50.0f);
    //mLightProjector->SetLookAt(vec3(-10.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    //    vec3(0.0f, 1.0f, 0.0f));
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "ISM/vShadow.glsl";
    programInfo.FShaderFileName = "ISM/fShadow.glsl";
    programInfo.TCShaderFileName = "ISM/tcShadow.glsl";
    programInfo.TEShaderFileName = "ISM/teShadow.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | 
                                  ShaderType::ST_Fragment | 
                                  ShaderType::ST_TessellationControl |
                                  ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(programInfo);
    programInfo.VShaderFileName = "ISM/vScene.glsl";
    programInfo.FShaderFileName = "ISM/fScene.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScene = new Pass(programInfo);
	Technique* techISM = new Technique();
    techISM->AddPass(passShadow);
    techISM->AddPass(passScene);
	MaterialTemplate* mtISM = new MaterialTemplate();
	mtISM->AddTechnique(techISM);

    programInfo.VShaderFileName = "ISM/vISMTemp.glsl";
    programInfo.FShaderFileName = "ISM/fISMTemp.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(programInfo);
    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    // Create shadow map render target.
    int shadowMapWidth, shadowMapHeight;
    shadowMapWidth = 256;
    shadowMapHeight = 256;
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

	// Create scene.
	mat4 rotM;
	material = new Material(mtISM);
	mModel = new ISMTriMesh(material, mMainCamera);
	mModel->LoadFromFile("cow.ply");
    mat4 scale = glm::scale(mat4(), vec3(2.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mModel->SetWorldTranslation(vec3(-2.0f, 3.0f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mGround = new ISMTriMesh(material, mMainCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mCeiling = new ISMTriMesh(material, mMainCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mBackWall = new ISMTriMesh(material, mMainCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mLeftWall = new ISMTriMesh(material, mMainCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mRightWall = new ISMTriMesh(material, mMainCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtScreenQuad);
    mShadowMapScreenQuad = new ISMTempScreenQuad(material);
    mShadowMapScreenQuad->LoadFromFile("screenquad.ply");
    mShadowMapScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mShadowMapScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mShadowMapScreenQuad->CreateDeviceResource(mDevice);
    mShadowMapScreenQuad->TempTexture = mShadowMapTexture;
}
//----------------------------------------------------------------------------
void ISMApp::DrawShadow()
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
void ISMApp::DrawScene()
{
    mGround->SetCamera(mMainCamera);
    mGround->Render(0, 1);

    mCeiling->SetCamera(mMainCamera);
    mCeiling->Render(0, 1);

    mBackWall->SetCamera(mMainCamera);
    mBackWall->Render(0, 1);

    mLeftWall->SetCamera(mMainCamera);
    mLeftWall->Render(0, 1);

    mRightWall->SetCamera(mMainCamera);
    mRightWall->Render(0, 1);

    mModel->SetCamera(mMainCamera);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void ISMApp::FrameFunc()
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

    mShadowMapFB->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawShadow();
    mShadowMapFB->Disable();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( mShowMode == SM_Scene )
    {
        DrawScene();
    }
    else if( mShowMode == SM_Shadow )
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mShadowMapScreenQuad->Render(0, 0);
    }
}
//----------------------------------------------------------------------------
void ISMApp::Terminate()
{
	// Release all resources.

    delete mLightProjector;

    mShadowMapTexture = 0;
    mShadowMapDepthTexture = 0;
    mShadowMapFB = 0;
    mShadowMapScreenQuad = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void ISMApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mShowMode = SM_Scene;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mShowMode = SM_Shadow;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mIsWireframe = !mIsWireframe;
	}
}