#include "SVOCubeTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SVOCubeTriMesh::SVOCubeTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SVOCubeTriMesh::~SVOCubeTriMesh()
{
}
//----------------------------------------------------------------------------
void SVOCubeTriMesh::OnGetShaderConstants()
{
    TriangleMesh::OnGetShaderConstants();

    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mVoxelExtensionLoc, "VoxelExtension");
}
//----------------------------------------------------------------------------
void SVOCubeTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);

    vec3 sceneBBMin = SceneBB->Min;
    vec3 voxelExtension = (SceneBB->Max - SceneBB->Min);
    voxelExtension /= 128;

    mSceneBBMinLoc.SetValue(sceneBBMin);
    mVoxelExtensionLoc.SetValue(voxelExtension);
}
//----------------------------------------------------------------------------