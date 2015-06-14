#include "ObjLoadingTestApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ObjLoadingTestApp::ObjLoadingTestApp(int width, int height)
{
    Width = width;
    Height = height;
    Title = "Obj Loading Test";
}
//----------------------------------------------------------------------------
ObjLoadingTestApp::~ObjLoadingTestApp()
{
}
//----------------------------------------------------------------------------
void ObjLoadingTestApp::Initialize(GPUDevice* device)
{
    float color = 0.0f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Create scene camera.
    mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.01f, 150.0f);
    mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create material templates.
    Material* material = 0;

    ShaderProgramInfo objMeshProgramInfo;
    objMeshProgramInfo.VShaderFileName = "ObjLoadingTest/vObjMesh.glsl";
    objMeshProgramInfo.FShaderFileName = "ObjLoadingTest/fObjMesh.glsl";
    objMeshProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passObjMesh = new Pass(objMeshProgramInfo);

    Technique* techObjMesh = new Technique();
    techObjMesh->AddPass(passObjMesh);
    MaterialTemplate* mtObjMesh = new MaterialTemplate();
    mtObjMesh->AddTechnique(techObjMesh);

    // Create scene.
    mat4 rotM;
    material = new Material(mtObjMesh);
    mModel = new ObjMesh(material, mMainCamera);
    mModel->LoadFromOBJFile("elephant-gallop/elephant-gallop-1.obj");
    mModel->GenerateNormals();
    mModel->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-60.0f), vec3(0, 1, 0));
    mModel->SetWorldTransform(rotM);
    mModel->SetWorldTranslation(vec3(3.2f, 3.6f, 2.4f));
    mModel->SetWorldScale(vec3(9.0f));
}
//----------------------------------------------------------------------------
void ObjLoadingTestApp::FrameFunc()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void ObjLoadingTestApp::Terminate()
{
    mModel = 0;
}
//----------------------------------------------------------------------------
void ObjLoadingTestApp::ProcessInput()
{
}
//----------------------------------------------------------------------------