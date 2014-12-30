#pragma once

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class CausticsCube : public TriangleMesh
{
public:
	CausticsCube(Material* material, Camera* camera);
	virtual ~CausticsCube();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
	TextureCubePtr CubeTexture;

private:
	ShaderUniform mMaterialColorLoc;

	ShaderUniform mCubemapSampler;
};

typedef RefPointer<CausticsCube> CausticsCubePtr;

}
