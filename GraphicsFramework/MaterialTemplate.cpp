//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "MaterialTemplate.h"

using namespace RTGI;

//----------------------------------------------------------------------------
MaterialTemplate::MaterialTemplate()
{
}
//----------------------------------------------------------------------------
MaterialTemplate::~MaterialTemplate()
{
	for( int i = 0; i < (int)mTechniques.size(); ++i )
	{
		delete mTechniques[i];
	}
}
//----------------------------------------------------------------------------
void MaterialTemplate::AddTechnique(Technique* technique)
{
	assert( technique );
	mTechniques.push_back(technique);
}
//----------------------------------------------------------------------------
unsigned int MaterialTemplate::GetTechniqueCount() const
{
	return (unsigned int)mTechniques.size();
}
//----------------------------------------------------------------------------
Technique* MaterialTemplate::GetTechnique(int i) const
{
	assert( i < (int)mTechniques.size() );

	return mTechniques[i];
}
//----------------------------------------------------------------------------
void MaterialTemplate::CreateDeviceResource(GPUDevice* device)
{
	for( int i = 0; i < (int)mTechniques.size(); ++i )
	{
		mTechniques[i]->CreateDeviceResource(device);
	}
}
//----------------------------------------------------------------------------