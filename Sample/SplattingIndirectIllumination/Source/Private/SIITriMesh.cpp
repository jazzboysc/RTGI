#include "SIITriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SIITriMesh::SIITriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    LightProjector = 0;
    ShadowMap = 0;
}
//----------------------------------------------------------------------------
SIITriMesh::~SIITriMesh()
{
}
//----------------------------------------------------------------------------
void SIITriMesh::OnGetShaderConstants()
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
    program->GetUniformLocation(&mMaterialColorLoc2, "MaterialColor");

    // Get pass 3 uniform locations.
    program = mMaterial->GetProgram(0, 2);
    program->GetUniformLocation(&mWorldLoc3, "World");
    program->GetUniformLocation(&mViewLoc3, "View");
    program->GetUniformLocation(&mProjLoc3, "Proj");
    program->GetUniformLocation(&mLightProjectorViewLoc3, "LightProjectorView");
    program->GetUniformLocation(&mShadowMapSamplerLoc3, "shadowMapSampler");
    program->GetUniformLocation(&mLightPositionWorldLoc3, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc3, "LightColor");
    program->GetUniformLocation(&mMaterialColorLoc3, "MaterialColor");
    program->GetUniformLocation(&mLightProjectorNearFarLoc3, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------
void SIITriMesh::OnUpdateShaderConstants(int technique, int pass)
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
		mWorldLoc2.SetValue(this->GetWorldTranslation());
        mMaterialColorLoc2.SetValue(MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);
        }
    }

    // Update pass 3 uniform data.
    if( pass == 2 )
    {
		mWorldLoc3.SetValue(this->GetWorldTranslation());
        mMaterialColorLoc3.SetValue(MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc3.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc3.SetValue(projTrans);
        }

        if( LightProjector )
        {
            mat4 viewTrans = LightProjector->GetViewTransform();
            mLightProjectorViewLoc3.SetValue(viewTrans);

            vec3 lightPosition = LightProjector->GetLocation();
            mLightPositionWorldLoc3.SetValue(lightPosition);

            float nearFarPlane[2];
            LightProjector->GetNearFarPlane(nearFarPlane);
            mLightProjectorNearFarLoc3.SetValue(nearFarPlane);
        }

        if( ShadowMap )
        {
            mShadowMapSamplerLoc3.SetValue(0);

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