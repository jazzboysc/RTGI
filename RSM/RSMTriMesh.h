#ifndef RTGI_RSMTriMesh_H
#define RTGI_RSMTriMesh_H

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMTriMesh : public TriangleMesh
{
public:
	RSMTriMesh(Material* material, Camera* camera);
	virtual ~RSMTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;

private:
    // pass 1 uniform locations.
	GLint mMaterialColorLoc;
    
    // pass 2 uniform locations.
    GLint mWorldLoc2, mViewLoc2, mProjLoc2;
};

typedef RefPointer<RSMTriMesh> RSMTriMeshPtr;

}

#endif