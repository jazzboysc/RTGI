//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TriangleMesh_H
#define RTGI_TriangleMesh_H


#include "RenderObject.h"
#include "Camera.h"
#include "AABB.h"
#include "StructuredBuffer.h"
#include "GPUResource.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/28/2013
//----------------------------------------------------------------------------
class TriangleMesh : public RenderObject
{
public:
	TriangleMesh(Material* material, Camera* camera);
	virtual ~TriangleMesh();

	// Implement base class interface.
	virtual void Render(int technique, int pass, SubRenderer* subRenderer = 0);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnEnableBuffers();
	virtual void OnDisableBuffers();

	// Load data from a ".ply" file and create its VBO and IBO.
	bool LoadFromFile(const std::string& fileName);
	bool LoadFromMemory(std::vector<glm::vec3>& _vData,
						std::vector<unsigned short>& _iData,
						std::vector<glm::vec3>& _nData);
	virtual void OnLoadFromFile();

	// Should be called after calling LoadFromFile or LoadFromMemory.
    void CreateDeviceResource(GPUDevice* device);
	virtual void OnGetShaderConstants();

	void SetWorldTransform(const glm::mat4& worldTrans);
	glm::mat4 GetWorldTransform() const;
	void SetWorldTranslation(const glm::vec3& translation);
	glm::vec3 GetWorldTranslation() const;
	void SetWorldScale(const glm::vec3& scale);
	glm::vec3 GetWorldScale() const;

	void GenerateNormals();
	virtual void OnGenerateNormals();

	void SetTCoord(int i, const glm::vec2& texCoord);
	glm::vec2 GetTCoord(int i) const;

	int GetVertexCount() const;
	glm::vec3 GetWorldVertex(int i) const;
	
	AABB GetModelSpaceBB() const;
	AABB GetWorldSpaceBB() const;

	void UpdateModelSpaceVertices(const glm::mat4& trans);

    // TODO:
    // Modify TriangleMesh to support quad mesh and change the name to Mesh.
    bool IsQuad;

	glm::mat4 Offset;

    unsigned int InstanceCount;
    bool IsIndirect;
    StructuredBufferPtr IndirectCommandBuffer;

protected:
	void CreateVertexBufferDeviceResource();
	void CreateIndexBufferDeviceResource();

	AABB mModelSpaceBB;

	int mVertexCount;
	int mFaceCount;
	int mVertexComponentCount;
	std::vector<glm::vec3> mVertexData;
	std::vector<glm::vec2> mTCoordData;
	std::vector<glm::vec3> mVertexNormalData;
	std::vector<glm::vec3> mFaceNormalData;
	std::vector<unsigned short> mIndexData;

	bool mHasTCoord;
	bool mHasNormal;

	glm::mat4 mWorldTransform;
	glm::vec3 mWorldScale;

	GLuint mIBO;
    ShaderUniform mWorldLoc;
    ShaderUniform mViewLoc;
    ShaderUniform mProjLoc;
};

typedef RefPointer<TriangleMesh> TriangleMeshPtr; 

}

#endif