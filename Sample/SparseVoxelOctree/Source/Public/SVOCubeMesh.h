#ifndef RTGI_SVOCubeMesh_H
#define RTGI_SVOCubeMesh_H

#include "QuadMesh.h"
#include "AABB.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/02/2015
//----------------------------------------------------------------------------
class SVOCubeMesh : public QuadMesh
{
public:
    SVOCubeMesh(Material* material, Camera* camera);
    virtual ~SVOCubeMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

    AABB* SceneBB;

private:
    ShaderUniform mSceneBBMinLoc;
    ShaderUniform mVoxelExtensionLoc;
};

typedef RefPointer<SVOCubeMesh> SVOCubeMeshPtr;

}

#endif