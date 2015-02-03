//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "QuadMesh.h"
#include "GeometryAttributes.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
QuadMesh::QuadMesh(Material* material, Camera* camera)
	:
	RenderObject(material),
	mVertexCount(0),
	mFaceCount(0),
	mVertexComponentCount(0),
	mWorldScale(1.0f, 1.0f, 1.0f)
{
    mIsIndirect = false;
    mCommandOffset = 0;
    SetCamera(camera);
}
//----------------------------------------------------------------------------
QuadMesh::~QuadMesh()
{
    mIndirectCommandBuffer = 0;

	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);
}
//----------------------------------------------------------------------------
void QuadMesh::Render(int technique, int pass, SubRenderer* subRenderer)
{
	// Apply current rendering pass.
	mMaterial->Apply(technique, pass);
}
//----------------------------------------------------------------------------
void QuadMesh::OnEnableBuffers()
{
}
//----------------------------------------------------------------------------
void QuadMesh::OnDisableBuffers()
{
}
//----------------------------------------------------------------------------
void QuadMesh::OnRender(Pass* pass, PassInfo*)
{
    bool enableTessellation = pass->IsTessellationEnabled();

    if( enableTessellation )
    {
        // TODO:
        // Check this later.
        assert(false);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawElements(GL_PATCHES, (GLsizei)mIndexData.size(),
            GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        if( !mIsIndirect )
        {
            glDrawElements(GL_QUADS, (GLsizei)mIndexData.size(),
                GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            assert(mIndirectCommandBuffer != 0);
            glDrawElementsIndirect(GL_QUADS, GL_UNSIGNED_SHORT,
                (void*)mCommandOffset);
        }
    }

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void QuadMesh::OnUpdateShaderConstants(int technique, int pass)
{
    assert( technique == 0 && pass == 0 );
    
    GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc, mWorldTransform);
	if( mCamera )
	{
		glm::mat4 viewTrans = mCamera->GetViewTransform();
        GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc, viewTrans);

		glm::mat4 projTrans = mCamera->GetProjectionTransform();
        GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc, projTrans);
	}
}
//----------------------------------------------------------------------------
void QuadMesh::LoadFromSystemMemory(std::vector<glm::vec3>& vertexData,
    std::vector<unsigned short>& indexData)
{
    mVertexCount = (int)vertexData.size();
    mFaceCount = (int)(indexData.size() >> 2);
    mVertexComponentCount = 3;
    mVertexData = vertexData;
    mIndexData = indexData;
}
//----------------------------------------------------------------------------
void QuadMesh::CreateDeviceResource(GPUDevice* device)
{
	// Create VBO and IBO.
	CreateIndexBufferDeviceResource();
	CreateVertexBufferDeviceResource();

	// Create shader programs.
	GeometryAttributes attributes;
	attributes.VBO = mVBO;
	attributes.IBO = mIBO;
	attributes.HasNormal = false;
	attributes.HasTCoord = false;
	attributes.VertexComponentCount = mVertexComponentCount;
	mMaterial->CreateDeviceResource(device, &attributes);

	// Get shader constants here.
	OnGetShaderConstants();

    if( mIsIndirect )
    {
        assert(mIndirectCommandBuffer);
        mIndirectCommandBuffer->Bind();
        char* bufferData = (char*)mIndirectCommandBuffer->Map(BA_Write_Only);
        assert( bufferData );
        DrawElementsIndirectCommand* commandBufferData = 
            (DrawElementsIndirectCommand*)(bufferData + mCommandOffset);
        commandBufferData->Count = (unsigned int)mIndexData.size();
        commandBufferData->PrimCount = 0;
        commandBufferData->FirstIndex = 0;
        commandBufferData->BaseVertex = 0;
        commandBufferData->BaseInstance = 0;
        mIndirectCommandBuffer->Unmap();
    }
}
//----------------------------------------------------------------------------
void QuadMesh::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mWorldLoc, "World");
    program->GetUniformLocation(&mViewLoc, "View");
    program->GetUniformLocation(&mProjLoc, "Proj");
}
//----------------------------------------------------------------------------
void QuadMesh::SetWorldTransform(const glm::mat4& worldTrans)
{
	mWorldTransform = worldTrans * Offset;
}
//----------------------------------------------------------------------------
glm::mat4 QuadMesh::GetWorldTransform() const
{
	return mWorldTransform;
}
//----------------------------------------------------------------------------
void QuadMesh::SetWorldTranslation(const glm::vec3& translation)
{
	mWorldTransform[3][0] = translation.x;
	mWorldTransform[3][1] = translation.y;
	mWorldTransform[3][2] = translation.z;
}
//----------------------------------------------------------------------------
glm::vec3 QuadMesh::GetWorldTranslation() const
{
	glm::vec3 res;
	res.x = mWorldTransform[3][0];
	res.y = mWorldTransform[3][1];
	res.z = mWorldTransform[3][2];
	return res;
}
//----------------------------------------------------------------------------
void QuadMesh::SetWorldScale(const glm::vec3& scale)
{
	mWorldTransform[0][0] /= mWorldScale[0];
	mWorldTransform[0][1] /= mWorldScale[1];
	mWorldTransform[0][2] /= mWorldScale[2];
	mWorldTransform[1][0] /= mWorldScale[0];
	mWorldTransform[1][1] /= mWorldScale[1];
	mWorldTransform[1][2] /= mWorldScale[2];
	mWorldTransform[2][0] /= mWorldScale[0];
	mWorldTransform[2][1] /= mWorldScale[1];
	mWorldTransform[2][2] /= mWorldScale[2];
	mWorldTransform[0][0] *= scale[0];
	mWorldTransform[0][1] *= scale[1];
	mWorldTransform[0][2] *= scale[2];
	mWorldTransform[1][0] *= scale[0];
	mWorldTransform[1][1] *= scale[1];
	mWorldTransform[1][2] *= scale[2];
	mWorldTransform[2][0] *= scale[0];
	mWorldTransform[2][1] *= scale[1];
	mWorldTransform[2][2] *= scale[2];
	mWorldScale = scale;
}
//----------------------------------------------------------------------------
glm::vec3 QuadMesh::GetWorldScale() const
{
	return mWorldScale;
}
//----------------------------------------------------------------------------
int QuadMesh::GetVertexCount() const
{
	return mVertexCount;
}
//----------------------------------------------------------------------------
glm::vec3 QuadMesh::GetWorldVertex(int i) const
{
	assert( (int)mVertexData.size() > i );
	glm::vec4 temp = glm::vec4(mVertexData[i], 1.0f);
	temp = mWorldTransform * temp;
	glm::vec3 res;
	res.x = temp.x;
	res.y = temp.y;
	res.z = temp.z;
	return res;
}
//----------------------------------------------------------------------------
AABB QuadMesh::GetModelSpaceBB() const
{
	return mModelSpaceBB;
}
//----------------------------------------------------------------------------
AABB QuadMesh::GetWorldSpaceBB() const
{
	AABB res;
	glm::vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = glm::vec4(mVertexData[i], 1.0f);
		tempV = mWorldTransform * tempV;
		res.Min.x = RTGI_MIN(res.Min.x, tempV.x);
		res.Min.y = RTGI_MIN(res.Min.y, tempV.y);
		res.Min.z = RTGI_MIN(res.Min.z, tempV.z);
		res.Max.x = RTGI_MAX(res.Max.x, tempV.x);
		res.Max.y = RTGI_MAX(res.Max.y, tempV.y);
		res.Max.z = RTGI_MAX(res.Max.z, tempV.z);
	}
	return res;
}
//----------------------------------------------------------------------------
void QuadMesh::UpdateModelSpaceVertices(const glm::mat4& trans)
{
	glm::vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = glm::vec4(mVertexData[i], 1.0f);
		tempV = trans * tempV;
		mVertexData[i].x = tempV.x;
		mVertexData[i].y = tempV.y;
		mVertexData[i].z = tempV.z;
	}
	// Adjust bounding box.
	tempV = glm::vec4(mModelSpaceBB.Min, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Min.x = tempV.x;
	mModelSpaceBB.Min.y = tempV.y;
	mModelSpaceBB.Min.z = tempV.z;
	tempV = glm::vec4(mModelSpaceBB.Max, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Max.x = tempV.x;
	mModelSpaceBB.Max.y = tempV.y;
	mModelSpaceBB.Max.z = tempV.z;
}
//----------------------------------------------------------------------------
void QuadMesh::CreateVertexBufferDeviceResource()
{
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    size_t bufferSize = sizeof(float) * mVertexCount * mVertexComponentCount;
    if( mVertexData.size() > 0 )
    {
        glBufferData(GL_ARRAY_BUFFER, bufferSize, (GLvoid*)&mVertexData[0], 
            GL_STATIC_DRAW);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void QuadMesh::CreateIndexBufferDeviceResource()
{
	assert( mIndexData.size() > 0 );
	if( mIndexData.size() > 0 )
	{
		glGenBuffers(1, &mIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(unsigned short)*mIndexData.size(), (GLvoid*)&mIndexData[0],
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void QuadMesh::SetIndirectCommandBuffer(
    StructuredBuffer* indirectCommandBuffer, unsigned int commandOffset)
{
    mIsIndirect = true;
    mIndirectCommandBuffer = indirectCommandBuffer;
    mCommandOffset = commandOffset;
}
//----------------------------------------------------------------------------
bool QuadMesh::IsIndirect() const
{
    return mIsIndirect;
}
//----------------------------------------------------------------------------