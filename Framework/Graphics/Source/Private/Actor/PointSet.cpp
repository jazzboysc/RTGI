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
	glPointSize(2.0);
	glDrawArrays(GL_POINTS, 0, mPointCount);
}
//----------------------------------------------------------------------------
void PointSet::CreateDeviceResource(GPUDevice* device)
{
    size_t bufferSize = sizeof(float)*mPointCount*mComponent;
    mPrimitive->VB = new VertexBuffer();
    if( mVertexData.size() > 0 )
    {
        mPrimitive->VB->LoadFromSystemMemory(device, bufferSize,
            (void*)&mVertexData[0], BU_Static_Draw);
    }
    else
    {
        mPrimitive->VB->ReserveImmutableDeviceResource(device, bufferSize);
    }

    // Create shader programs.
    GeometryAttributes attributes;
    attributes.Prim = mPrimitive;
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