#include "SimpleVoxelizationTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::SimpleVoxelizationTriMesh(Material* material, 
    Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::~SimpleVoxelizationTriMesh()
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
}
//----------------------------------------------------------------------------
void SimpleVoxelizationTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);
	}
}
//----------------------------------------------------------------------------