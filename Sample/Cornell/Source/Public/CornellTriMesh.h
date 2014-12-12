#ifndef RTGI_CornellTriMesh_H
#define RTGI_CornellTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class CornellTriMesh : public TriangleMesh
{
public:
	CornellTriMesh(Material* material, Camera* camera);
	virtual ~CornellTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	bool IsLight;
	vec3 MaterialColor;
};

typedef RefPointer<CornellTriMesh> CornellTriMeshPtr;

}

#endif