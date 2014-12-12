#ifndef RTGI_SimpleVoxelizationTriMesh_H
#define RTGI_SimpleVoxelizationTriMesh_H

#include "TriangleMesh.h"
#include "Texture3D.h"
#include "AABB.h"
#include "ShaderUniform.h"

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
    ShaderUniform mSceneBBCenterLoc;
    ShaderUniform mSceneBBExtensionLoc;
    ShaderUniform mMaterialColorLoc;
    ShaderUniform mDimLoc;
    ShaderUniform mShowWorldPositionLoc;
    ShaderUniform mInv2SceneBBExtensionLoc;

    // pass 2.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mSceneBBCenterLoc2;
    ShaderUniform mSceneBBExtensionLoc2;
    ShaderUniform mDimLoc2;
};

typedef RefPointer<SimpleVoxelizationTriMesh> SimpleVoxelizationTriMeshPtr;

}

#endif