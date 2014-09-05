#include "ISMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ISMTriMesh::ISMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
ISMTriMesh::~ISMTriMesh()
{
}
//----------------------------------------------------------------------------
void ISMTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
}
//----------------------------------------------------------------------------
void ISMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);
	}
}
//----------------------------------------------------------------------------