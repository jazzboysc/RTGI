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
void PointSet::Render(int technique, int pass, SubRenderer* subRenderer)
{
    // Apply current rendering pass.
    mMaterial->Apply(technique, pass);
}
//----------------------------------------------------------------------------
void PointSet::OnRender(Pass*, PassInfo*)
{ 
	//glVertexPointer(3, GL_FLOAT, 0, &mVertexData[0]);
	glPointSize(7.0);
	glDrawArrays(GL_POINTS, 0, mPointCount);
}
//----------------------------------------------------------------------------
void PointSet::CreateDeviceResource(GPUDevice* device)
{
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    if( mVertexData.size() > 0 )
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mPointCount*mComponent,
            (GLvoid*)&mVertexData[0], GL_STATIC_DRAW);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mPointCount*mComponent, 0, 
            GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
void PointSet::LoadFromSystemMemory(unsigned int pointCount, 
    float* vertexData, unsigned int component)
{
	mPointCount = pointCount;
    mComponent = component;
    mVertexData.reserve(mPointCount*component);

    if( vertexData )
    {
        for( unsigned int i = 0; i < mPointCount; ++i )
        {
            for( unsigned int j = 0; j < component; ++j )
            {
                mVertexData.push_back(vertexData[i*component + j]);
            }
        }
    }
}
//----------------------------------------------------------------------------