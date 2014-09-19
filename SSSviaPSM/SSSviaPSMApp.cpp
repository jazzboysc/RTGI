#include "SSSviaPSMApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSSviaPSMApp::SSSviaPSMApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Subsurface Scattering via PSM demo")
{
    mShowMode = SM_Scene;
    mIsRotatingModel = false;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
SSSviaPSMApp::~SSSviaPSMApp()
{
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 1.0f;
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
    mLightProjector->SetPerspectiveFrustum(45.0f, (float)mWidth / (float)mHeight, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 5.0f, -9.5f), vec3(0.0f, 5.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

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
    programInfo.VShaderFileName = "vScene.glsl";
    programInfo.FShaderFileName = "fScene.glsl";
    programInfo.TCShaderFileName = "";
    programInfo.TEShaderFileName = "";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                  ShaderType::ST_Fragment;
    Pass* passScene = new Pass(programInfo);
	Technique* techISM = new Technique();
    techISM->AddPass(passShadow);
    techISM->AddPass(passScene);
	MaterialTemplate* mtISM = new MaterialTemplate();
	mtISM->AddTechnique(techISM);

    programInfo.VShaderFileName = "vSSSviaPSMTemp.glsl";
    programInfo.FShaderFileName = "fSSSviaPSMTemp.glsl";
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
    mShadowMapTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight, 
        Texture2D::RTF_RGBF);

    mShadowMapDepthTexture = new Texture2D();
    mShadowMapDepthTexture->CreateRenderTarget(shadowMapWidth, shadowMapHeight, 
        Texture2D::RTF_Depth);

    // Create shadow map frame buffer.
    Texture2D* renderTargets[] = { mShadowMapTexture };
    mShadowMapFB = new FrameBuffer();
    mShadowMapFB->SetRenderTargets(1, renderTargets, mShadowMapDepthTexture);

	// Create scene.
	mat4 rotM;
	material = new Material(mtISM);
	mModel = new SSSviaPSMTriMesh(material, mCamera);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = Scale(vec3(60.0f));
    //mat4 scale = Scale(vec3(1.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 0.0f));
    //mModel->SetWorldTranslation(vec3(0.0f, 6.0f, 0.0f));
	mModel->MaterialColor = vec3(0.15f, 0.55f, 0.35f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;
    mModel->IsSSS = true;

    material = new Material(mtISM);
	mGround = new SSSviaPSMTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.8f, 0.8f, 0.8f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mCeiling = new SSSviaPSMTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mBackWall = new SSSviaPSMTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mLeftWall = new SSSviaPSMTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtISM);
	mRightWall = new SSSviaPSMTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->LightProjector = mLightProjector;
    mRightWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtScreenQuad);
    mShadowMapScreenQuad = new SSSviaPSMTempScreenQuad(material);
    mShadowMapScreenQuad->LoadFromFile("screenquad.ply");
    mShadowMapScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mShadowMapScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mShadowMapScreenQuad->CreateDeviceResource();
    mShadowMapScreenQuad->TempTexture = mShadowMapTexture;
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::DrawShadow()
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
void SSSviaPSMApp::DrawScene()
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
void SSSviaPSMApp::Run()
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

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::Terminate()
{
	// Release all resources.

	delete mCamera;
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
void SSSviaPSMApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
    case '1':
        mShowMode = SM_Scene;
        break;

    case '2':
        mShowMode = SM_Shadow;
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
void SSSviaPSMApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void SSSviaPSMApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------