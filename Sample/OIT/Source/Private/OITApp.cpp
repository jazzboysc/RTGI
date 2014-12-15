#include "OITApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
OITApp::OITApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Order Independent Transparency demo";
}
//----------------------------------------------------------------------------
OITApp::~OITApp()
{
}
//----------------------------------------------------------------------------
void OITApp::Initialize(GPUDevice* device)
{
	const char* version = (const char*)glGetString(GL_VERSION);
	const char* vendor = (const char*)glGetString(GL_VENDOR);
	printf("OpenGL version: %s, vendor: %s\n", version, vendor);

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
    glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

	// Create camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 0.0001f, 100.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 9.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;

    ShaderProgramInfo gpuABufferProgramInfo;
    gpuABufferProgramInfo.VShaderFileName = "OIT/vGPUABuffer.glsl";
    gpuABufferProgramInfo.FShaderFileName = "OIT/fGPUABuffer.glsl";
    gpuABufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex | ShaderType::ST_Fragment;

    Pass* passGPUABuffer = new Pass(gpuABufferProgramInfo);
	Technique* techGPUABuffer = new Technique();
	techGPUABuffer->AddPass(passGPUABuffer);
	MaterialTemplate* mtGPUABuffer = new MaterialTemplate();
	mtGPUABuffer->AddTechnique(techGPUABuffer);

    ShaderProgramInfo oitProgramInfo;
    oitProgramInfo.VShaderFileName = "OIT/vOIT.glsl";
    oitProgramInfo.FShaderFileName = "OIT/fOIT.glsl";
    oitProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex | ShaderType::ST_Fragment;

    Pass* passOIT = new Pass(oitProgramInfo);
	Technique* techOIT = new Technique();
	techOIT->AddPass(passOIT);
	MaterialTemplate* mtOIT = new MaterialTemplate();
	mtOIT->AddTechnique(techOIT);

	// Create OIT screen quad.
	material = new Material(mtOIT);
	mScreenQuad = new OITScreenQuad(material);
	mScreenQuad->LoadFromFile("screenquad.ply");
	mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mScreenQuad->CreateDeviceResource(mDevice);
	mScreenQuad->HeadPointerTexture = mHeadPointerTexture;
	mScreenQuad->GPUMemPool = mGPUMemPool;
    
	// Create scene.
	material = new Material(mtGPUABuffer);
	mModel = new OITTriMesh(material, mMainCamera);
	mModel->LoadFromFile("dragon_s.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mat4 rot = rotate(mat4(), radians(30.0f), vec3(0, 1, 0));
	mModel->SetWorldTransform(rot);
	mModel->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
    mModel->SetWorldScale(vec3(40.0f));

	// Create head pointer texture.
	mHeadPointerTexture = new Texture2D();
	mHeadPointerTexture->CreateRenderTarget(Width, Height, 
		Texture2D::TF_R32UI);

	// Create head pointer texture init data.
	int pixelCount = Width * Height;
	mHeadPointerTextureInitData = new PixelBuffer();
	mHeadPointerTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(GLuint), BU_Static_Draw);
	mHeadPointerTextureInitData->Bind();
	void* pixelBufferData = mHeadPointerTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, pixelCount*sizeof(GLuint));
	mHeadPointerTextureInitData->Unmap();

	// Create GPU memory allocator counter.
	mGPUMemAllocCounter = new AtomicCounterBuffer();
	mGPUMemAllocCounter->ReserveDeviceResource(sizeof(GLuint),
        BU_Dynamic_Copy);

	// Create GPU memory pool for concurrent linked lists.
	size_t gpuMemPoolSize = 2 * pixelCount * sizeof(vec4);
	mGPUMemPool = new TextureBuffer();
    mGPUMemPool->ReserveDeviceResource(gpuMemPoolSize, BU_Dynamic_Copy);

	gpuMemPoolSize = 2 * pixelCount * (sizeof(vec4) + sizeof(GLuint) + 
		sizeof(GLfloat));
	mGPUMemPool2 = new StructuredBuffer();
    mGPUMemPool2->ReserveDeviceResource(gpuMemPoolSize, BU_Dynamic_Copy);

	// Create GPU memory pool texture.
	mGPUMemPoolTexture = new Texture2D();
	mGPUMemPoolTexture->LoadFromTextureBuffer(mGPUMemPool, GL_RGBA32UI);
}
//----------------------------------------------------------------------------
void OITApp::FrameFunc()
{
	// Reset atomic counter.
	//GLuint zero = 0;
	//mGPUMemAllocCounter->UpdateSubData(0, 0, sizeof(zero), &zero);
	mGPUMemAllocCounter->Bind(0);
	GLuint* counterData = (GLuint*)mGPUMemAllocCounter->Map(GL_WRITE_ONLY);
	assert( counterData );
	counterData[0] = 0;
	mGPUMemAllocCounter->Unmap();

	// Reset head pointer texture.
	mHeadPointerTexture->UpdateFromPixelBuffer(mHeadPointerTextureInitData);

	// Bind textures to image units.
	mHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);
	mGPUMemPoolTexture->BindToImageUnit(1, GL_READ_WRITE);

	// Bind structured buffer.
	mGPUMemPool2->Bind(0);
 
   	glClear(GL_COLOR_BUFFER_BIT);
    mModel->Render(0, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	mScreenQuad->Render(0, 0);
}
//----------------------------------------------------------------------------
void OITApp::Terminate()
{
	mGPUMemAllocCounter = 0;
	mHeadPointerTexture = 0;
	mHeadPointerTextureInitData = 0;
	mGPUMemPoolTexture = 0;
	mGPUMemPool = 0;
	mGPUMemPool2 = 0;

	mScreenQuad = 0;
    mModel = 0;
}
//----------------------------------------------------------------------------
void OITApp::ProcessInput()
{

}