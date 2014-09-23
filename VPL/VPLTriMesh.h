#ifndef RTGI_VPLTriMesh_H
#define RTGI_VPLTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

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

	bool IsLight;
	vec3 MaterialColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;

private:
    // pass 1, scene shadow pass.
    GLint mLightProjectorNearFarLoc;

    // pass 2, G-buffer pass.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mMaterialColorLoc;

    // pass 3, RSM pass.
    GLint mWorldLoc3, mProjLoc3;
    GLint mLightPositionWorldLoc;
};

typedef RefPointer<VPLTriMesh> VPLTriMeshPtr;

}

#endif