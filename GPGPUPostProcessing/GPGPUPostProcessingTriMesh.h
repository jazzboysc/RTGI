#ifndef RTGI_GPGPUPostProcessingTriMesh_H
#define RTGI_GPGPUPostProcessingTriMesh_H

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/15/2014
//----------------------------------------------------------------------------
class GPGPUPostProcessingTriMesh : public TriangleMesh
{
public:
	GPGPUPostProcessingTriMesh(Material* material, Camera* camera);
	virtual ~GPGPUPostProcessingTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;

private:
	GLint mMaterialColorLoc;
};

typedef RefPointer<GPGPUPostProcessingTriMesh> GPGPUPostProcessingTriMeshPtr;

}

#endif