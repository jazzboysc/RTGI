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
    ShaderUniform mLightProjectorNearFarLoc;

    // pass 2 uniform locations.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mLightProjectorViewLoc;
    ShaderUniform mShadowMapSamplerLoc;
    ShaderUniform mLightPositionWorldLoc;
    ShaderUniform mCameraPositionWorldLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mMaterialColorLoc;
    ShaderUniform mLightProjectorNearFarLoc2;
    ShaderUniform mIsSSSLoc;
};

typedef RefPointer<SSSviaPSMTriMesh> SSSviaPSMTriMeshPtr;

}

#endif