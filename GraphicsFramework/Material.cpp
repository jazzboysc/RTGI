//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Material.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Material::Material(MaterialTemplate* materialTemplate)
{
	mRenderObject = 0;
	mMaterialTemplate = materialTemplate;
}
//----------------------------------------------------------------------------
Material::~Material()
{
	mMaterialTemplate = 0;
}
//----------------------------------------------------------------------------
void Material::Apply(int technique, int pass)
{
	Technique* tech = mMaterialTemplate->GetTechnique(technique);
    assert( tech );

    Pass* p = tech->GetPass(pass);
    assert( p );
    
    p->Enable();
    mRenderObject->OnEnableBuffers();
    mRenderObject->OnUpdateShaderConstants();
    mRenderObject->OnRender();
    mRenderObject->OnDisableBuffers();
    p->Disable();
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