//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TriangleMesh_H
#define RTGI_TriangleMesh_H

#include "FrameworkCommon.h"
#include "RenderObject.h"
#include "Camera.h"
#include "AABB.h"
#include "StructuredBuffer.h"

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
	virtual void Render(int technique, int pass);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnEnableBuffers();
	virtual void OnDisableBuffers();

	// Load data from a ".ply" file and create its VBO and IBO.
	bool LoadFromFile(const std::string& fileName);
	virtual void OnLoadFromFile();

	// Should be called after calling LoadFromFile or LoadFromMemory.
	void CreateDeviceResource();
	virtual void OnGetShaderConstants();

	void SetWorldTransform(const mat4& worldTrans);
	mat4 GetWorldTransform() const;
	void SetWorldTranslation(const vec3& translation);
	vec3 GetWorldTranslation() const;
	void SetWorldScale(const vec3& scale);
	vec3 GetWorldScale() const;

	void GenerateNormals();
	virtual void OnGenerateNormals();

	void SetTCoord(int i, const vec2& texCoord);
	vec2 GetTCoord(int i) const;

	int GetVertexCount() const;
	vec3 GetWorldVertex(int i) const;
	
	AABB GetModelSpaceBB() const;
	AABB GetWorldSpaceBB() const;

	void UpdateModelSpaceVertices(const mat4& trans);

    // TODO:
    // Modify TriangleMesh to support quad mesh and change the name to Mesh.
    bool IsQuad;

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
	std::vector<vec3> mVertexData;
	std::vector<vec2> mTCoordData;
	std::vector<vec3> mVertexNormalData;
	std::vector<vec3> mFaceNormalData;
	std::vector<unsigned short> mIndexData;

	bool mHasTCoord;
	bool mHasNormal;

	mat4 mWorldTransform;
	vec3 mWorldScale;

	GLuint mIBO;
	GLint mWorldLoc, mViewLoc, mProjLoc; 
};

typedef RefPointer<TriangleMesh> TriangleMeshPtr; 

}

#endif