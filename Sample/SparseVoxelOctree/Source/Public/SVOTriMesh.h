#ifndef RTGI_SVOTriMesh_H
#define RTGI_SVOTriMesh_H

#include "TriangleMesh.h"
#include "Texture3D.h"
#include "AABB.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/24/2015
//----------------------------------------------------------------------------
class SVOTriMesh : public TriangleMesh
{
public:
	SVOTriMesh(Material* material, Camera* camera);
	virtual ~SVOTriMesh();

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
    ShaderUniform mInv2SceneBBExtensionLoc;

    // pass 2.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mSceneBBCenterLoc2;
    ShaderUniform mSceneBBExtensionLoc2;
    ShaderUniform mDimLoc2;
    ShaderUniform mShowWorldPositionLoc;
};

typedef RefPointer<SVOTriMesh> SVOTriMeshPtr;

}

#endif