//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

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
    RTGI_ASSERT( tech );

    Pass* pass = (Pass*)tech->GetPass(passNum);
    RTGI_ASSERT( pass );

	PassInfo* passInfo = mTechniqueInfo[techniqueNum]->GetPassInfo(passNum);
	RTGI_ASSERT( passInfo );
    
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
void Material::CreateDeviceResource(GPUDevice* device, 
    GeometryAttributes* geometryAttr)
{
	mMaterialTemplate->CreateDeviceResource(device);

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
	Pass* p = (Pass*)t->GetPass(pass);
	ShaderProgram* program = p->GetShaderProgram();
	return program;
}
//----------------------------------------------------------------------------