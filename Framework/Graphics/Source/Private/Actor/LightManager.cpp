//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "LightManager.h"
#include "LightMesh.h"

using namespace RTGI;
using namespace glm;

//----------------------------------------------------------------------------
LightManager::LightManager(GPUDevice* device)
    :
    mDevice(device),
    mLightBufferBindingPoint(0)
{
    size_t bufferSize = sizeof(LightBufferHead) + 
        sizeof(SceneLight)*MAX_LIGHT_COUNT;
    mLightBufferCache = new unsigned char[bufferSize];
    memset(mLightBufferCache, 0, bufferSize);

    ShaderProgramInfo lightMeshProgramInfo;
    lightMeshProgramInfo.VShaderFileName = "VPLviaSVOGI/vLightMesh.glsl";
    lightMeshProgramInfo.FShaderFileName = "VPLviaSVOGI/fLightMesh.glsl";
    lightMeshProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                           ShaderType::ST_Fragment;
    Pass* passLightMesh = new Pass(lightMeshProgramInfo);
    Technique* techLightMesh = new Technique();
    techLightMesh->AddPass(passLightMesh);
    mMtLightMesh = new MaterialTemplate();
    mMtLightMesh->AddTechnique(techLightMesh);
}
//----------------------------------------------------------------------------
LightManager::~LightManager()
{
    for( int i = 0; i < (int)mPointLights.size(); ++i )
    {
        mPointLights[i] = 0;
    }

    for( int i = 0; i < (int)mSpotLights.size(); ++i )
    {
        mSpotLights[i] = 0;
    }

    delete[] mLightBufferCache;
    mLightBuffer = 0;

    mMtLightMesh = 0;
}
//----------------------------------------------------------------------------
void LightManager::CreatePointLight(LightProjectorDesc* desc, 
    Camera* lightMeshCamera)
{
    Material* lightMeshMaterial = new Material(mMtLightMesh);
    LightMesh* lightMesh = new LightMesh(lightMeshMaterial, lightMeshCamera);
    lightMesh->LoadFromPLYFile("square.ply");
    mat4 lightMeshScale = glm::scale(mat4(), vec3(0.05f));
    lightMesh->UpdateModelSpaceVertices(lightMeshScale);
    mat4 lightRotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
    lightMesh->SetWorldTransform(lightRotM);
    lightMesh->SetTCoord(0, vec2(0.0f, 0.0f));
    lightMesh->SetTCoord(1, vec2(1.0f, 0.0f));
    lightMesh->SetTCoord(2, vec2(1.0f, 1.0f));
    lightMesh->SetTCoord(3, vec2(0.0f, 1.0f));
    lightMesh->CreateDeviceResource(mDevice);

    lightMesh->LightMeshTexture = new Texture2D();
    lightMesh->LightMeshTexture->LoadPNGFromFile(mDevice, 
        "Textures/pointLight.png");

    Camera* lightProjector = new Camera();
    lightProjector->SetPerspectiveFrustum(desc->UpFovDegrees, 
        desc->AspectRatio, desc->NearPlane, desc->FarPlane);
    lightProjector->SetLookAt(desc->Location, desc->LookAt, desc->Up);
    Light* light = new Light();
    light->Intensity = vec3(50.0f);
    light->SetProjector(lightProjector);
    light->SetLightMesh(lightMesh);
    lightMesh->SetWorldTranslation(lightProjector->GetLocation());

    AddPointLight(light);
}
//----------------------------------------------------------------------------
void LightManager::AddPointLight(Light* light)
{
    RTGI_ASSERT((mPointLights.size() + mSpotLights.size()) < MAX_LIGHT_COUNT);
    mPointLights.push_back(light);
}
//----------------------------------------------------------------------------
Light* LightManager::GetPointLight(int i) const
{
    RTGI_ASSERT(i >= 0 && i < (int)mPointLights.size());
    return mPointLights[i];
}
//----------------------------------------------------------------------------
int LightManager::GetPointLightCount() const
{
    return mPointLights.size();
}
//----------------------------------------------------------------------------
void LightManager::AddSpotLight(Light* light)
{
    RTGI_ASSERT((mPointLights.size() + mSpotLights.size()) < MAX_LIGHT_COUNT);
    mSpotLights.push_back(light);
}
//----------------------------------------------------------------------------
Light* LightManager::GetSpotLight(int i) const
{
    RTGI_ASSERT(i >= 0 && i < (int)mSpotLights.size());
    return mSpotLights[i];
}
//----------------------------------------------------------------------------
int LightManager::GetSpotLightCount() const
{
    return mSpotLights.size();
}
//----------------------------------------------------------------------------
void LightManager::CreateLightBuffer(GPUDevice* device)
{
    if( mPointLights.size() > 0 || mSpotLights.size() > 0 )
    {
        mLightBuffer = new UniformBuffer();
        size_t bufferSize = sizeof(LightBufferHead) + 
            sizeof(SceneLight)*MAX_LIGHT_COUNT;
        mLightBuffer->ReserveMutableDeviceResource(device, bufferSize,
            BU_Dynamic_Copy);

        UpdateLightBuffer();
    }
}
//----------------------------------------------------------------------------
void LightManager::UpdateLightBuffer()
{
    LightBufferHead* info = (LightBufferHead*)mLightBufferCache;
    info->PointLightCount = (unsigned int)mPointLights.size();
    info->SpotLightCount = (unsigned int)mSpotLights.size();

    SceneLight* curLight = (SceneLight*)(mLightBufferCache + 
        sizeof(LightBufferHead));
    for( int i = 0; i < (int)mPointLights.size(); ++i )
    {
        mPointLights[i]->OnUpdateLightBufferCache(curLight);
        curLight++;
    }
    for( int i = 0; i < (int)mSpotLights.size(); ++i )
    {
        mSpotLights[i]->OnUpdateLightBufferCache(curLight);
        curLight++;
    }

    size_t bufferSize = sizeof(LightBufferHead) + 
        sizeof(SceneLight)*MAX_LIGHT_COUNT;
    mLightBuffer->UpdateSubData(mLightBufferBindingPoint, 0, bufferSize,
        (void*)mLightBufferCache);
}
//----------------------------------------------------------------------------
void LightManager::SetLightBufferBindingPoint(unsigned int bindingPoint)
{
    mLightBufferBindingPoint = bindingPoint;
}
//----------------------------------------------------------------------------
unsigned int LightManager::GetLightBufferBindingPoint() const
{
    return mLightBufferBindingPoint;
}
//----------------------------------------------------------------------------
void LightManager::RenderLightMesh(int technique, int pass, 
    SubRenderer* subRenderer)
{
    for( int i = 0; i < (int)mPointLights.size(); ++i )
    {
        mPointLights[i]->RenderLightMesh(technique, pass, subRenderer);
    }
    for( int i = 0; i < (int)mSpotLights.size(); ++i )
    {
        mSpotLights[i]->RenderLightMesh(technique, pass, subRenderer);
    }
}
//----------------------------------------------------------------------------