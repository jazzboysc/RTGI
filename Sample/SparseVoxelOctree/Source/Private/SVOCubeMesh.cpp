#include "SVOCubeMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SVOCubeMesh::SVOCubeMesh(Material* material, Camera* camera)
	:
	QuadMesh(material, camera)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SVOCubeMesh::~SVOCubeMesh()
{
}
//----------------------------------------------------------------------------
void SVOCubeMesh::OnGetShaderConstants()
{
    QuadMesh::OnGetShaderConstants();

    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mVoxelExtensionLoc, "VoxelExtension");
}
//----------------------------------------------------------------------------
void SVOCubeMesh::OnUpdateShaderConstants(int technique, int pass)
{
    QuadMesh::OnUpdateShaderConstants(technique, pass);

    vec3 sceneBBMin = SceneBB->Min;
    vec3 voxelExtension = (SceneBB->Max - SceneBB->Min);
    voxelExtension /= 128;

    mSceneBBMinLoc.SetValue(sceneBBMin);
    mVoxelExtensionLoc.SetValue(voxelExtension);
}
//----------------------------------------------------------------------------