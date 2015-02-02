#ifndef RTGI_SVOCubeTriMesh_H
#define RTGI_SVOCubeTriMesh_H

#include "TriangleMesh.h"
#include "AABB.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/02/2015
//----------------------------------------------------------------------------
class SVOCubeTriMesh : public TriangleMesh
{
public:
    SVOCubeTriMesh(Material* material, Camera* camera);
    virtual ~SVOCubeTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

    AABB* SceneBB;

private:
    ShaderUniform mSceneBBMinLoc;
    ShaderUniform mVoxelExtensionLoc;
};

typedef RefPointer<SVOCubeTriMesh> SVOCubeTriMeshPtr;

}

#endif