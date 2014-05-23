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
	virtual void Render();
	virtual void OnRender();
	virtual void OnUpdateShaderConstants();
	virtual void OnEnableBuffers();
	virtual void OnDisableBuffers();

	// Load data from a ".ply" file and create its VBO and IBO.
	bool LoadFromFile(const std::string& fileName);
	virtual void OnLoadFromFile();

	// Should be called after calling LoadFromFile or LoadFromMemory.
	void CreateDeviceResource();
	virtual void OnGetShaderConstants();

	void SetCamera(Camera* camera);
	void SetWorldTransform(const mat4& worldTrans);
	mat4 GetWorldTransform() const;
	void SetWorldTranslation(const vec3& translation);
	vec3 GetWorldTranslation() const;
	void SetWorldScale(const vec3& scale);
	vec3 GetWorldScale() const;

	void GenerateNormals();
	virtual void OnGenerateNormals();

	void SetTCoord(int i, vec2& texCoord);
	vec2 GetTCoord(int i) const;
	
	AABB GetModelSpaceBB() const;

	void UpdateModelSpaceVertices(const mat4& trans);

protected:
	AABB mModelSpaceBB;

	int mVertexCount;
	int mFaceCount;
	std::vector<vec3> mVertexData;
	std::vector<vec2> mTCoordData;
	std::vector<vec3> mVertexNormalData;
	std::vector<vec3> mFaceNormalData;
	std::vector<unsigned short> mIndexData;

	bool mHasTCoord;
	bool mHasNormal;

	mat4 mWorldTransform;
	vec3 mWorldScale;

	Camera* mCamera;

	GLuint mIBO;
	GLint mWorldLoc, mViewLoc, mProjLoc; 
};

typedef RefPointer<TriangleMesh> TriangleMeshPtr; 

}

#endif