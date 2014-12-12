#include "CornellTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CornellTriMesh::CornellTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
CornellTriMesh::~CornellTriMesh()
{
}
//----------------------------------------------------------------------------
void CornellTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
}
//----------------------------------------------------------------------------
void CornellTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);
	}
}
//----------------------------------------------------------------------------