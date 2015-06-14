#include "ObjMesh.h"
#include "ObjLoadingTestApp.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ObjMesh::ObjMesh(Material* material, Camera* camera)
    :
    TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
ObjMesh::~ObjMesh()
{
}
//----------------------------------------------------------------------------
void ObjMesh::OnGetShaderConstants()
{
    TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void ObjMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        TriangleMesh::OnUpdateShaderConstants(technique, pass);
    }
}
//----------------------------------------------------------------------------