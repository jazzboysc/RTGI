#pragma once

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
	class CausticsTriMesh : public TriangleMesh
{
public:
	CausticsTriMesh(Material* material, Camera* camera);
	virtual ~CausticsTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
	Texture2DPtr Texture;

private:
	ShaderUniform mMaterialColorLoc;
};

	typedef RefPointer<CausticsTriMesh> CausticsTriMeshPtr;

}
