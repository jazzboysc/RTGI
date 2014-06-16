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
	virtual void OnUpdateShaderConstants();

	vec3 MaterialColor;

private:
	GLint mMaterialColorLoc;
};

typedef RefPointer<RSMTriMesh> RSMTriMeshPtr;

}

#endif