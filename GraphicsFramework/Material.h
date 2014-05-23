//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Material_H
#define RTGI_Material_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "MaterialTemplate.h"

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

	void Apply();
	void CreateDeviceResource();
	ShaderProgram* GetProgram(int technique, int pass);

protected:
	MaterialTemplatePtr mMaterialTemplate;

	friend class RenderObject;
	RenderObject* mRenderObject;

	int mTechnique;
};

typedef RefPointer<Material> MaterialPtr;

}

#endif