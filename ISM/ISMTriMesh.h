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
    // pass 2 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
    GLint mLightProjectorViewLoc;
    GLint mShadowMapSamplerLoc;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mMaterialColorLoc;
};

typedef RefPointer<ISMTriMesh> ISMTriMeshPtr;

}

#endif