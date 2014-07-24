#include "OITScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
OITScreenQuad::OITScreenQuad(Material* material)
	:
	ScreenQuad(material)
{
}
//----------------------------------------------------------------------------
OITScreenQuad::~OITScreenQuad()
{
	HeadPointerTexture = 0;
	GPUMemPool = 0;
}
//----------------------------------------------------------------------------
void OITScreenQuad::OnUpdateShaderConstants(int, int)
{
}
//----------------------------------------------------------------------------
void OITScreenQuad::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------