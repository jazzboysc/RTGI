#pragma once

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
	class AdaptiveCausticsTriMesh : public TriangleMesh
{
public:
	AdaptiveCausticsTriMesh(Material* material, Camera* camera);
	virtual ~AdaptiveCausticsTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
	Texture2DPtr Texture;

	float TessLevel = 14.0f;

private:
	ShaderUniform mWorldLoc2;
	ShaderUniform mViewLoc2;
	ShaderUniform mProjLoc2;

	ShaderUniform mTessLevelLoc;

	ShaderUniform mMaterialColorLoc;
	ShaderUniform mLightProjectorNearFarLoc;
};

	typedef RefPointer<AdaptiveCausticsTriMesh> CausticsTriMeshPtr;

}
