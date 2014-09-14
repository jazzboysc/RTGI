#ifndef RTGI_VPLviaRSMTriMesh_H
#define RTGI_VPLviaRSMTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2014
//----------------------------------------------------------------------------
class VPLviaRSMTriMesh : public TriangleMesh
{
public:
	VPLviaRSMTriMesh(Material* material, Camera* camera);
	virtual ~VPLviaRSMTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	bool IsLight;
	vec3 MaterialColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;

private:
    // pass 1 uniform locations.
    GLint mLightProjectorNearFarLoc;

    // pass 2 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mMaterialColorLoc2;

    // pass 3 uniform locations.
    GLint mWorldLoc3, mViewLoc3, mProjLoc3;
    GLint mLightProjectorViewLoc3;
    GLint mShadowMapSamplerLoc3;
    GLint mLightPositionWorldLoc3;
    GLint mLightColorLoc3;
    GLint mMaterialColorLoc3;
    GLint mLightProjectorNearFarLoc3;
};

typedef RefPointer<VPLviaRSMTriMesh> VPLviaRSMTriMeshPtr;

}

#endif