//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Material_H
#define RTGI_Material_H


#include "RefObject.h"
#include "MaterialTemplate.h"
#include "TechniqueInfo.h"
#include "GeometryAttributes.h"

namespace RTGI
{

class RenderObject;
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class Material : public RefObject
{
public:
	Material(MaterialTemplate* materialTemplate);
	~Material();

	void Apply(int techniqueNum, int passNum);
    void CreateDeviceResource(GPUDevice* device, 
        GeometryAttributes* geometryAttr);
	ShaderProgram* GetProgram(int technique, int pass);

protected:
	MaterialTemplatePtr mMaterialTemplate;

	friend class RenderObject;
	RenderObject* mRenderObject;

	std::vector<TechniqueInfo*> mTechniqueInfo;
};

typedef RefPointer<Material> MaterialPtr;

}

#endif