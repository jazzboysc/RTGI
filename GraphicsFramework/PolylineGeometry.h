//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PolylineGeometry_H
#define RTGI_PolylineGeometry_H

#include "FrameworkCommon.h"
#include "RenderObject.h"
#include "Camera.h"
#include "GPUResource.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2013
//----------------------------------------------------------------------------
class PolylineGeometry : public RenderObject
{
public:
    PolylineGeometry(Material* material, Camera* camera, 
        GLenum polylineType = GL_LINE_STRIP);
	virtual ~PolylineGeometry();

	// Implement base class interface.
    virtual void Render(int technique, int pass, SubRenderer* subRenderer = 0);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnEnableBuffers();
    virtual void OnDisableBuffers();

	// Load data from a ".dat" file and create its VBO.
	bool LoadFromFile(const std::string& fileName);

	// Load data from user specified memory location.
	void LoadFromMemory(int polylineCount, 
		std::vector<int>& polylineVertexCounts, int totalVertexCount, 
		GLfloat* vertexData);

	// Should be called after calling LoadFromFile or LoadFromMemory.
    void CreateDeviceResource(GPUDevice* device);
    virtual void OnGetShaderConstants();

	// Currently this is a cumbersome function, it release the current 
	// device resource and re-create it.
    void UpdateDeviceResource(GPUDevice* device);

	// Data accessors.
	GLuint GetVertexBuffer() const;
	int GetPolylineCount() const;
	int GetTotalVertexCount() const;
    void AttachPoint(GPUDevice* device, GLfloat x, GLfloat y);
    void AttachPolyLineStartPoint(GPUDevice* device, GLfloat x, GLfloat y);
	void SetWorldWindow(GLfloat left, GLfloat top, GLfloat right, 
		GLfloat bottom);
	int SearchClosedPoint(GLfloat x, GLfloat y, GLfloat* dist = 0);
    void UpdatePoint(GPUDevice* device, int index, GLfloat x, GLfloat y);
    void DeletePoint(GPUDevice* device, int index);
	mat4 GetWorldWindowTransform();

    GLfloat LineWidth;

protected:
    void CreateVertexBufferDeviceResource();
	void Reset();

	GLenum mPolylineType;
	int mPolylineCount;
	int mTotalVertexCount;
	std::vector<int> mPolylineVertexCounts;
	std::vector<GLfloat> mVertexData;

	float mWorldWindow[4]; // Left, Top, Right, Bottom;
	mat4 mWorldWindowTransform;
    mat4 mWorldTransform;

    Camera* mCamera;

    ShaderUniform mWorldLoc;
    ShaderUniform mViewLoc;
    ShaderUniform mProjLoc;
};

typedef RefPointer<PolylineGeometry> Polyline2GeometryPtr;

}

#endif