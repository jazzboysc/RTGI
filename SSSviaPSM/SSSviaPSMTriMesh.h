#ifndef RTGI_SSSviaPSMTriMesh_H
#define RTGI_SSSviaPSMTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/09/2014
//----------------------------------------------------------------------------
class SSSviaPSMTriMesh : public TriangleMesh
{
public:
	SSSviaPSMTriMesh(Material* material, Camera* camera);
	virtual ~SSSviaPSMTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	bool IsLight;
    bool IsSSS;
	vec3 MaterialColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;

private:
    // pass 1 uniform locations.
    GLint mLightProjectorNearFarLoc;

    // pass 2 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mLightProjectorViewLoc;
    GLint mShadowMapSamplerLoc;
    GLint mLightPositionWorldLoc;
    GLint mCameraPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mMaterialColorLoc;
    GLint mLightProjectorNearFarLoc2;
    GLint mIsSSSLoc;
};

typedef RefPointer<SSSviaPSMTriMesh> SSSviaPSMTriMeshPtr;

}

#endif