//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "PolylineGeometry.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PolylineGeometry::PolylineGeometry(Material* material, Camera* camera, 
    GLenum polylineType)
	:
	mPolylineType(polylineType),
	RenderObject(material),
    mCamera(camera)
{
	mPolylineCount = 0;
	mTotalVertexCount = 0;

	mWorldWindow[0] = 0.0f;
	mWorldWindow[1] = 0.0f;
	mWorldWindow[2] = 0.0f;
	mWorldWindow[3] = 0.0f;

    LineWidth = 1.0f;
}
//----------------------------------------------------------------------------
PolylineGeometry::~PolylineGeometry()
{
}
//----------------------------------------------------------------------------
void PolylineGeometry::Render(int technique, int pass, 
    SubRenderer* subRenderer)
{
    // Apply current rendering pass.
    mMaterial->Apply(technique, pass);
}
//----------------------------------------------------------------------------
void PolylineGeometry::Reset()
{
	mPolylineCount = 0;
	mTotalVertexCount = 0;
	mPolylineVertexCounts.resize(0);
	mVertexData.resize(0);
}
//----------------------------------------------------------------------------
bool PolylineGeometry::LoadFromFile(const std::string& fileName)
{
	Reset();

	std::ifstream fileData("PLY/" + fileName);
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
	mWorldWindowTransform = glm::ortho(mWorldWindow[0], 
		mWorldWindow[2], mWorldWindow[3], mWorldWindow[1]);

	// Get polyline count.
	getline(fileData, curLine);
	while( curLine.size() == 0 || curLine[0] == '\n' )
	{
		getline(fileData, curLine);
	}
	mPolylineCount = atoi(curLine.c_str());
	RTGI_ASSERT( mPolylineCount >= 0 );

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

	RTGI_ASSERT( mVertexData.size() == mTotalVertexCount*2 );
	fileData.close();

	return true;
}
//----------------------------------------------------------------------------
void PolylineGeometry::LoadFromMemory(int polylineCount, 
	std::vector<int>& polylineVertexCounts, int totalVertexCount, 
	GLfloat* vertexData)
{
	RTGI_ASSERT( polylineCount > 0 && 
		polylineCount == (int)polylineVertexCounts.size() &&
		totalVertexCount > 0  && vertexData );

	mPolylineCount = polylineCount;
	mPolylineVertexCounts = polylineVertexCounts;
	mTotalVertexCount = totalVertexCount;
    mVertexData.resize(mTotalVertexCount*3);
	for( int i = 0; i < mTotalVertexCount*3; ++i )
	{
		mVertexData[i] = vertexData[i];
	}
}
//----------------------------------------------------------------------------
void PolylineGeometry::CreateVertexBufferDeviceResource(GPUDevice* device)
{
    // Create a VBO for this object.
    if( mVertexData.size() > 0 )
    {
        size_t bufferSize = sizeof(float)*mVertexData.size();
        mPrimitive->VB = new VertexBuffer();
        mPrimitive->VB->LoadFromSystemMemory(device, bufferSize,
            (void*)&mVertexData[0], BU_Static_Draw);
    }
}
//----------------------------------------------------------------------------
void PolylineGeometry::CreateDeviceResource(GPUDevice* device)
{
    // Create VBO.
    CreateVertexBufferDeviceResource(device);

    // Create shader programs.
    GeometryAttributes attributes;
    attributes.Prim = mPrimitive;
    attributes.HasNormal = false;
    attributes.HasTCoord = false;
    attributes.VertexComponentCount = 3;
    mMaterial->CreateDeviceResource(device, &attributes);

    // Get shader constants here.
    OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void PolylineGeometry::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
	program->GetUniformLocation(&mWorldLoc, "World");
	program->GetUniformLocation(&mViewLoc, "View");
	program->GetUniformLocation(&mProjLoc, "Proj");
}
//----------------------------------------------------------------------------
void PolylineGeometry::UpdateDeviceResource(GPUDevice* device)
{
	CreateDeviceResource(device);
}
//----------------------------------------------------------------------------
int PolylineGeometry::GetPolylineCount() const
{
	return mPolylineCount;
}
//----------------------------------------------------------------------------
int PolylineGeometry::GetTotalVertexCount() const
{
	return mTotalVertexCount;
}
//----------------------------------------------------------------------------
void PolylineGeometry::AttachPoint(GPUDevice* device, GLfloat x, GLfloat y)
{
	if( mPolylineCount == 0 )
	{
		AttachPolyLineStartPoint(device, x, y);
		return;
	}

	mTotalVertexCount++;
	mPolylineVertexCounts[mPolylineCount - 1]++;
	mVertexData.push_back(x);
	mVertexData.push_back(y);

	UpdateDeviceResource(device);
}
//----------------------------------------------------------------------------
void PolylineGeometry::AttachPolyLineStartPoint(GPUDevice* device, GLfloat x, 
    GLfloat y)
{
	mPolylineCount++;
	mTotalVertexCount++;
	mPolylineVertexCounts.push_back(1);
	mVertexData.push_back(x);
	mVertexData.push_back(y);

	UpdateDeviceResource(device);
}
//----------------------------------------------------------------------------
void PolylineGeometry::SetWorldWindow(GLfloat left, GLfloat top, 
	GLfloat right, GLfloat bottom)
{
	mWorldWindow[0] = left;
	mWorldWindow[1] = top;
	mWorldWindow[2] = right;
	mWorldWindow[3] = bottom;

	mWorldWindowTransform = glm::ortho(mWorldWindow[0], 
		mWorldWindow[2], mWorldWindow[3], mWorldWindow[1]);
}
//----------------------------------------------------------------------------
int PolylineGeometry::SearchClosedPoint(GLfloat x, GLfloat y, GLfloat* dist)
{
	if( mTotalVertexCount == 0 )
	{
		return -1;
	}

	RTGI_ASSERT( (mTotalVertexCount<<1) == (int)mVertexData.size() );

	float minDist2 = glm::distance2(glm::vec2(x, y), glm::vec2(mVertexData[0], mVertexData[1]));
	int closed = 0;
	for( int i = 1; i < mTotalVertexCount; ++i )
	{
        float tempDist2 = glm::distance2(glm::vec2(x, y),
            glm::vec2(mVertexData[2 * i], mVertexData[2 * i + 1]));

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
void PolylineGeometry::UpdatePoint(GPUDevice* device, int index, GLfloat x, 
    GLfloat y)
{
	RTGI_ASSERT( index >= 0 && index < mTotalVertexCount );
	mVertexData[2*index    ] = x;
	mVertexData[2*index + 1] = y;

	UpdateDeviceResource(device);
}
//----------------------------------------------------------------------------
void PolylineGeometry::DeletePoint(GPUDevice* device, int index)
{
	RTGI_ASSERT( index >= 0 && index < mTotalVertexCount );

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

	UpdateDeviceResource(device);
}
//----------------------------------------------------------------------------
glm::mat4 PolylineGeometry::GetWorldWindowTransform()
{
	return mWorldWindowTransform;
}
//----------------------------------------------------------------------------
void PolylineGeometry::OnRender(Pass*, PassInfo*)
{
    GLint first = 0;
    for( int i = 0; i < mPolylineCount; ++i )
    {
    	if( mPolylineVertexCounts[i] == 1 )
    	{
    		glDrawArrays(GL_POINTS, first, 1);
    	}
    	else
    	{
            glLineWidth(LineWidth);
    		glDrawArrays(mPolylineType, first, mPolylineVertexCounts[i]);
    	}

    	first += mPolylineVertexCounts[i];
    }
}
//----------------------------------------------------------------------------
void PolylineGeometry::OnUpdateShaderConstants(int technique, int pass)
{
    RTGI_ASSERT(technique == 0 && pass == 0);

	mWorldLoc.SetValue(mWorldTransform);
    if( mCamera )
    {
        glm::mat4 viewTrans = mCamera->GetViewTransform();
		mViewLoc.SetValue(viewTrans);

        glm::mat4 projTrans = mCamera->GetProjectionTransform();
		mProjLoc.SetValue(projTrans);
    }
}
//----------------------------------------------------------------------------
void PolylineGeometry::OnEnableBuffers()
{
}
//----------------------------------------------------------------------------
void PolylineGeometry::OnDisableBuffers()
{
}
//----------------------------------------------------------------------------