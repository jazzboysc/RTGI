#ifndef RTGI_UpdateAccumulationScreenQuad_H
#define RTGI_UpdateAccumulationScreenQuad_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/04/2014
//----------------------------------------------------------------------------
class UpdateAccumulationScreenQuad : public ScreenQuad
{
public:
	UpdateAccumulationScreenQuad(Material* material);
	~UpdateAccumulationScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	vec3 WorldRayBundleDirection;
	vec3 VoxelGridCenter;

private:
	ShaderUniform mWorldRayBundleDirectionLoc;
    ShaderUniform mVoxelGridCenterLoc;
};

typedef RefPointer<UpdateAccumulationScreenQuad> UpdateAccumulationScreenQuadPtr;

}

#endif