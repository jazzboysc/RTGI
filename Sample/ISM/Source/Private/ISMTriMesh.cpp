#include "ISMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ISMTriMesh::ISMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    LightProjector = 0;
}
//----------------------------------------------------------------------------
ISMTriMesh::~ISMTriMesh()
{
    ShadowMap = 0;
}
//----------------------------------------------------------------------------
void ISMTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mWorldLoc2, "World");
    program->GetUniformLocation(&mViewLoc2, "View");
    program->GetUniformLocation(&mProjLoc2, "Proj");
    program->GetUniformLocation(&mLightProjectorViewLoc, "LightProjectorView");
    program->GetUniformLocation(&mShadowMapSamplerLoc, "shadowMapSampler");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mMaterialColorLoc, "MaterialColor");
    program->GetUniformLocation(&mLightProjectorNearFarLoc2, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------
void ISMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        if( mCamera )
        {
            float nearFarPlane[2];
            mCamera->GetNearFarPlane(nearFarPlane);
            mLightProjectorNearFarLoc.SetValue(nearFarPlane);
        }
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        mWorldLoc2.SetValue(mWorldTransform);
        mMaterialColorLoc.SetValue(MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);
        }

        if( LightProjector )
        {
            mat4 viewTrans = LightProjector->GetViewTransform();
            mLightProjectorViewLoc.SetValue(viewTrans);

            vec3 lightPosition = LightProjector->GetLocation();
            mLightPositionWorldLoc.SetValue(lightPosition);

            float nearFarPlane[2];
            LightProjector->GetNearFarPlane(nearFarPlane);
            mLightProjectorNearFarLoc2.SetValue(nearFarPlane);
        }

        if( ShadowMap )
        {
            mShadowMapSamplerLoc.SetValue(0);

            SamplerDesc sampler;
            sampler.MinFilter = FT_Nearest;
            sampler.MagFilter = FT_Nearest;
            sampler.WrapS = WT_Clamp;
            sampler.WrapT = WT_Clamp;

            ShadowMap->BindToSampler(0, &sampler);
        }
    }
}
//----------------------------------------------------------------------------