#ifndef RTGI_SimpleVoxelizationTriMesh_H
#define RTGI_SimpleVoxelizationTriMesh_H

#include "TriangleMesh.h"
#include "Texture3D.h"
#include "AABB.h"

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

    AABB* SceneBB;
	vec3 MaterialColor;

private:
    // pass 1.
    GLint mSceneBBCenterLoc;
    GLint mSceneBBExtensionLoc;
    GLint mMaterialColorLoc;
    GLint mDimLoc;

    // pass 2.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mSceneBBCenterLoc2;
    GLint mSceneBBExtensionLoc2;
    GLint mDimLoc2;
};

typedef RefPointer<SimpleVoxelizationTriMesh> SimpleVoxelizationTriMeshPtr;

}

#endif