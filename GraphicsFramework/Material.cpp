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
void Material::Apply(int techniqueNum, int passNum)
{
	Technique* tech = mMaterialTemplate->GetTechnique(techniqueNum);
    assert( tech );

    Pass* pass = tech->GetPass(passNum);
    assert( pass );

	PassInfo* passInfo = mTechniqueInfo[techniqueNum]->GetPassInfo(passNum);
	assert( passInfo );
    
	passInfo->Enable();
    pass->Enable();

    mRenderObject->OnEnableBuffers();
    mRenderObject->OnUpdateShaderConstants(techniqueNum, passNum);
    mRenderObject->OnRender(pass, passInfo);
    mRenderObject->OnDisableBuffers();

    pass->Disable();
	passInfo->Disable();
}
//----------------------------------------------------------------------------
void Material::CreateDeviceResource(GeometryAttributes* geometryAttr)
{
	mMaterialTemplate->CreateDeviceResource();

	unsigned int tcount = mMaterialTemplate->GetTechniqueCount();
	mTechniqueInfo.reserve(tcount);
	for( unsigned int i = 0; i < tcount; ++i )
	{
		Technique* technique = mMaterialTemplate->GetTechnique((int)i);
		TechniqueInfo* techInfo = new TechniqueInfo();
		techInfo->CreatePassInfo(technique, geometryAttr);
		mTechniqueInfo.push_back(techInfo);
	}
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