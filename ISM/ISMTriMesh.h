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
};

typedef RefPointer<ISMTriMesh> ISMTriMeshPtr;

}

#endif