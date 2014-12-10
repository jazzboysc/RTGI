#include "RSMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMTriMesh::RSMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
RSMTriMesh::~RSMTriMesh()
{
    RSMPositionTexture = 0;
    RSMNormalTexture = 0;
    RSMFluxTexture = 0;
    RSMSamplingPatternTexture = 0;
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

    // Get pass 1 uniform locations.
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
    
    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mWorldLoc2, "World");
    program->GetUniformLocation(&mViewLoc2, "View");
    program->GetUniformLocation(&mProjLoc2, "Proj");
    program->GetUniformLocation(&mMaterialColorLoc2, "materialColor");
    program->GetUniformLocation(&mLightProjectorViewLoc, "lightProjectorView");
    program->GetUniformLocation(&mLightProjectorProjLoc, "lightProjectorProj");
    program->GetUniformLocation(&mSampleRadiusLoc, "sampleRadius");
    program->GetUniformLocation(&mSampleCountLoc, "sampleCount");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mFluxSamplerLoc, "fluxSampler");
    program->GetUniformLocation(&mSamplingPatternSamplerLoc, "samplingPatternSampler");
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        TriangleMesh::OnUpdateShaderConstants(technique, pass);
        mMaterialColorLoc.SetValue(MaterialColor);
    }
    
    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        mWorldLoc2.SetValue(mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);
            
            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);
        }
        
        mMaterialColorLoc2.SetValue(MaterialColor);
        
        if( LightProjector )
        {
            mat4 viewTrans = LightProjector->GetViewTransform();
            mLightProjectorViewLoc.SetValue(viewTrans);
            
            mat4 projTrans = LightProjector->GetProjectionTransform();
            mLightProjectorProjLoc.SetValue(projTrans);
        }
        
        mSampleRadiusLoc.SetValue(SampleRadius);
        mSampleCountLoc.SetValue(SampleCount);
        mPositionSamplerLoc.SetValue(0);
        mNormalSamplerLoc.SetValue(1);
        mFluxSamplerLoc.SetValue(2);
        mSamplingPatternSamplerLoc.SetValue(3);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RSMPositionTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RSMNormalTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, RSMFluxTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, RSMSamplingPatternTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}
//----------------------------------------------------------------------------