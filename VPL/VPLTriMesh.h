#ifndef RTGI_VPLTriMesh_H
#define RTGI_VPLTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/21/2014
//----------------------------------------------------------------------------
class VPLTriMesh : public TriangleMesh
{
public:
	VPLTriMesh(Material* material, Camera* camera);
	virtual ~VPLTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
    vec3 LightColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;
    StructuredBufferPtr VPLBuffer;

private:
    // pass 1, scene shadow pass.
    ShaderUniform mLightProjectorNearFarLoc;

    // pass 2, G-buffer pass.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mMaterialColorLoc2;

    // pass 3, RSM pass.
    ShaderUniform mWorldLoc3, mProjLoc3;
    ShaderUniform mLightPositionWorldLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mMaterialColorLoc3;

    // pass 4, VPL shadow pass.
    ShaderUniform mWorldLoc4;
    ShaderUniform mLightProjectorNearFarLoc4;
};

typedef RefPointer<VPLTriMesh> VPLTriMeshPtr;

}

#endif