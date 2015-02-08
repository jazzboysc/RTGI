#pragma once

#include "PointSet.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
	class VertexGrid : public PointSet
{
public:
	VertexGrid(int width, int height, Material* material);
	virtual ~VertexGrid();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	// Pass 0
	Texture2DPtr IntersectionPositionTexture;
	Texture2DPtr CausticsMapTexture;
	int CausticsMapsResolution;

	vec3 MaterialColor;
	Texture2DPtr Texture;

	LightPtr Light;

private:
	// Pass 0
	ShaderUniform mCubeTextureLoc2;

	ShaderUniform mCausticsMapResolutionLoc;
	ShaderUniform mCausticsMapSamplerLoc;
	ShaderUniform mIntersectionPositionSamplerLoc;

	ShaderUniform mWorldLoc, mViewLoc, mProjLoc;
	ShaderUniform mLightViewLoc;
	ShaderUniform mLightProjLoc;

	ShaderUniform mMaterialColorLoc;

};

	typedef RefPointer<VertexGrid> VertexGridPtr;

}
