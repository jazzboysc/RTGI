//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Technique.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Technique::Technique()
{
}

RTGI::Technique::Technique(Pass* _pPass)
{
	AddPass(_pPass);
}

//----------------------------------------------------------------------------
Technique::~Technique()
{
}
//----------------------------------------------------------------------------