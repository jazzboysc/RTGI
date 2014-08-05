#ifndef RTGI_AccumulationScreenQuad_H
#define RTGI_AccumulationScreenQuad_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/03/2014
//----------------------------------------------------------------------------
class AccumulationScreenQuad : public ScreenQuad
{
public:
	AccumulationScreenQuad(Material* material);
	~AccumulationScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();
};

typedef RefPointer<AccumulationScreenQuad> AccumulationScreenQuadPtr;

}

#endif