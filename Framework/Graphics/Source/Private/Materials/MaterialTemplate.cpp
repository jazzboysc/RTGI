//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "MaterialTemplate.h"

using namespace RTGI;

//----------------------------------------------------------------------------
MaterialTemplate::MaterialTemplate()
{
}

RTGI::MaterialTemplate::MaterialTemplate(Technique* _pTechnique)
{
	AddTechnique(_pTechnique);
}
//----------------------------------------------------------------------------
RTGI::MaterialTemplate::MaterialTemplate(
	std::initializer_list<Technique*> _techniques)
{
	for (auto i : _techniques)
	{
		AddTechnique(i);
	}
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
	RTGI_ASSERT( technique );
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
	RTGI_ASSERT( i < (int)mTechniques.size() );

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