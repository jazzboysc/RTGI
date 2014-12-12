#ifndef RTGI_SSDOTriMesh_H
#define RTGI_SSDOTriMesh_H

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class SSDOTriMesh : public TriangleMesh
{
public:
	SSDOTriMesh(Material* material, Camera* camera);
	virtual ~SSDOTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;

private:
	ShaderUniform mMaterialColorLoc;
};

typedef RefPointer<SSDOTriMesh> SSDOTriMeshPtr;

}

#endif