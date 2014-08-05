#include "AccumulationScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
AccumulationScreenQuad::AccumulationScreenQuad(Material* material)
	:
	ScreenQuad(material)
{
}
//----------------------------------------------------------------------------
AccumulationScreenQuad::~AccumulationScreenQuad()
{
}
//----------------------------------------------------------------------------
void AccumulationScreenQuad::OnUpdateShaderConstants(int, int)
{
}
//----------------------------------------------------------------------------
void AccumulationScreenQuad::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------