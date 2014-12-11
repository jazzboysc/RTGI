#ifndef RTGI_SIITriMesh_H
#define RTGI_SIITriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/12/2014
//----------------------------------------------------------------------------
class SIITriMesh : public TriangleMesh
{
public:
	SIITriMesh(Material* material, Camera* camera);
	virtual ~SIITriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	bool IsLight;
	vec3 MaterialColor;
    Camera* LightProjector;
    Texture2DPtr ShadowMap;

private:
    // pass 1 uniform locations.
    ShaderUniform mLightProjectorNearFarLoc;

    // pass 2 uniform locations.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mMaterialColorLoc2;

    // pass 3 uniform locations.
    ShaderUniform mWorldLoc3, mViewLoc3, mProjLoc3;
    ShaderUniform mLightProjectorViewLoc3;
    ShaderUniform mShadowMapSamplerLoc3;
    ShaderUniform mLightPositionWorldLoc3;
    ShaderUniform mLightColorLoc3;
    ShaderUniform mMaterialColorLoc3;
    ShaderUniform mLightProjectorNearFarLoc3;
};

typedef RefPointer<SIITriMesh> SIITriMeshPtr;

}

#endif