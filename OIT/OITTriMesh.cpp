#include "OITTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
OITTriMesh::OITTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
OITTriMesh::~OITTriMesh()
{
}
//----------------------------------------------------------------------------
void OITTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void OITTriMesh::OnUpdateShaderConstants(int technique,
    int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------