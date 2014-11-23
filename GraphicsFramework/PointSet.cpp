//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "PointSet.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PointSet::PointSet(Material* material)
	:
	RenderObject(material),
	mPointCount(0)
{
	mWorldWindow[0] = 0.0f;
	mWorldWindow[1] = 0.0f;
	mWorldWindow[2] = 0.0f;
	mWorldWindow[3] = 0.0f;
}
//----------------------------------------------------------------------------
PointSet::~PointSet()
{
	glDeleteBuffers(1, &mVBO);
}
//----------------------------------------------------------------------------
void PointSet::Render(int technique, int pass)
{
	// TODO:
	//// Enable shader program.
	//mShaderProgram->Enable();

	//// Update shader constants.
	//glUniformMatrix4fv(mWorldWindowTransformLoc, 1, GL_FALSE, 
	//	mWorldWindowTransform);

	//// Enable VAO and VBO.
	//glBindVertexArray(mVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	//glDrawArrays(GL_POINTS, 0, mPointCount);

	//// Disable VAO and VBO.
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//// Disable shader program.
	//mShaderProgram->Disable();
}
//----------------------------------------------------------------------------
void PointSet::CreateDeviceResource()
{
	// TODO:
	//// First create shader program used by this geometry object.
	//mShaderProgram->CreateDeviceResource();
	//GLuint program = mShaderProgram->GetProgram();

	//// Create a VAO for this object.
	//glGenVertexArrays(1, &mVAO);
	//glBindVertexArray(mVAO);

	//// Create a VBO for this object.
	//if( mVertexData.size() > 0 )
	//{
	//	glGenBuffers(1, &mVBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mVertexData.size(), 
	//		(GLvoid*)&mVertexData[0], GL_STATIC_DRAW);
	//}

	//// Specify vertex attributes.
	//GLuint loc = glGetAttribLocation(program, "vPosition");
 //   glEnableVertexAttribArray(loc);
 //   glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindVertexArray(0);

	//// Get shader constants.
	//mWorldWindowTransformLoc = glGetUniformLocation(program, "Ortho2DMatrix");
}
//----------------------------------------------------------------------------
void PointSet::LoadFromMemory(int pointCount, GLfloat* vertexData)
{
	assert( pointCount > 0 && vertexData );

	mPointCount = pointCount;
	mVertexData.reserve(mPointCount<<2);
	for( int i = 0; i < mPointCount; ++i )
	{
		mVertexData.push_back(vertexData[2*i    ]);
		mVertexData.push_back(vertexData[2*i + 1]);
	}
}
//----------------------------------------------------------------------------
void PointSet::SetWorldWindow(GLfloat left, GLfloat top, GLfloat right, 
	GLfloat bottom)
{
	mWorldWindow[0] = left;
	mWorldWindow[1] = top;
	mWorldWindow[2] = right;
	mWorldWindow[3] = bottom;

	mWorldWindowTransform = Ortho2D(mWorldWindow[0], 
		mWorldWindow[2], mWorldWindow[3], mWorldWindow[1]);
}
//----------------------------------------------------------------------------