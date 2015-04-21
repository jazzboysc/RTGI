#include "LightMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
LightMesh::LightMesh(Material* material, Camera* camera)
    :
    TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
LightMesh::~LightMesh()
{
    LightMeshTexture = 0;
}
//----------------------------------------------------------------------------
void LightMesh::OnGetShaderConstants()
{
    TriangleMesh::OnGetShaderConstants();

    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightMeshTextureSamplerLoc, 
        "LightMeshTexture5");
}
//----------------------------------------------------------------------------
void LightMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    mLightMeshTextureSamplerLoc.SetValue(0);
    LightMeshTexture->BindToSampler(0, &sampler);
}
//----------------------------------------------------------------------------
void LightMesh::OnRender(Pass* pass, PassInfo* passInfo)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TriangleMesh::OnRender(pass, passInfo);

    glDisable(GL_BLEND);
}
//----------------------------------------------------------------------------