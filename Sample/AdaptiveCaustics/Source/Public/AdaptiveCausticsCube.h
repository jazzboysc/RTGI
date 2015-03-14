#pragma once

#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class AdaptiveCausticsCube : public TriangleMesh
{
public:
	AdaptiveCausticsCube(Material* material, Camera* camera);
	virtual ~AdaptiveCausticsCube();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
	TextureCubePtr CubeTexture;

	LightPtr Light;

private:
	ShaderUniform mLightPositionLoc;
	ShaderUniform mLightViewLoc;
	ShaderUniform mLightProjLoc;
	ShaderUniform mLightColorLoc;

	ShaderUniform mMaterialColorLoc;
	ShaderUniform mVertexSplattingSamplerLoc;

	ShaderUniform mCubemapSampler;
};

typedef RefPointer<AdaptiveCausticsCube> AdaptiveCausticsCubePtr;

}
