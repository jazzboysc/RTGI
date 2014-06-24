//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ScreenQuad::ScreenQuad(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera)
{
}
//----------------------------------------------------------------------------
ScreenQuad::~ScreenQuad()
{
}
//----------------------------------------------------------------------------
void ScreenQuad::OnUpdateShaderConstants(int, int)
{
	// Nothing to do.
}
//----------------------------------------------------------------------------
void ScreenQuad::OnGetShaderConstants()
{
	// Nothing to do.
}
//----------------------------------------------------------------------------