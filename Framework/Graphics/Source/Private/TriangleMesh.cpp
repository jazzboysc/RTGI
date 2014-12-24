//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "TriangleMesh.h"
#include "GeometryAttributes.h"
#include "GPUDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TriangleMesh::TriangleMesh(Material* material, Camera* camera)
	:
	RenderObject(material),
	mVertexCount(0),
	mFaceCount(0),
	mVertexComponentCount(0),
	mHasTCoord(false),
	mHasNormal(false),
	mWorldScale(1.0f, 1.0f, 1.0f)
{
    mTriangleMaxEdgeLength = 0.0f;
    IsQuad = false;
    InstanceCount = 1;
    IsIndirect = false;
    SetCamera(camera);
}
//----------------------------------------------------------------------------
TriangleMesh::~TriangleMesh()
{
    IndirectCommandBuffer = 0;

	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);
}
//----------------------------------------------------------------------------
void TriangleMesh::Render(int technique, int pass, SubRenderer* subRenderer)
{
	// Apply current rendering pass.
	mMaterial->Apply(technique, pass);
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
void TriangleMesh::OnRender(Pass* pass, PassInfo*)
{
    bool enableTessellation = pass->IsTessellationEnabled();

    if( enableTessellation )
    {
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        glDrawElements(GL_PATCHES, (GLsizei)mIndexData.size(),
            GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        if( !IsQuad )
        {
            if( !IsIndirect )
            {
                if( InstanceCount == 1 )
                {
                    glDrawElements(GL_TRIANGLES, (GLsizei)mIndexData.size(),
                        GL_UNSIGNED_SHORT, 0);
                }
                else
                {
                    // TODO:
                }
            }
            else
            {
                assert(IndirectCommandBuffer != 0);
                glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0);
            }
        }
        else
        {
            // TODO:
            // Modify TriangleMesh to support quad mesh and change the name to Mesh.
            glDrawArraysInstanced(GL_QUADS, 0, 4, 128);
        }
    }

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void TriangleMesh::OnUpdateShaderConstants(int technique, int pass)
{
    assert( technique == 0 && pass == 0 );
    
    GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc, mWorldTransform);
	if( mCamera )
	{
		glm::mat4 viewTrans = mCamera->GetViewTransform();
        GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc, viewTrans);

		glm::mat4 projTrans = mCamera->GetProjectionTransform();
        GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc, projTrans);
	}
}
//----------------------------------------------------------------------------
bool TriangleMesh::LoadFromFile(const std::string& fileName)
{
	std::ifstream fileData("PLY/" + fileName);
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
		glm::vec3 vertex;

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
	glm::vec3 bc = mModelSpaceBB.GetBoxCenter();
	glm::mat4 modelTrans = glm::translate(glm::mat4(), -bc);
	glm::vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = glm::vec4(mVertexData[i], 1.0f);
		tempV = modelTrans * tempV;
		mVertexData[i].x = tempV.x;
		mVertexData[i].y = tempV.y;
		mVertexData[i].z = tempV.z;
	}
	// Adjust bounding box.
	tempV = glm::vec4(mModelSpaceBB.Min, 1.0f);
	tempV = modelTrans * tempV;
	mModelSpaceBB.Min.x = tempV.x;
	mModelSpaceBB.Min.y = tempV.y;
	mModelSpaceBB.Min.z = tempV.z;
	tempV = glm::vec4(mModelSpaceBB.Max, 1.0f);
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
    
#ifdef RTGI_OUTPUT_MESH_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading mesh %s finished\n", 
        fileName.c_str());
#endif

	return true;
}
//----------------------------------------------------------------------------
bool RTGI::TriangleMesh::LoadFromMemory(std::vector<glm::vec3>& _vData,
										std::vector<unsigned short>& _iData,
										std::vector<glm::vec3>& _nData)
{
	mVertexData = _vData;
	mIndexData = _iData;
	mVertexNormalData.resize(_nData.size());
	for (int i = 0, e = _nData.size(); i < e; ++i)
	{
		mVertexNormalData[i] = -_nData[i];
	}
	this->mFaceCount = mIndexData.size() / 3;
	this->mVertexCount = mVertexData.size();
	mHasNormal = true;
	// Adjust bounding box.
	for (auto vertex : mVertexData)
	{
		// Update model space bounding box.
		mModelSpaceBB.Min.x = RTGI_MIN(mModelSpaceBB.Min.x, vertex.x);
		mModelSpaceBB.Min.y = RTGI_MIN(mModelSpaceBB.Min.y, vertex.y);
		mModelSpaceBB.Min.z = RTGI_MIN(mModelSpaceBB.Min.z, vertex.z);
		mModelSpaceBB.Max.x = RTGI_MAX(mModelSpaceBB.Max.x, vertex.x);
		mModelSpaceBB.Max.y = RTGI_MAX(mModelSpaceBB.Max.y, vertex.y);
		mModelSpaceBB.Max.z = RTGI_MAX(mModelSpaceBB.Max.z, vertex.z);
	}

	return true;
}
//----------------------------------------------------------------------------
void TriangleMesh::OnLoadFromFile()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::CreateDeviceResource(GPUDevice* device)
{
	// Create VBO and IBO.
	CreateIndexBufferDeviceResource();
	CreateVertexBufferDeviceResource();

	// Create shader programs.
	GeometryAttributes attributes;
	attributes.VBO = mVBO;
	attributes.IBO = mIBO;
	attributes.HasNormal = mHasNormal;
	attributes.HasTCoord = mHasTCoord;
	attributes.VertexComponentCount = mVertexComponentCount;
	mMaterial->CreateDeviceResource(device, &attributes);

	// Get shader constants here.
	OnGetShaderConstants();

    if( IsIndirect )
    {
        assert(IndirectCommandBuffer);
        IndirectCommandBuffer->Bind();
        DrawElementsIndirectCommand* bufferData = 
            (DrawElementsIndirectCommand*)IndirectCommandBuffer->Map(
            GL_WRITE_ONLY);
        assert( bufferData );
        bufferData->Count = (unsigned int)mIndexData.size();
        bufferData->PrimCount = 0;
        bufferData->FirstIndex = 0;
        bufferData->BaseVertex = 0;
        bufferData->BaseInstance = 0;
        IndirectCommandBuffer->Unmap();
    }
}
//----------------------------------------------------------------------------
void TriangleMesh::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mWorldLoc, "World");
    program->GetUniformLocation(&mViewLoc, "View");
    program->GetUniformLocation(&mProjLoc, "Proj");
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldTransform(const glm::mat4& worldTrans)
{
	mWorldTransform = worldTrans * Offset;
}
//----------------------------------------------------------------------------
glm::mat4 TriangleMesh::GetWorldTransform() const
{
	return mWorldTransform;
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldTranslation(const glm::vec3& translation)
{
	mWorldTransform[3][0] = translation.x;
	mWorldTransform[3][1] = translation.y;
	mWorldTransform[3][2] = translation.z;
}
//----------------------------------------------------------------------------
glm::vec3 TriangleMesh::GetWorldTranslation() const
{
	glm::vec3 res;
	res.x = mWorldTransform[3][0];
	res.y = mWorldTransform[3][1];
	res.z = mWorldTransform[3][2];
	return res;
}
//----------------------------------------------------------------------------
void TriangleMesh::SetWorldScale(const glm::vec3& scale)
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
glm::vec3 TriangleMesh::GetWorldScale() const
{
	return mWorldScale;
}
//----------------------------------------------------------------------------
void TriangleMesh::GenerateNormals()
{
	mVertexNormalData.clear();
	mVertexNormalData.resize(mVertexCount);

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
		glm::vec3& v1 = mVertexData[v1ID];
		glm::vec3& v2 = mVertexData[v2ID];
		glm::vec3& v3 = mVertexData[v3ID];

		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v1;
        glm::vec3 e3 = v3 - v2;
		glm::vec3 n = glm::cross(e1, e2);

        // Get max edge length.
        float lenE1 = glm::length(e1);
        float lenE2 = glm::length(e2);
        float lenE3 = glm::length(e3);
        float maxLen = RTGI_MAX(lenE1, RTGI_MAX(lenE2, lenE3));
        mTriangleMaxEdgeLength = RTGI_MAX(maxLen, mTriangleMaxEdgeLength);

		// Update vertex normals.
		mVertexNormalData[v1ID] += n;
		mVertexNormalData[v2ID] += n;
		mVertexNormalData[v3ID] += n;

		// Get face normal.
		n = glm::normalize(n);
		mFaceNormalData.push_back(n);
	}

	// Normalize vertex normals.
	for( int i = 0; i < mVertexCount; ++i )
	{
		mVertexNormalData[i] = glm::normalize(mVertexNormalData[i]);
	}

	mHasNormal = true;

	OnGenerateNormals();
}
//----------------------------------------------------------------------------
void TriangleMesh::OnGenerateNormals()
{
}
//----------------------------------------------------------------------------
void TriangleMesh::SetTCoord(int i, const glm::vec2& texCoord)
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
glm::vec2 TriangleMesh::GetTCoord(int i) const
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
glm::vec3 TriangleMesh::GetWorldVertex(int i) const
{
	assert( (int)mVertexData.size() > i );
	glm::vec4 temp = glm::vec4(mVertexData[i], 1.0f);
	temp = mWorldTransform * temp;
	glm::vec3 res;
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
AABB TriangleMesh::GetWorldSpaceBB() const
{
	AABB res;
	glm::vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = glm::vec4(mVertexData[i], 1.0f);
		tempV = mWorldTransform * tempV;
		res.Min.x = RTGI_MIN(res.Min.x, tempV.x);
		res.Min.y = RTGI_MIN(res.Min.y, tempV.y);
		res.Min.z = RTGI_MIN(res.Min.z, tempV.z);
		res.Max.x = RTGI_MAX(res.Max.x, tempV.x);
		res.Max.y = RTGI_MAX(res.Max.y, tempV.y);
		res.Max.z = RTGI_MAX(res.Max.z, tempV.z);
	}
	return res;
}
//----------------------------------------------------------------------------
float TriangleMesh::GetTriangleMaxEdgeLength() const
{
    return mTriangleMaxEdgeLength;
}
//----------------------------------------------------------------------------
void TriangleMesh::UpdateModelSpaceVertices(const glm::mat4& trans)
{
	glm::vec4 tempV;
	for( int i = 0; i < mVertexCount; ++i )
	{
		tempV = glm::vec4(mVertexData[i], 1.0f);
		tempV = trans * tempV;
		mVertexData[i].x = tempV.x;
		mVertexData[i].y = tempV.y;
		mVertexData[i].z = tempV.z;
	}
	// Adjust bounding box.
	tempV = glm::vec4(mModelSpaceBB.Min, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Min.x = tempV.x;
	mModelSpaceBB.Min.y = tempV.y;
	mModelSpaceBB.Min.z = tempV.z;
	tempV = glm::vec4(mModelSpaceBB.Max, 1.0f);
	tempV = trans * tempV;
	mModelSpaceBB.Max.x = tempV.x;
	mModelSpaceBB.Max.y = tempV.y;
	mModelSpaceBB.Max.z = tempV.z;

	GenerateNormals();
}
//----------------------------------------------------------------------------
void TriangleMesh::CreateVertexBufferDeviceResource()
{
	mVertexComponentCount = 3;
	if( mHasTCoord )
	{
		mVertexComponentCount += 2;
	}
	if( mHasNormal )
	{
		mVertexComponentCount += 3;
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
			sizeof(GLfloat)*mVertexCount*mVertexComponentCount, 
			(GLvoid*)&tempVB[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void TriangleMesh::CreateIndexBufferDeviceResource()
{
	assert( mIndexData.size() > 0 );
	if( mIndexData.size() > 0 )
	{
		glGenBuffers(1, &mIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(unsigned short)*mIndexData.size(), (GLvoid*)&mIndexData[0],
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------