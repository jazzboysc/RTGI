#ifndef RTGI_RayBundleTriMesh_H
#define RTGI_RayBundleTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/31/2014
//----------------------------------------------------------------------------
class RayBundleTriMesh : public TriangleMesh
{
public:
	RayBundleTriMesh(Material* material, Camera* camera);
	virtual ~RayBundleTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	bool IsLight;
	vec3 MaterialColor;
    vec3 EmissionColor;

private:
	// pass 1 uniform locations.
	GLint mIsLightLoc0;

    // pass 2 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
	GLint mIsLightLoc;
	GLint mMaterialColorLoc;
	GLint mEmissionColorLoc;
};

typedef RefPointer<RayBundleTriMesh> RayBundleTriMeshPtr;

}

#endif