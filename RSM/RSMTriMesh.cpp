#include "RSMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMTriMesh::RSMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f),
	mMaterialColorLoc(0)
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
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
    
    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
	mWorldLoc2 = glGetUniformLocation(program, "World");
	mViewLoc2 = glGetUniformLocation(program, "View");
	mProjLoc2 = glGetUniformLocation(program, "Proj");
    mMaterialColorLoc2 = glGetUniformLocation(program, "materialColor");
    mLightProjectorViewLoc = glGetUniformLocation(program, "lightProjectorView");
    mLightProjectorProjLoc = glGetUniformLocation(program, "lightProjectorProj");
    mSampleRadiusLoc = glGetUniformLocation(program, "sampleRadius");
    mSampleCountLoc = glGetUniformLocation(program, "sampleCount");
    mPositionSamplerLoc = glGetUniformLocation(program, "positionSampler");
    mNormalSamplerLoc = glGetUniformLocation(program, "normalSampler");
    mFluxSamplerLoc = glGetUniformLocation(program, "fluxSampler");
    mSamplingPatternSamplerLoc = glGetUniformLocation(program, "samplingPatternSampler");
}
//----------------------------------------------------------------------------
void RSMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
        TriangleMesh::OnUpdateShaderConstants(technique, pass);
        glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
    }
    
    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        glUniformMatrix4fv(mWorldLoc2, 1, GL_TRUE, mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);
            
            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);
        }
        
        glUniform3fv(mMaterialColorLoc2, 1, (GLfloat*)&MaterialColor);
        
        if( LightProjector )
        {
            mat4 viewTrans = LightProjector->GetViewTransform();
            glUniformMatrix4fv(mLightProjectorViewLoc, 1, GL_TRUE, viewTrans);
            
            mat4 projTrans = LightProjector->GetProjectionTransform();
            glUniformMatrix4fv(mLightProjectorProjLoc, 1, GL_TRUE, projTrans);
        }
        
        glUniform1f(mSampleRadiusLoc, SampleRadius);
        glUniform1i(mSampleCountLoc, SampleCount);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RSMPositionTexture->GetTexture());
        glUniform1i(mPositionSamplerLoc, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RSMNormalTexture->GetTexture());
        glUniform1i(mNormalSamplerLoc, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, RSMFluxTexture->GetTexture());
        glUniform1i(mFluxSamplerLoc, 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, RSMSamplingPatternTexture->GetTexture());
        glUniform1i(mSamplingPatternSamplerLoc, 3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}
//----------------------------------------------------------------------------