#ifndef RTGI_ISMTriMesh_H
#define RTGI_ISMTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class ISMTriMesh : public TriangleMesh
{
public:
	ISMTriMesh(Material* material, Camera* camera);
	virtual ~ISMTriMesh();

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
    ShaderUniform mLightProjectorViewLoc;
    ShaderUniform mShadowMapSamplerLoc;
    ShaderUniform mLightPositionWorldLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mMaterialColorLoc;
    ShaderUniform mLightProjectorNearFarLoc2;
};

typedef RefPointer<ISMTriMesh> ISMTriMeshPtr;

}

#endif