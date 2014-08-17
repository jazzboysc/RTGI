#include "RayBundleApp.h"
#include "RNG.h"

using namespace RTGI;

static GLboolean QueryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string. Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names. Could use strtok() but the constant
    ** string returned by glGetString might be in read-only memory.
    */
    char *p;
    char *end;
    int extNameLen;   

    extNameLen = strlen(extName);
        
    p = (char *)glGetString(GL_EXTENSIONS);
    if( NULL == p )
	{
        return GL_FALSE;
    }

    end = p + strlen(p);   

    while( p < end )
	{
        int n = strcspn(p, " ");
        if( (extNameLen == n) && (strncmp(extName, p, n) == 0) )
		{
            return GL_TRUE;
        }
        p += (n + 1);
    }
    return GL_FALSE;
}

//----------------------------------------------------------------------------
RayBundleApp::RayBundleApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Ray-bundle demo")
{
	sphereRadius = 14.15f;
	sphereCenter = vec3(0.0f, 10.0f, 0.0f);
	mVoxelGridDim = 128;
	mVoxelCount = mVoxelGridDim * mVoxelGridDim * mVoxelGridDim;
	mRayBundleRTWidth = 64;
	mRayBundleRTHeight = 64;
}
//----------------------------------------------------------------------------
RayBundleApp::~RayBundleApp()
{
}
//----------------------------------------------------------------------------
void RayBundleApp::Initialize()
{
	GLboolean supportNVAtomicFloatOp = QueryExtension("GL_NV_shader_atomic_float");
	assert( supportNVAtomicFloatOp );

	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	// Create ray-bundle projector.
	mRayBundleProjector = new Camera(false);
	mRayBundleProjector->SetOrthogonalFrustum(11.0f, 
		(float)mRayBundleRTWidth/(float)mRayBundleRTHeight, 1.0f, 100.0f);
	mRayBundleProjector->SetLookAt(vec3(0.0f, -4.15f, 0.0f), sphereCenter,
		vec3(0.0f, 0.0f, 1.0f));
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passScene = new Pass("vScene.glsl", "fScene.glsl");
	Pass* passRayBundle = new Pass("vRayBundle.glsl", "fRayBundle.glsl");
	Technique* techRayBundle = new Technique();
	techRayBundle->AddPass(passScene);
	techRayBundle->AddPass(passRayBundle);
	MaterialTemplate* mtRayBundle = new MaterialTemplate();
	mtRayBundle->AddTechnique(techRayBundle);

	Pass* passUpdateAccumulation = new Pass("vUpdateAccumulation.glsl", "fUpdateAccumulation.glsl");
	Technique* techUpdateAccumulation = new Technique();
	techUpdateAccumulation->AddPass(passUpdateAccumulation);
	MaterialTemplate* mtUpdateAccumulation = new MaterialTemplate();
	mtUpdateAccumulation->AddTechnique(techUpdateAccumulation);

	// Create scene.
	mat4 rotM;
	material = new Material(mtRayBundle);
	mModel = new RayBundleTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
	mSceneBB.Merge(mModel->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mGround = new RayBundleTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);
	mSceneBB.Merge(mGround->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mCeiling = new RayBundleTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
	mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mLight = new RayBundleTriMesh(material, mCamera);
	mLight->LoadFromFile("square.ply");
	mLight->GenerateNormals();
	mLight->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mLight->SetWorldTransform(rotM);
	mLight->SetWorldScale(vec3(0.5f));
	mLight->SetWorldTranslation(vec3(0.0f, 19.0f, 3.0f));
	mLight->EmissionColor = vec3(1000.0f, 1000.0f, 1000.0f);
	mLight->MaterialColor = vec3(0.0f, 0.0f, 0.0f);
	mLight->IsLight = true;
	mSceneBB.Merge(mLight->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mBackWall = new RayBundleTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
	mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mLeftWall = new RayBundleTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
	mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

	material = new Material(mtRayBundle);
	mRightWall = new RayBundleTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
	mSceneBB.Merge(mRightWall->GetWorldSpaceBB());

	mVoxelGridCenter = mSceneBB.GetBoxCenter();

	int pixelCount = 0;
	void* pixelBufferData = 0;
	size_t gpuMemPoolSize = 0;

	// Create ray head pointer texture.
	mRayHeadPointerTexture = new Texture2D();
	mRayHeadPointerTexture->CreateRenderTarget(mRayBundleRTWidth, mRayBundleRTHeight, 
		Texture2D::RTF_R32UI);

	// Create ray head pointer texture init data.
	pixelCount = mRayBundleRTWidth * mRayBundleRTHeight;
	mRayHeadPointerTextureInitData = new PixelBuffer();
	mRayHeadPointerTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(GLuint), GL_STATIC_DRAW);
	mRayHeadPointerTextureInitData->Bind();
	pixelBufferData = mRayHeadPointerTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, pixelCount*sizeof(GLuint));
	mRayHeadPointerTextureInitData->Unmap();

	// Create per-voxel mutex texture.
	int voxelMutexCount = mVoxelCount;
	mPerVoxelMutexTexture = new Texture1D();
	mPerVoxelMutexTexture->LoadFromSystemMemory(GL_R32UI, voxelMutexCount, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);

	// Create per-voxel mutex texture init data.
	mPerVoxelMutexTextureInitData = new PixelBuffer();
	mPerVoxelMutexTextureInitData->ReserveDeviceResource(voxelMutexCount*sizeof(GLuint), GL_STATIC_DRAW);
	mPerVoxelMutexTextureInitData->Bind();
	pixelBufferData = mPerVoxelMutexTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, voxelMutexCount*sizeof(GLuint));
	mPerVoxelMutexTextureInitData->Unmap();

	// Create ray GPU memory allocator counter.
	mRayAllocCounter = new AtomicCounterBuffer();
	mRayAllocCounter->ReserveDeviceResource(sizeof(GLuint),
		GL_DYNAMIC_COPY);

	// Create ray GPU memory pool for concurrent linked lists.
	gpuMemPoolSize = 8 * pixelCount * (4*sizeof(vec4) + sizeof(GLuint) + 
		sizeof(GLfloat) + sizeof(GLboolean));
	mRayBundleNodeBuffer = new StructuredBuffer();
	mRayBundleNodeBuffer->ReserveDeviceResource(gpuMemPoolSize, GL_DYNAMIC_COPY);

	// Create accumulation buffer.
	size_t bufferSize = mVoxelCount * sizeof(vec4);
	mAccumulationBuffer = new StructuredBuffer();
	mAccumulationBuffer->ReserveDeviceResource(bufferSize, GL_DYNAMIC_COPY);

	// Create ray-bundle render target.
	mRayBundleRT = new Texture2D();
	mRayBundleRT->CreateRenderTarget(mRayBundleRTWidth, mRayBundleRTHeight, Texture2D::RTF_RGBF);
	mRayBundleDepth = new Texture2D();
	mRayBundleDepth->CreateRenderTarget(mRayBundleRTWidth, mRayBundleRTHeight, Texture2D::RTF_Depth);

	Texture2D* rayBundleRT[1] = {mRayBundleRT};
	mRayBundleFB = new FrameBuffer();
	mRayBundleFB->SetRenderTargets(1, rayBundleRT, mRayBundleDepth);

	// Create update accumulation screen quad.
	material = new Material(mtUpdateAccumulation);
	mUpdateAccuScreenQuad = new UpdateAccumulationScreenQuad(material);
	mUpdateAccuScreenQuad->LoadFromFile("screenquad.ply");
	mUpdateAccuScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mUpdateAccuScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mUpdateAccuScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mUpdateAccuScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mUpdateAccuScreenQuad->CreateDeviceResource();

	for( int i = 0; i < RAYBUNDLE_SAMPLE_COUNT; ++i )
	{
		float u1 = (float)UniformRandom();
		float u2 = (float)UniformRandom();
		mRandmoDirections[i] = UniformSampleSphere(u1, u2);
	}
}
//----------------------------------------------------------------------------
void RayBundleApp::DrawRayBundle()
{
	mGround->SetCamera(mRayBundleProjector);
	mGround->Render(0, 1);

	//mCeiling->SetCamera(mRayBundleProjector);
	//mCeiling->Render(0, 1);

	mLight->SetCamera(mRayBundleProjector);
	mLight->Render(0, 1);

	//mBackWall->SetCamera(mRayBundleProjector);
	//mBackWall->Render(0, 1);

	mLeftWall->SetCamera(mRayBundleProjector);
	mLeftWall->Render(0, 1);

	mRightWall->SetCamera(mRayBundleProjector);
	mRightWall->Render(0, 1);

	mModel->SetCamera(mRayBundleProjector);
	mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void RayBundleApp::DrawScene()
{
	mGround->SetCamera(mCamera);
	mGround->Render(0, 0);

	//mCeiling->SetCamera(mCamera);
	//mCeiling->Render(0, 0);

	mLight->SetCamera(mCamera);
	mLight->Render(0, 0);

	//mBackWall->SetCamera(mCamera);
	//mBackWall->Render(0, 0);

	mLeftWall->SetCamera(mCamera);
	mLeftWall->Render(0, 0);

	mRightWall->SetCamera(mCamera);
	mRightWall->Render(0, 0);

	mModel->SetCamera(mCamera);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void RayBundleApp::Run()
{
	// Reset accumulation buffer.
	mAccumulationBuffer->Bind(1);
	vec4* bufferData = (vec4*)mAccumulationBuffer->Map(GL_WRITE_ONLY);
	assert( bufferData );
	memset(bufferData, 0x00, mAccumulationBuffer->GetSize());
	mAccumulationBuffer->Unmap();

	//for( int i = 0; i < RAYBUNDLE_SAMPLE_COUNT; ++i )
	//{
	//	vec3 d = mRandmoDirections[i];
	//	vec3 u, r;
	//	if( d.x > d.y )
	//	{
	//		u = vec3(d.z, 0.0, -d.x);
	//	}
	//	else
	//	{
	//		u = vec3(0.0, d.z, -d.y);
	//	}
	//	u = normalize(u);
	//	r = cross(u, d);
	//	vec3 e = sphereCenter + d*sphereRadius;

	//	mRayBundleProjector->SetLocation(e);
	//	mRayBundleProjector->SetAxes(r, u, d);

		mRayBundleProjector->SetLookAt(vec3(0.0f, -4.15f, 0.0f), sphereCenter,
			vec3(0.0f, 0.0f, 1.0f));

		// Reset ray bundle atomic counter.
		mRayAllocCounter->Bind(0);
		GLuint* counterData = (GLuint*)mRayAllocCounter->Map(GL_WRITE_ONLY);
		assert( counterData );
		counterData[0] = 0;
		mRayAllocCounter->Unmap();

		// Reset ray bundle head pointer texture.
		mRayHeadPointerTexture->UpdateFromPixelBuffer(
			mRayHeadPointerTextureInitData);

		// Bind ray bundle texture to image unit.
		mRayHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);

		// Bind ray bundle buffer.
		mRayBundleNodeBuffer->Bind(0);

		// Reset per-voxel mutex texture.
		mPerVoxelMutexTexture->UpdateFromPixelBuffer(mPerVoxelMutexTextureInitData);
		mPerVoxelMutexTexture->BindToImageUnit(1, GL_READ_WRITE);

		mRayBundleFB->Enable();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT);

		// Create a ray bundle.
		DrawRayBundle();

		// Transfer energy for the ray bundle.
		mUpdateAccuScreenQuad->WorldRayBundleDirection = -mRayBundleProjector->GetDirection();
		mUpdateAccuScreenQuad->VoxelGridCenter = mVoxelGridCenter;
		mUpdateAccuScreenQuad->Render(0, 0);

		mRayBundleFB->Disable();

		//mRayBundleProjector->SetLookAt(vec3(14.15f, 10.0f, 0.0f), sphereCenter,
		//vec3(0.0f, 1.0f, 0.0f));

		//// Reset ray bundle atomic counter.
		//mRayAllocCounter->Bind(0);
		//counterData = (GLuint*)mRayAllocCounter->Map(GL_WRITE_ONLY);
		//assert( counterData );
		//counterData[0] = 0;
		//mRayAllocCounter->Unmap();

		//// Reset ray bundle head pointer texture.
		//mRayHeadPointerTexture->UpdateFromPixelBuffer(
		//	mRayHeadPointerTextureInitData);

		//// Bind ray bundle texture to image unit.
		//mRayHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);

		//// Bind ray bundle buffer.
		//mRayBundleNodeBuffer->Bind(0);

		//// Reset per-voxel mutex texture.
		////mPerVoxelMutexTexture->UpdateFromPixelBuffer(mPerVoxelMutexTextureInitData);

		////mPerVoxelMutexTexture->BindToImageUnit(1, GL_READ_WRITE);

		//// Create a ray bundle.
		//DrawRayBundle();

		//// Transfer energy for the ray bundle.
		//mUpdateAccuScreenQuad->WorldRayBundleDirection = -mRayBundleProjector->GetDirection();
		//mUpdateAccuScreenQuad->Render(0, 0);
	//}

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void RayBundleApp::Terminate()
{
	// Release all resources.

    delete mRayBundleProjector;
	delete mCamera;

	mGround = 0;
	mCeiling = 0;
	mLight = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;

	mRayAllocCounter = 0;
	mRayHeadPointerTexture = 0;
	mRayHeadPointerTextureInitData = 0;
	mPerVoxelMutexTexture = 0;
	mPerVoxelMutexTextureInitData = 0;
	mRayBundleNodeBuffer = 0;
	mAccumulationBuffer = 0;
	mRayBundleFB = 0;
	mRayBundleRT = 0;
	mRayBundleDepth = 0;

	mUpdateAccuScreenQuad = 0;
}
//----------------------------------------------------------------------------
void RayBundleApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------