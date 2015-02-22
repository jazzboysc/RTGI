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
	Texture2DPtr VertexSplattingTexture;

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

typedef RefPointer<CausticsCube> CausticsCubePtr;

}
