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
}
//----------------------------------------------------------------------------
void LightMesh::OnGetShaderConstants()
{
    TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void LightMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------