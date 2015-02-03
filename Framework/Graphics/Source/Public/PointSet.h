//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PointSet_H
#define RTGI_PointSet_H

#include "RenderObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/19/2013
//----------------------------------------------------------------------------
class PointSet : public RenderObject
{
public:
	PointSet(Material* material);
	~PointSet();

	// Implement base class interface.
	void Render(int technique, int pass, SubRenderer* subRenderer = 0) override;
    virtual void OnRender(Pass* pass, PassInfo* passInfo);

	// Load data from user specified memory location.
    void LoadFromSystemMemory(unsigned int pointCount, float* vertexData,
        unsigned int component);

    void CreateDeviceResource(GPUDevice* device);
	void OnUpdateShaderConstants(int technique, int pass) override {};
	virtual void OnGetShaderConstants() {};

protected:
	unsigned int mPointCount;
    unsigned int mComponent;
	std::vector<float> mVertexData;
};

typedef RefPointer<PointSet> PointSetPtr;

}

#endif