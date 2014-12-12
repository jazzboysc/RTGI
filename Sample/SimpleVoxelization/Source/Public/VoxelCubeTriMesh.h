#ifndef RTGI_VoxelCubeTriMesh_H
#define RTGI_VoxelCubeTriMesh_H

#include "TriangleMesh.h"
#include "Texture3D.h"
#include "AABB.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2014
//----------------------------------------------------------------------------
class VoxelCubeTriMesh : public TriangleMesh
{
public:
    VoxelCubeTriMesh(Material* material, Camera* camera);
    virtual ~VoxelCubeTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;

};

typedef RefPointer<VoxelCubeTriMesh> VoxelCubeTriMeshPtr;

}

#endif