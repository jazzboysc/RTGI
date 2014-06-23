//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Polyline2Geometry.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Polyline2Geometry::Polyline2Geometry(Material* material, GLenum polylineType)
	:
	//mPolylineType(polylineType),
	RenderObject(material)
{
	mPolylineCount = 0;
	mTotalVertexCount = 0;

	mWorldWindow[0] = 0.0f;
	mWorldWindow[1] = 0.0f;
	mWorldWindow[2] = 0.0f;
	mWorldWindow[3] = 0.0f;
}
//----------------------------------------------------------------------------
Polyline2Geometry::~Polyline2Geometry()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}
//----------------------------------------------------------------------------
void Polyline2Geometry::Render(int technique, int pass)
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

	//GLint first = 0;
	//for( int i = 0; i < mPolylineCount; ++i )
	//{
	//	if( mPolylineVertexCounts[i] == 1 )
	//	{
	//		glDrawArrays(GL_POINTS, first, 1);
	//	}
	//	else
	//	{
	//		glDrawArrays(mPolylineType, first, mPolylineVertexCounts[i]);
	//	}

	//	first += mPolylineVertexCounts[i];
	//}

	//// Disable VAO and VBO.
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//// Disable shader program.
	//mShaderProgram->Disable();
}
//----------------------------------------------------------------------------
void Polyline2Geometry::Reset()
{
	mPolylineCount = 0;
	mTotalVertexCount = 0;
	mPolylineVertexCounts.resize(0);
	mVertexData.resize(0);
}
//----------------------------------------------------------------------------
bool Polyline2Geometry::LoadFromFile(const std::string& fileName)
{
	Reset();

	std::ifstream fileData(fileName);
    if( !fileData )
    {
        return false;
    }

	std::string curLine;
	std::string curValue;
	std::string::size_type uiBegin, uiEnd;

	// Skip the comments.
	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() > 0 && curLine[0] == '*' )
		{
			break;
		}
	}

	// Get world window params.
	getline(fileData, curLine);
	while( curLine.size() == 0 || curLine[0] == '\n' )
	{
		getline(fileData, curLine);
	}
	uiBegin = 0;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mWorldWindow[0] = (float)atof(curValue.c_str());

	uiBegin = uiEnd;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mWorldWindow[1] = (float)atof(curValue.c_str());

	uiBegin = uiEnd;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mWorldWindow[2] = (float)atof(curValue.c_str());

	uiBegin = uiEnd;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mWorldWindow[3] = (float)atof(curValue.c_str());

	// Set WorldWindowTransform.
	mWorldWindowTransform = Ortho2D(mWorldWindow[0], 
		mWorldWindow[2], mWorldWindow[3], mWorldWindow[1]);

	// Get polyline count.
	getline(fileData, curLine);
	while( curLine.size() == 0 || curLine[0] == '\n' )
	{
		getline(fileData, curLine);
	}
	mPolylineCount = atoi(curLine.c_str());
	assert( mPolylineCount >= 0 );

	// Get all polyline data.
	for( int i = 0; i < mPolylineCount; ++i )
	{
		getline(fileData, curLine);
		while( curLine.size() == 0 || curLine[0] == '\n' )
		{
			getline(fileData, curLine);
		}
		int vertexCount = atoi(curLine.c_str());
		mPolylineVertexCounts.push_back(vertexCount);

		mTotalVertexCount += vertexCount;

		// Get one polyline.
		for( int j = 0; j < vertexCount; ++j )
		{
			float x, y;

			getline(fileData, curLine);
			while( curLine.size() == 0 || curLine[0] == '\n' )
			{
				getline(fileData, curLine);
			}
			uiBegin = 0;
			uiBegin = curLine.find_first_not_of(" ", uiBegin);
			uiEnd = curLine.find(" ", uiBegin);
			curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
			x = (float)atof(curValue.c_str());
			mVertexData.push_back(x);

			uiBegin = curLine.find_first_not_of(" ", uiEnd);
			uiEnd = curLine.find("\n", uiBegin);
			curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
			y = (float)atof(curValue.c_str());
			mVertexData.push_back(y);
		}
	}

	assert( mVertexData.size() == mTotalVertexCount*2 );
	fileData.close();

	return true;
}
//----------------------------------------------------------------------------
void Polyline2Geometry::LoadFromMemory(int polylineCount, 
	std::vector<int>& polylineVertexCounts, int totalVertexCount, 
	GLfloat* vertexData)
{
	assert( polylineCount > 0 && 
		polylineCount == (int)polylineVertexCounts.size() &&
		totalVertexCount > 0  && vertexData );

	mPolylineCount = polylineCount;
	mPolylineVertexCounts = polylineVertexCounts;
	mTotalVertexCount = totalVertexCount;
	for( int i = 0; i < mTotalVertexCount*2; ++i )
	{
		mVertexData.push_back(vertexData[i]);
	}
}
//----------------------------------------------------------------------------
void Polyline2Geometry::CreateDeviceResource()
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
void Polyline2Geometry::UpdateDeviceResource()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	CreateDeviceResource();
}
//----------------------------------------------------------------------------
GLuint Polyline2Geometry::GetVertexBuffer() const
{
	return mVBO;
}
//----------------------------------------------------------------------------
int Polyline2Geometry::GetPolylineCount() const
{
	return mPolylineCount;
}
//----------------------------------------------------------------------------
int Polyline2Geometry::GetTotalVertexCount() const
{
	return mTotalVertexCount;
}
//----------------------------------------------------------------------------
void Polyline2Geometry::AttachPoint(GLfloat x, GLfloat y)
{
	if( mPolylineCount == 0 )
	{
		AttachPolyLineStartPoint(x, y);
		return;
	}

	mTotalVertexCount++;
	mPolylineVertexCounts[mPolylineCount - 1]++;
	mVertexData.push_back(x);
	mVertexData.push_back(y);

	UpdateDeviceResource();
}
//----------------------------------------------------------------------------
void Polyline2Geometry::AttachPolyLineStartPoint(GLfloat x, GLfloat y)
{
	mPolylineCount++;
	mTotalVertexCount++;
	mPolylineVertexCounts.push_back(1);
	mVertexData.push_back(x);
	mVertexData.push_back(y);

	UpdateDeviceResource();
}
//----------------------------------------------------------------------------
void Polyline2Geometry::SetWorldWindow(GLfloat left, GLfloat top, 
	GLfloat right, GLfloat bottom)
{
	mWorldWindow[0] = left;
	mWorldWindow[1] = top;
	mWorldWindow[2] = right;
	mWorldWindow[3] = bottom;

	mWorldWindowTransform = Ortho2D(mWorldWindow[0], 
		mWorldWindow[2], mWorldWindow[3], mWorldWindow[1]);
}
//----------------------------------------------------------------------------
int Polyline2Geometry::SearchClosedPoint(GLfloat x, GLfloat y, GLfloat* dist)
{
	if( mTotalVertexCount == 0 )
	{
		return -1;
	}

	assert( (mTotalVertexCount<<1) == (int)mVertexData.size() );

	float minDist2 = DistSquare2(x, y, mVertexData[0], mVertexData[1]);
	int closed = 0;
	for( int i = 1; i < mTotalVertexCount; ++i )
	{
		float tempDist2 = DistSquare2(x, y, 
			mVertexData[2*i], mVertexData[2*i + 1]);

		if( tempDist2 < minDist2 )
		{
			minDist2 = tempDist2;
			closed = i;
		}
	}

	if( dist )
	{
		*dist = minDist2;
	}

	return closed;
}
//----------------------------------------------------------------------------
void Polyline2Geometry::UpdatePoint(int index, GLfloat x, GLfloat y)
{
	assert( index >= 0 && index < mTotalVertexCount );
	mVertexData[2*index    ] = x;
	mVertexData[2*index + 1] = y;

	UpdateDeviceResource();
}
//----------------------------------------------------------------------------
void Polyline2Geometry::DeletePoint(int index)
{
	assert( index >= 0 && index < mTotalVertexCount );

	mTotalVertexCount--;
	mVertexData.erase(mVertexData.begin() + 2*index, 
		mVertexData.begin() + 2*index + 2);

	int rangeBegin = 0;
	int rangeEnd = 0;
	int i;
	for( i = 0; i < (int)mPolylineVertexCounts.size(); ++i )
	{
		rangeEnd = rangeBegin + mPolylineVertexCounts[i];
		if( rangeBegin <= index && index < rangeEnd )
		{
			mPolylineVertexCounts[i]--;
			break;
		}
		rangeBegin = rangeEnd;
	}

	if( mPolylineVertexCounts[i] == 0 )
	{
		// Remove the polyline.
		mPolylineCount--;
		mPolylineVertexCounts.erase(mPolylineVertexCounts.begin() + i);
	}

	UpdateDeviceResource();
}
//----------------------------------------------------------------------------
mat4 Polyline2Geometry::GetWorldWindowTransform()
{
	return mWorldWindowTransform;
}
//----------------------------------------------------------------------------