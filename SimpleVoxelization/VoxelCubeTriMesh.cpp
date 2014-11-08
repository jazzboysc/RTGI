#include "VoxelCubeTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VoxelCubeTriMesh::VoxelCubeTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
VoxelCubeTriMesh::~VoxelCubeTriMesh()
{
}
//----------------------------------------------------------------------------
void VoxelCubeTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void VoxelCubeTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------