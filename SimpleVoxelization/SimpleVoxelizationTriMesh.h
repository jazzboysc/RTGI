#ifndef RTGI_SimpleVoxelizationTriMesh_H
#define RTGI_SimpleVoxelizationTriMesh_H

#include "TriangleMesh.h"
#include "Texture3D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/08/2014
//----------------------------------------------------------------------------
class SimpleVoxelizationTriMesh : public TriangleMesh
{
public:
	SimpleVoxelizationTriMesh(Material* material, Camera* camera);
	virtual ~SimpleVoxelizationTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

    Texture3DPtr SceneVoxels;
	vec3 MaterialColor;
};

typedef RefPointer<SimpleVoxelizationTriMesh> SimpleVoxelizationTriMeshPtr;

}

#endif