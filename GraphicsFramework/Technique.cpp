//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Technique.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Technique::Technique()
{
}
//----------------------------------------------------------------------------
Technique::~Technique()
{
	for( int i = 0; i < (int)mPasses.size(); ++i )
	{
		delete mPasses[i];
	}
}
//----------------------------------------------------------------------------
void Technique::AddPass(Pass* pass)
{
	assert( pass );
	mPasses.push_back(pass);
}
//----------------------------------------------------------------------------
unsigned int Technique::GetPassCount() const
{
	return (unsigned int)mPasses.size();
}
//----------------------------------------------------------------------------
Pass* Technique::GetPass(unsigned int i) const
{
	return mPasses[i];
}
//----------------------------------------------------------------------------
void Technique::CreateDeviceResource()
{
	for( int i = 0; i < (int)mPasses.size(); ++i )
	{
		mPasses[i]->CreateDeviceResource();
	}
}
//----------------------------------------------------------------------------