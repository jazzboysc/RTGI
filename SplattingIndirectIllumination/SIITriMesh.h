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
    GLint mLightProjectorNearFarLoc;

    // pass 3 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mLightProjectorViewLoc;
    GLint mShadowMapSamplerLoc;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mMaterialColorLoc;
    GLint mLightProjectorNearFarLoc2;
};

typedef RefPointer<SIITriMesh> SIITriMeshPtr;

}

#endif