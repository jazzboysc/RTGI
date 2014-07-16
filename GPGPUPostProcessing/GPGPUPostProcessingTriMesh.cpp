#include "GPGPUPostProcessingTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUPostProcessingTriMesh::GPGPUPostProcessingTriMesh(Material* material,
    Camera* camera)
	:
	TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
GPGPUPostProcessingTriMesh::~GPGPUPostProcessingTriMesh()
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingTriMesh::OnUpdateShaderConstants(int technique,
    int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------