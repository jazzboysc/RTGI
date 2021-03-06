#ifndef RTGI_RSMTriMesh_H
#define RTGI_RSMTriMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMTriMesh : public TriangleMesh
{
public:
	RSMTriMesh(Material* material, Camera* camera);
	virtual ~RSMTriMesh();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

	vec3 MaterialColor;
    
    Camera* LightProjector;
    
    float SampleRadius;
    int SampleCount;
    
    Texture2DPtr RSMPositionTexture;
    Texture2DPtr RSMNormalTexture;
    Texture2DPtr RSMFluxTexture;
    Texture2DPtr RSMSamplingPatternTexture;

private:
    // pass 1 uniform locations.
	ShaderUniform mMaterialColorLoc;
    
    // pass 2 uniform locations.
    ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
    ShaderUniform mMaterialColorLoc2;
    ShaderUniform mLightProjectorViewLoc, mLightProjectorProjLoc;
    ShaderUniform mSampleRadiusLoc, mSampleCountLoc;
    ShaderUniform mPositionSamplerLoc, mNormalSamplerLoc, mFluxSamplerLoc;
    ShaderUniform mSamplingPatternSamplerLoc;
    
};

typedef RefPointer<RSMTriMesh> RSMTriMeshPtr;

}

#endif