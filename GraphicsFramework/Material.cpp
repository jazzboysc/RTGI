//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Material.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Material::Material(MaterialTemplate* materialTemplate)
	:
	mTechnique(0)
{
	mMaterialTemplate = materialTemplate;
}
//----------------------------------------------------------------------------
Material::~Material()
{
	mMaterialTemplate = 0;
}
//----------------------------------------------------------------------------
void Material::Apply()
{
	Technique* technique = mMaterialTemplate->GetTechnique(mTechnique);

	unsigned int passCount = technique->GetPassCount();
	for( unsigned int i = 0; i < passCount; ++i )
	{
		Pass* pass = technique->GetPass(i);
		pass->Enable();

		mRenderObject->OnEnableBuffers();
		mRenderObject->OnUpdateShaderConstants();
		mRenderObject->OnRender();
		mRenderObject->OnDisableBuffers();

		pass->Disable();
	}
}
//----------------------------------------------------------------------------
void Material::CreateDeviceResource()
{
	mMaterialTemplate->CreateDeviceResource();
}
//----------------------------------------------------------------------------
ShaderProgram* Material::GetProgram(int technique, int pass)
{
	Technique* t = mMaterialTemplate->GetTechnique(technique);
	Pass* p = t->GetPass(pass);
	ShaderProgram* program = p->GetShaderProgram();
	return program;
}
//----------------------------------------------------------------------------