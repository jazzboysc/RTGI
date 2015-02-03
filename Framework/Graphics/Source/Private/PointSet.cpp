//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "PointSet.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PointSet::PointSet(Material* material)
	:
	RenderObject(material),
	mPointCount(0),
    mComponent(0)
{
}
//----------------------------------------------------------------------------
PointSet::~PointSet()
{
	glDeleteBuffers(1, &mVBO);
}
//----------------------------------------------------------------------------
void PointSet::Render(int technique, int pass)
{
    // Apply current rendering pass.
    mMaterial->Apply(technique, pass);
}
//----------------------------------------------------------------------------
void PointSet::CreateDeviceResource(GPUDevice* device)
{
    if( mVertexData.size() > 0 )
    {
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mPointCount*mComponent,
            (GLvoid*)&mVertexData[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    // Create shader programs.
    GeometryAttributes attributes;
    attributes.VBO = mVBO;
    attributes.IBO = 0;
    attributes.HasNormal = false;
    attributes.HasTCoord = false;
    attributes.VertexComponentCount = mComponent;
    mMaterial->CreateDeviceResource(device, &attributes);

    // Get shader constants here.
    OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void PointSet::LoadFromMemory(unsigned int pointCount, float* vertexData,
    unsigned int component)
{
    assert(pointCount > 0 && vertexData && component > 0);

	mPointCount = pointCount;
    mComponent = component;
    mVertexData.reserve(mPointCount*component);
	for( unsigned int i = 0; i < mPointCount; ++i )
	{
        for( unsigned int j = 0; j < component; ++j )
        {
            mVertexData.push_back(vertexData[i*component + j]);
        }
	}
}
//----------------------------------------------------------------------------