//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "TechniqueInfo.h"
#include "OpenGLResource.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TechniqueInfo::TechniqueInfo()
{
}
//----------------------------------------------------------------------------
TechniqueInfo::~TechniqueInfo()
{
}
//----------------------------------------------------------------------------
void TechniqueInfo::CreatePassInfo(Technique* technique, 
	GeometryAttributes* geometryAttr)
{
	unsigned int pcount = technique->GetPassCount();
	mPassInfoArray.reserve(pcount);
	for( unsigned int i = 0; i < pcount; ++i )
	{
		Pass* pass = (Pass*)technique->GetPass(i);
		ShaderProgram* program = pass->GetShaderProgram();
        OpenGLShaderProgramHandle* programHandle = 
            (OpenGLShaderProgramHandle*)program->GetProgramHandle();

		PassInfo* passInfo = new PassInfo();
		passInfo->Create(
            programHandle->mProgram,
			geometryAttr->Prim,
			geometryAttr->HasNormal,
			geometryAttr->HasTCoord,
			geometryAttr->VertexComponentCount);
		mPassInfoArray.push_back(passInfo);
	}
}
//----------------------------------------------------------------------------
PassInfo* TechniqueInfo::GetPassInfo(int i) const
{
	RTGI_ASSERT( i >= 0 && i < (int)mPassInfoArray.size());
	return mPassInfoArray[i];
}
//----------------------------------------------------------------------------