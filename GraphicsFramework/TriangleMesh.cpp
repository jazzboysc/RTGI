//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "TriangleMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TriangleMesh::TriangleMesh(Material* material, Camera* camera)
	:
	RenderObject(material),
	mVertexCount(0),
	mFaceCount(0),
	mHasTCoord(false),
	mHasNormal(false),
	mWorldScale(1.0f, 1.0f, 1.0f),
	mCamera(camera)
{
	mWorldLoc = mViewLoc = mProjLoc = -1;
}
//----------------------------------------------------------------------------
TriangleMesh::~TriangleMesh()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);
	glDeleteVertexArrays(1, &mVAO);
}
//----------------------------------------------------------------------------
void TriangleMesh::Render(int technique, int pass)
{
	// Enable VAO, VBO and IBO.
	glBindVertexArray(mVAO);

	// Apply current rendering pass.
	mMaterial->Apply(technique, pass);

	// Disable VAO, VBO and IBO.
	glBindVertexArray(0);
}
//----------------------------------------------------------------------------
void TriangleMesh::OnEnableBuffers()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::OnDisableBuffers()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::OnRender()
{
	glDrawElements(GL_TRIANGLES, (GLsizei)mIndexData.size(), 
		GL_UNSIGNED_SHORT, 0);
}
//----------------------------------------------------------------------------
void TriangleMesh::OnUpdateShaderConstants(int technique, int pass)
{
    assert( technique == 0 && pass == 0 );
    
	glUniformMatrix4fv(mWorldLoc, 1, GL_TRUE, mWorldTransform);
	if( mCamera )
	{
		mat4 viewTrans = mCamera->GetViewTransform();
		glUniformMatrix4fv(mViewLoc, 1, GL_TRUE, viewTrans);

		mat4 projTrans = mCamera->GetProjectionTransform();
		glUniformMatrix4fv(mProjLoc, 1, GL_TRUE, projTrans);
	}
}
//----------------------------------------------------------------------------
bool TriangleMesh::LoadFromFile(const std::string& fileName)
{
	std::ifstream fileData(fileName);
    if( !fileData )
    {
        return false;
    }

	std::string curLine;
	std::string curValue;
	std::string::size_type uiBegin, uiEnd;

	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() > 14 )
		{
			curValue = curLine.substr(0, 14);
			if( curValue == "element vertex" )
			{
				break;
			}
		}
	}

	// Get vertex count.
	uiBegin = 14;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mVertexCount = atoi(curValue.c_str());
	mVertexData.reserve(mVertexCount);

	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() > 12 )
		{
			curValue = curLine.substr(0, 12);
			if( curValue == "element face" )
			{
				break;
			}
		}
	}

	// Get face count.
	uiBegin = 12;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mFaceCount = atoi(curValue.c_str());
	mIndexData.reserve(mFaceCount*3);

	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() >= 10 )
		{
			curValue = curLine.substr(0, 10);
			if( curValue == "end_header" )
			{
				break;
			}
		}
	}

	// Get vertex data.
	for( int i = 0; i < mVertexCount; ++i )
	{
		vec3 vertex;

		getline(fileData, curLine);
		while( curLine.size() == 0 || curLine[0] == '\n' )
		{
			getline(fileData, curLine);
		}
		uiBegin = 0;
		uiBegin = curLine.find_first_not_of(" ", uiBegin);
		uiEnd = curLine.find(" ", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		vertex.x = (float)atof(curValue.c_str());

		uiBegin = curLine.find_first_not_of(" ", uiEnd);
		uiEnd = curLine.find(" ", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		vertex.y = (float)atof(curValue.c_str());

		uiBegin = curLine.find_first_not_of(" ", uiEnd);
		uiEnd = curLine.find("\n", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		vertex.z = (float)atof(curValue.c_str());

		mVertexData.push_back(vertex);

		// Update model space bounding box.
		mModelSpaceBB.Min[0] = RTGI_MIN(mModelSpaceBB.Min[0], vertex[0]);
		mModelSpaceBB.Min[1] = RTGI_MIN(mModelSpaceBB.Min[1], vertex[1]);
		mModelSpaceBB.Min[2] = RTGI_MIN(mModelSpaceBB.Min[2], vertex[2]);
		mModelSpaceBB.Max[0] = RTGI_MAX(mModelSpaceBB.Max[0], vertex[0]);
		mModelSpaceBB.Max[1] = RTGI_MAX(mModelSpaceBB.Max[1], vertex[1]);
		mModelSpaceBB.Max[2] = RTGI_MAX(mModelSpaceBB.Max[2], vertex[2]);
	}

	// Adjust vertices based on the center of the model.
	vec3 bc = mModelSpaceBB.GetBoxCenter();
	mat4 modelTrans = Translate(-bc.x, -bc.y, -bc.z);
	vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = vec4(mVertexData[i], 1.0f);
		tempV = modelTrans * tempV;
		mVertexData[i].x = tempV.x;
		mVertexData[i].y = tempV.y;
		mVertexData[i].z = tempV.z;
	}
	// Adjust bounding box.
	tempV = vec4(mModelSpaceBB.Min, 1.0f);
	tempV = modelTrans * tempV;
	mModelSpaceBB.Min.x = tempV.x;
	mModelSpaceBB.Min.y = tempV.y;
	mModelSpaceBB.Min.z = tempV.z;
	tempV = vec4(mModelSpaceBB.Max, 1.0f);
	tempV = modelTrans * tempV;
	mModelSpaceBB.Max.x = tempV.x;
	mModelSpaceBB.Max.y = tempV.y;
	mModelSpaceBB.Max.z = tempV.z;

	// Get index data.
	for( int i = 0; i < mFaceCount; ++i )
	{
		unsigned short value;

		getline(fileData, curLine);
		while( curLine.size() == 0 || curLine[0] == '\n' )
		{
			getline(fileData, curLine);
		}
		uiBegin = 0;
		uiBegin = curLine.find_first_not_of(" ", uiBegin);
		uiEnd = curLine.find(" ", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		value = (unsigned short)atoi(curValue.c_str());
		// TODO:
		// Only support trangle for now.
		assert( value == 3);

		uiBegin = curLine.find_first_not_of(" ", uiEnd);
		uiEnd = curLine.find(" ", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		value = (unsigned short)atof(curValue.c_str());
		mIndexData.push_back(value);

		uiBegin = curLine.find_first_not_of(" ", uiEnd);
		uiEnd = curLine.find(" ", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		value = (unsigned short)atof(curValue.c_str());
		mIndexData.push_back(value);

		uiBegin = curLine.find_first_not_of(" ", uiEnd);
		uiEnd = curLine.find("\n", uiBegin);
		curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
		value = (unsigned short)atof(curValue.c_str());
		mIndexData.push_back(value);
	}

	OnLoadFromFile();
    
#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Loading mesh %s finished\n", fileName.c_str());
#endif

	return true;
}
//----------------------------------------------------------------------------
void TriangleMesh::OnLoadFromFile()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::CreateDeviceResource()
{
	// First create shader program used by this geometry object.
	mMaterial->CreateDeviceResource();
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

	// Create VAO for this object.
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Create VBO for this object.
	int vertexComponent = 3;
	if( mHasTCoord )
	{
		vertexComponent += 2;
	}
	if( mHasNormal )
	{
		vertexComponent += 3;
	}
	std::vector<float> tempVB;
	if( mVertexData.size() > 0 )
	{
		glGenBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);

		for( int i = 0; i < mVertexCount; ++i )
		{
			tempVB.push_back(mVertexData[i].x);
			tempVB.push_back(mVertexData[i].y);
			tempVB.push_back(mVertexData[i].z);

			if( mHasTCoord )
			{
				tempVB.push_back(mTCoordData[i].x);
				tempVB.push_back(mTCoordData[i].y);
			}

			if( mHasNormal )
			{
				tempVB.push_back(mVertexNormalData[i].x);
				tempVB.push_back(mVertexNormalData[i].y);
				tempVB.push_back(mVertexNormalData[i].z);
			}
		}
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(GLfloat)*mVertexCount*vertexComponent, 
			(GLvoid*)&tempVB[0], GL_STATIC_DRAW);
	}

	// Create IBO for this object.
	if( mIndexData.size() > 0 )
	{
		glGenBuffers(1, &mIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(unsigned short)*mIndexData.size(), (GLvoid*)&mIndexData[0],
			GL_STATIC_DRAW);
	}

	// Specify vertex attributes.
	if( !mHasNormal && !mHasTCoord )
	{
		GLuint loc = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else if( mHasNormal && !mHasTCoord )
	{
		GLuint loc = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), 0);

		loc = glGetAttribLocation(program, "vNormal");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), (void*)12);
	}
	else if( mHasNormal && mHasTCoord )
	{
		GLuint loc = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), 0);

		loc = glGetAttribLocation(program, "vTCoord");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), (void*)12);

		loc = glGetAttribLocation(program, "vNormal");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), (void*)20);
	}
	else
	{
		GLuint loc = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), 0);

		loc = glGetAttribLocation(program, "vTCoord");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 
			vertexComponent*sizeof(float), (void*)12);
	}

	glBindVertexArray(0);

	// Get shader constants here.
	OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void TriangleMesh::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mWorldLoc = glGetUniformLocation(program, "World");
	mViewLoc = glGetUniformLocation(program, "View");
	mProjLoc = glGetUniformLocation(program, "Proj");
}
//----------------------------------------------------------------------------
void TriangleMesh::SetCamera(Camera* camera)
{
	mCamera = camera;
}
//----------------------------------------------------------------------------
Camera* TriangleMesh::GetCamera() const
{
    return mCamera;
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldTransform(const mat4& worldTrans)
{
	mWorldTransform = worldTrans;
}
//----------------------------------------------------------------------------
mat4 TriangleMesh::GetWorldTransform() const
{
	return mWorldTransform;
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldTranslation(const vec3& translation)
{
	mWorldTransform[0][3] = translation.x;
	mWorldTransform[1][3] = translation.y;
	mWorldTransform[2][3] = translation.z;
}
//----------------------------------------------------------------------------
vec3 TriangleMesh::GetWorldTranslation() const
{
	vec3 res;
	res.x = mWorldTransform[0][3];
	res.y = mWorldTransform[1][3];
	res.z = mWorldTransform[2][3];
	return res;
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldScale(const vec3& scale)
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
vec3 TriangleMesh::GetWorldScale() const
{
	return mWorldScale;
}
//----------------------------------------------------------------------------
void TriangleMesh::GenerateNormals()
{
	mVertexNormalData.clear();
	mVertexNormalData.reserve(mVertexCount);
	for( int i = 0; i < mVertexCount; ++i )
	{
		mVertexNormalData.push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	mFaceNormalData.clear();
	mFaceNormalData.reserve(mFaceCount);
	for( int i = 0; i < mFaceCount; ++i )
	{
		unsigned short v1ID, v2ID, v3ID;

		// Get vertex indices.
		v1ID = mIndexData[i*3    ];
		v2ID = mIndexData[i*3 + 1];
		v3ID = mIndexData[i*3 + 2];

		// Get 3 vertices.
		vec3& v1 = mVertexData[v1ID];
		vec3& v2 = mVertexData[v2ID];
		vec3& v3 = mVertexData[v3ID];

		vec3 e1 = v2 - v1;
		vec3 e2 = v3 - v1;
		vec3 n = cross(e1, e2);

		// Update vertex normals.
		mVertexNormalData[v1ID] += n;
		mVertexNormalData[v2ID] += n;
		mVertexNormalData[v3ID] += n;

		// Get face normal.
		n = normalize(n);
		mFaceNormalData.push_back(n);
	}

	// Normalize vertex normals.
	for( int i = 0; i < mVertexCount; ++i )
	{
		mVertexNormalData[i] = normalize(mVertexNormalData[i]);
	}

	mHasNormal = true;

	OnGenerateNormals();
}
//----------------------------------------------------------------------------
void TriangleMesh::OnGenerateNormals()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::SetTCoord(int i, const vec2& texCoord)
{
	if( !mHasTCoord )
	{
		mTCoordData.resize(mVertexCount);
		mHasTCoord = true;
	}

	assert( mVertexCount > i );
	mTCoordData[i] = texCoord;
}
//----------------------------------------------------------------------------
vec2 TriangleMesh::GetTCoord(int i) const
{
	assert( (int)mTCoordData.size() > i );
	return mTCoordData[i];
}
//----------------------------------------------------------------------------
int TriangleMesh::GetVertexCount() const
{
	return mVertexCount;
}
//----------------------------------------------------------------------------
vec3 TriangleMesh::GetWorldVertex(int i) const
{
	assert( (int)mVertexData.size() > i );
	vec4 temp = vec4(mVertexData[i], 1.0f);
	temp = mWorldTransform * temp;
	vec3 res;
	res.x = temp.x;
	res.y = temp.y;
	res.z = temp.z;
	return res;
}
//----------------------------------------------------------------------------
AABB TriangleMesh::GetModelSpaceBB() const
{
	return mModelSpaceBB;
}
//----------------------------------------------------------------------------
void TriangleMesh::UpdateModelSpaceVertices(const mat4& trans)
{
	vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = vec4(mVertexData[i], 1.0f);
		tempV = trans * tempV;
		mVertexData[i].x = tempV.x;
		mVertexData[i].y = tempV.y;
		mVertexData[i].z = tempV.z;
	}
	// Adjust bounding box.
	tempV = vec4(mModelSpaceBB.Min, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Min.x = tempV.x;
	mModelSpaceBB.Min.y = tempV.y;
	mModelSpaceBB.Min.z = tempV.z;
	tempV = vec4(mModelSpaceBB.Max, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Max.x = tempV.x;
	mModelSpaceBB.Max.y = tempV.y;
	mModelSpaceBB.Max.z = tempV.z;

	GenerateNormals();
}
//----------------------------------------------------------------------------