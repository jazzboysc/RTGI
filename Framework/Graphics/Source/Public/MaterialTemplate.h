//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_MaterialTemplate_H
#define RTGI_MaterialTemplate_H


#include "RefObject.h"
#include "Technique.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class MaterialTemplate : public RefObject
{
public:
	MaterialTemplate();
	MaterialTemplate(Technique* _pTechnique);
	MaterialTemplate(std::initializer_list<Technique*> _techniques);
	~MaterialTemplate();

	void AddTechnique(Technique* technique);
	unsigned int GetTechniqueCount() const;
	Technique* GetTechnique(int i) const;

    void CreateDeviceResource(GPUDevice* device);

protected:
	std::vector<Technique*> mTechniques;
};

typedef RefPointer<MaterialTemplate> MaterialTemplatePtr;

}

#endif