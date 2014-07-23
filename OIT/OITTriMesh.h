#ifndef RTGI_OITTriMesh_H
#define RTGI_OITTriMesh_H

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/22/2014
//----------------------------------------------------------------------------
class OITTriMesh : public TriangleMesh
{
public:
	OITTriMesh(Material* material, Camera* camera);
	virtual ~OITTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);
};

typedef RefPointer<OITTriMesh> OITTriMeshPtr;

}

#endif