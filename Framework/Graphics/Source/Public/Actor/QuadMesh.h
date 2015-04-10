//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_QuadMesh_H
#define RTGI_QuadMesh_H

#include "RenderObject.h"
#include "Camera.h"
#include "StructuredBuffer.h"
#include "GPUResource.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/03/2015
//----------------------------------------------------------------------------
class QuadMesh : public RenderObject
{
public:
    QuadMesh(Material* material, Camera* camera);
    virtual ~QuadMesh();

	// Implement base class interface.
	virtual void Render(int technique, int pass, SubRenderer* subRenderer = 0);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnEnableBuffers();
	virtual void OnDisableBuffers();

    // Load data from user specified memory location.
    void LoadFromSystemMemory(std::vector<glm::vec3>& vertexData, 
        std::vector<unsigned int>& indexData);

	// Should be called after calling LoadFromSystemMemory.
    void CreateDeviceResource(GPUDevice* device);
	virtual void OnGetShaderConstants();

	int GetVertexCount() const;
	glm::vec3 GetWorldVertex(int i) const;
	
	AABB GetModelSpaceBB() const;
	AABB GetWorldSpaceBB() const;

	void UpdateModelSpaceVertices(const glm::mat4& trans);

	glm::mat4 Offset;

    void SetIndirectCommandBuffer(StructuredBuffer* indirectCommandBuffer, 
        unsigned int commandOffset);
    bool IsIndirect() const;

protected:
    void CreateVertexBufferDeviceResource(GPUDevice* device);
    void CreateIndexBufferDeviceResource(GPUDevice* device);

    bool mIsIndirect;
    StructuredBufferPtr mIndirectCommandBuffer;
    BufferViewPtr mIndirectCommandBufferView;
    unsigned int mCommandOffset;

	AABB mModelSpaceBB;

	int mVertexCount;
	int mFaceCount;
	int mVertexComponentCount;
	std::vector<glm::vec3> mVertexData;
	std::vector<unsigned int> mIndexData;

    ShaderUniform mWorldLoc;
    ShaderUniform mViewLoc;
    ShaderUniform mProjLoc;
};

typedef RefPointer<QuadMesh> QuadMeshPtr;

}

#endif