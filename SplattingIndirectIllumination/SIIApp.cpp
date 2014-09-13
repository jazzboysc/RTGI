#include "SIIApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SIIApp::SIIApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Splatting Indirect Illumination demo")
{
    mShowMode = SM_Scene;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
SIIApp::~SIIApp()
{
}
//----------------------------------------------------------------------------
void SIIApp::Initialize()
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
    mLightProjector->SetPerspectiveFrustum(90.0f, 1.0f, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "vVPLShadow.glsl";
    programInfo.FShaderFileName = "fVPLShadow.glsl";
    programInfo.TCShaderFileName = "tcVPLShadow.glsl";
    programInfo.TEShaderFileName = "teVPLShadow.glsl";
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

    programInfo.VShaderFileName = "vRSMBuffer.glsl";
    programInfo.FShaderFileName = "fRSMBuffer.glsl";
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

    programInfo.VShaderFileName = "vSIITemp.glsl";
    programInfo.FShaderFileName = "fSIITemp.glsl";
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

    // Create RSM-buffer MRT textures.
    int RSMWidth = 1024;
    int RSMHeight = 1024;
    mRSMPositionTexturePX = new Texture2D();
    mRSMPositionTexturePX->CreateRenderTarget(RSMWidth, RSMHeight, Texture2D::RTF_RGBF);
    mRSMNormalTexturePX = new Texture2D();
    mRSMNormalTexturePX->CreateRenderTarget(RSMWidth, RSMHeight, Texture2D::RTF_RGBF);
    mRSMFluxTexturePX = new Texture2D();
    mRSMFluxTexturePX->CreateRenderTarget(RSMWidth, RSMHeight, Texture2D::RTF_RGBF);
    mRSMDepthTexturePX = new Texture2D();
    mRSMDepthTexturePX->CreateRenderTarget(RSMWidth, RSMHeight, Texture2D::RTF_Depth);

    // Create RSM-buffer.
    Texture2D* rsmTextures[3] = { mRSMPositionTexturePX, mRSMNormalTexturePX, mRSMFluxTexturePX };
    mRSMBufferPX = new FrameBuffer();
    mRSMBufferPX->SetRenderTargets(3, rsmTextures, mRSMDepthTexturePX);

	// Create scene.
	mat4 rotM;
	material = new Material(mtSII);
	mModel = new SIITriMesh(material, mCamera);
	mModel->LoadFromFile("cow.ply");
    mat4 scale = Scale(vec3(2.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(-2.0f, 3.0f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->LightProjector = mLightProjector;
    mModel->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mGround = new SIITriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);
    mGround->LightProjector = mLightProjector;
    mGround->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mCeiling = new SIITriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->LightProjector = mLightProjector;
    mCeiling->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mBackWall = new SIITriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->LightProjector = mLightProjector;
    mBackWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mLeftWall = new SIITriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->ShadowMap = mShadowMapTexture;

    material = new Material(mtSII);
	mRightWall = new SIITriMesh(material, mCamera);
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
    mShadowMapScreenQuad = new SIITempScreenQuad(material);
    mShadowMapScreenQuad->LoadFromFile("screenquad.ply");
    mShadowMapScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mShadowMapScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mShadowMapScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mShadowMapScreenQuad->CreateDeviceResource();
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
}
//----------------------------------------------------------------------------
void SIIApp::DrawScene()
{
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

    mGround->SetCamera(mCamera);
    mGround->Render(0, 2);

    mCeiling->SetCamera(mCamera);
    mCeiling->Render(0, 2);

    mBackWall->SetCamera(mCamera);
    mBackWall->Render(0, 2);

    mLeftWall->SetCamera(mCamera);
    mLeftWall->Render(0, 2);

    mRightWall->SetCamera(mCamera);
    mRightWall->Render(0, 2);

    mModel->SetCamera(mCamera);
    mModel->Render(0, 2);
}
//----------------------------------------------------------------------------
void SIIApp::Run()
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

    mShadowMapFB->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawShadow();
    mShadowMapFB->Disable();

    mRSMBufferPX->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawRSMs();
    mRSMBufferPX->Disable();

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
void SIIApp::Terminate()
{
	// Release all resources.

	delete mCamera;
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

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void SIIApp::OnKeyboard(unsigned char key, int x, int y)
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

    default:
        break;
    }
}
//----------------------------------------------------------------------------
void SIIApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void SIIApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void SIIApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void SIIApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------