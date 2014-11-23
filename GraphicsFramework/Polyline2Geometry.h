//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Polyline2Geometry_H
#define RTGI_Polyline2Geometry_H

#include "FrameworkCommon.h"
#include "RenderObject.h"
#include "Camera.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2013
//----------------------------------------------------------------------------
class Polyline2Geometry : public RenderObject
{
public:
    Polyline2Geometry(Material* material, Camera* camera, 
        GLenum polylineType = GL_LINE_STRIP);
	virtual ~Polyline2Geometry();

	// Implement base class interface.
	virtual void Render(int technique, int pass);
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
	void CreateDeviceResource();
    virtual void OnGetShaderConstants();

	// Currently this is a cumbersome function, it release the current 
	// device resource and re-create it.
	void UpdateDeviceResource();

	// Data accessors.
	GLuint GetVertexBuffer() const;
	int GetPolylineCount() const;
	int GetTotalVertexCount() const;
	void AttachPoint(GLfloat x, GLfloat y);
	void AttachPolyLineStartPoint(GLfloat x, GLfloat y);
	void SetWorldWindow(GLfloat left, GLfloat top, GLfloat right, 
		GLfloat bottom);
	int SearchClosedPoint(GLfloat x, GLfloat y, GLfloat* dist = 0);
	void UpdatePoint(int index, GLfloat x, GLfloat y);
	void DeletePoint(int index);
	mat4 GetWorldWindowTransform();

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

    GLint mWorldLoc, mViewLoc, mProjLoc;
};

typedef RefPointer<Polyline2Geometry> Polyline2GeometryPtr;

}

#endif